#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Timeline.h"
#include "ci_nanovg_gl.hpp"
#include "cinder/Log.h"

#include "ofxTablet.h"
#include "Line.hpp"
#include "Menu.hpp"
#include "Composition.hpp"
#include "BrushManager.hpp"
#include "PlayerLogo.hpp"

#include "NetworkHelper.hpp"
#include "GlobalSettings.h"


using namespace ci;
using namespace ci::app;
using namespace std;



class LineDancer : public App {
    
    std::shared_ptr<nvg::Context> mNanoVG;
    
    
    vec3 lastWacomPoint;
    TabletData lastDataPoint;
    
    
    bool   isHistoryVisible;
    bool   isPenDown;
    bool   isPenClose;
    bool   isDrawing;
    bool   isMouseOnly;
    bool   isOSCReceiver;
    bool   isCursorVisible;
    
    NetworkHelper   mNetworkHelper;
    
    Menu menu;
    PlayerLogo logo;
    
    ci::gl::TextureRef              mPrevTexture;
    std::shared_ptr<Composition>    mActiveComposition;
    
    
    ci::Anim<int> showGifSavedTimer;

    
public:
    
    void setup() override;

    void mouseDown( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    
    
    void penDown(vec3 point,std::shared_ptr<Composition>& composition);
    void penMove(vec3 point,std::shared_ptr<Composition>& composition);
    void penUp(std::shared_ptr<Composition>& composition);
    void penHover(vec3 point,std::shared_ptr<Composition>& composition);
    
    
    void keyUp( KeyEvent event ) override;
    void onWacomData(TabletData& data);
    void resize() override;
    
    void update() override;
    void draw() override;
    void drawInFbo();
    void drawGrid();
    void drawTextMessages();
    
    void setupComposition(std::shared_ptr<Composition>& composition,bool hasHistory = false);
    void toggleCursor();
};


void LineDancer::toggleCursor(){
    isCursorVisible = !isCursorVisible;
    
    if(isCursorVisible){ hideCursor();}
    else{ showCursor(); }
}




void LineDancer::setup()
{
    
    //setFullScreen(true);

    
    showGifSavedTimer = -1;
    
    
    log::makeLogger<log::LoggerFileRotating>(getAppPath(), "lineDancer.%Y.%m.%d.log", true);
    CI_LOG_I("START application");

    
    setWindowSize(1600, 800);

    CI_LOG_I("START ofxTablet");
    ofxTablet::start();
    ofxTablet::onData.connect(bind(&LineDancer::onWacomData,this,std::placeholders::_1));
    CI_LOG_I("finished ofxTablet");

    
    CI_LOG_I("SETUP nanogvg");
    mNanoVG = std::make_shared<nvg::Context>(nvg::createContextGL());
    auto  font = ci::app::getResourceDirectory().string() + "/Roboto-Regular.ttf";
    mNanoVG->createFont("standard", font);
    mNanoVG->fontFace("standard");

    menu.setup(mNanoVG);
    menu.btnSave.onPressed.connect([=]{
        mPrevTexture = ci::gl::Texture::create(mActiveComposition->getTexture()->createSource());
        
        mActiveComposition->newComposition();
    });
    
    
//    menu.btnUndo.onPressed.connect([=](){
//        mActiveComposition->historyBack();
//    });

    
    menu.btnGif.onPressed.connect([=](){
        mPrevTexture.reset();
        mActiveComposition->finished();
        mActiveComposition->newComposition();
        
        timeline().apply(&showGifSavedTimer,1,11,3.0f);
    });

    
    menu.onBrushSizeChanged.connect([&](float brushScale){
        BrushManagerSingleton::Instance()->brushScale = 120.0f * brushScale;
    });
    
    
    CI_LOG_I("finished menu setup");

    
    isPenDown =     false;
    isDrawing =     false;
    isMouseOnly =   false;
    isPenClose =    false;
    isHistoryVisible = false;
    isCursorVisible  =false;
    
    isOSCReceiver   = false;
    
    CI_LOG_I("SETUP brush");
    BrushManagerSingleton::Instance()->setup();
    

    CI_LOG_I("SETUP composition with FBO");
    setupComposition(mActiveComposition);
    
    resize();
    

    BrushManagerSingleton::Instance()->brushScale = 80.0f * menu.brushScale;
    BrushManagerSingleton::Instance()->mActiveColor = ColorA(0,0,0,1);
    

    if(mNetworkHelper.setup()){
        mNetworkHelper.onReceivePoints.connect([=] (std::vector<ci::vec3>& points, bool isEraserOn){
            BrushManagerSingleton::Instance()->isEraserOn = isEraserOn;
            mActiveComposition->drawInFbo(points);
        });
    }

    
    CI_LOG_I("finished SETUP");
    
}



void LineDancer::setupComposition(std::shared_ptr<Composition>& composition,bool hasHistory){
    
    
    composition = make_shared<Composition>();
    composition->setup(mNanoVG,hasHistory);
    composition->setNewSize(getWindowSize(),getWindowContentScale());
    
    composition->onNewPoints.connect([=] (pointVec p){
        mNetworkHelper.sendPoints(p, BrushManagerSingleton::Instance()->isEraserOn);

    });
}



void LineDancer::onWacomData(TabletData& data){
    // std::cout << data.pressure << std::endl;
    //    std::cout << data.pointerType << std::endl;
    lastDataPoint = data;
    
    isPenClose = data.in_proximity;
    BrushManagerSingleton::Instance()->isEraserOn = data.buttonMask >= 2;
    
    vec3 point(data.abs_screen[0] *getWindowWidth(), getWindowHeight() - data.abs_screen[1]*getWindowHeight(), data.pressure * BrushManagerSingleton::Instance()->brushScale * GS()->scale);
    lastWacomPoint = point;
    
    
    bool isMenuHit = false;
    
    // hovering
    penHover(point,mActiveComposition);
    
    if(data.pressure > 0 && !isPenDown){
        isPenDown = true;
        
        isMenuHit = menu.checkTouchDown(ci::vec2(point.x,point.y));
        
        if(!isMenuHit){
            penDown(point,mActiveComposition);
        }else{
            return;
        }
    }
    
    
    
    // exit line
    if(data.pressure == 0 && isPenDown){
        isPenDown = false;
        isDrawing = false;
        
        isMenuHit = menu.touchUp();
        
        if(!isMenuHit) penUp(mActiveComposition);
        return;
    }
    
    
    
    // add data
    if(isDrawing &&!isMenuHit){
        penMove(point,mActiveComposition);
    }
}




void LineDancer::penDown(vec3 point,std::shared_ptr<Composition>& composition){
    
    // check for menu
    isDrawing=true;
    point.x *= 1 / GS()->scale;
    point.y *= 1 / GS()->scale;
    composition->newLine(point);
}


void LineDancer::penMove(vec3 point,std::shared_ptr<Composition>& composition){
    point.x *= 1 / GS()->scale;
    point.y *= 1 / GS()->scale;
    composition->lineTo(point);
}


void LineDancer::penUp(std::shared_ptr<Composition>&  composition){
    isDrawing=false;
    composition->endLine();
    menu.touchUp();
}


void LineDancer::penHover(vec3 point,std::shared_ptr<Composition>& composition){
    menu.checkHover(ci::vec2(point.x,point.y));
}



void LineDancer::keyUp( KeyEvent event ){
    
    if(event.getChar() == 'f'){
        setFullScreen(!isFullScreen());
        if(isFullScreen() && !isMouseOnly) toggleCursor();
    }else if(event.getCode() == event.KEY_ESCAPE){
        quit();
    }else if(event.getCode() == event.KEY_TAB){
        mNetworkHelper.setNextGroup();
    }else if(event.getCode() == event.KEY_h){
        isHistoryVisible = !isHistoryVisible;
    }
    else if(event.getCode() == event.KEY_c){
        toggleCursor();
    }else if( event.getCode() == event.KEY_e){
        bool isEraserOn = BrushManagerSingleton::Instance()->isEraserOn;
        BrushManagerSingleton::Instance()->isEraserOn = !isEraserOn;
    }
    
    
    else if(event.getCode() == event.KEY_m){
        isMouseOnly = !isMouseOnly;
    }

}



void LineDancer::mouseDown( MouseEvent event )
{
    if(!isMouseOnly) return;
    
    lastWacomPoint = vec3(event.getPos(),6.1 +fabs(sin(getElapsedSeconds())));
    
    if(!menu.checkTouchDown(ci::vec2(lastWacomPoint.x,lastWacomPoint.y))){
        penDown(lastWacomPoint,mActiveComposition);
    }
    
    
}

void LineDancer::mouseUp( MouseEvent event )
{
    if(!isMouseOnly) return;
    
    lastWacomPoint = vec3(event.getPos(),10);
    penUp(mActiveComposition);
}


void LineDancer::mouseDrag( MouseEvent event )
{
    bool isMenuHit = menu.checkTouchDown(event.getPos());
    if((!isMouseOnly && !isMenuHit) || !isDrawing) return;
    
    lastWacomPoint = vec3(event.getPos(),10 +fabs(sin(getElapsedSeconds())));
    penMove(lastWacomPoint,mActiveComposition);
    
}




void LineDancer::update()
{
    menu.update();
    mNetworkHelper.update();
}


void LineDancer::resize()
{
    mActiveComposition->setNewSize(getWindowSize(),getWindowContentScale());
    menu.setPosition(vec2(getWindowWidth()-90,60));
}



void LineDancer::drawGrid(){
    
    int const stepSize = 80;
    ci::ivec2 size = getWindowSize();
    auto& vg = *mNanoVG;

    for(int x =stepSize * 0.5; x < size.x; x+=stepSize){
        for(int y = stepSize*0.5; y < size.y; y+=stepSize){
            
            vg.beginPath();
            vg.fillColor(ColorAf{0.8, .8f, .6f, 0.6});
            vg.circle(x, y, 2);
            
            vg.strokeWidth(0.7);
            vg.fill();
            
        }
    }
}


void LineDancer::draw()
{
  
    gl::clear();
    //gl::clear( ColorA( 0.3, 0.3, 1.0, 0.0 ) );
    gl::clear( ColorA( 1.0, 1.0, .0, 0.0 ) );

    mActiveComposition->draw();

    if(BrushManagerSingleton::Instance()->isEraserOn) ci::gl::color(1, 0.0, 0.0);
    else ci::gl::color(0, 0.3, 1.0);
    ci::gl::drawStrokedCircle(vec2(lastWacomPoint.x,lastWacomPoint.y), 8, 2, 12);

    
   // if(isHistoryVisible) mActiveComposition->drawHistory();
    
    
    auto& vg = *mNanoVG;

    vg.beginFrame(getWindowSize(), getWindowContentScale());
    
    menu.draw();
    logo.draw(false,vec2(30,30), ci::toString( mNetworkHelper.getGroupId()) + "|" + mNetworkHelper.getLastMyIpNr(), 0 ,vg);
    
    int i=0;
    for(auto client : mNetworkHelper.mAliveIps){
        i+=60;
        logo.draw(true,vec2(30, 30 + i), client.first, client.second,vg);
    }
    
    
    drawGrid();

    drawTextMessages();
    
    vg.endFrame();   
    
}




void LineDancer::drawTextMessages(){
    
    const int time = ci::app::getElapsedSeconds();
    auto& vg = *mNanoVG;

    if(time < 6){
        
        vg.fontFace("standard");
        vg.fontSize(40);
        vg.text(ci::app::getWindowCenter(), "KEYS:");
        vg.fontSize(30);
        vg.strokeColor(ci::Color(0,0,0));
        vg.fillColor(ci::Color(0,0,0));

        vg.text(ci::app::getWindowCenter() +vec2(0,50), "toggle (f)ullscreen");
        vg.text(ci::app::getWindowCenter() +vec2(0,80), "toggle mouse (c)ursor on");
        vg.text(ci::app::getWindowCenter() +vec2(0,120), "toggle (m)ouse drawing on");
        vg.text(ci::app::getWindowCenter() +vec2(0,150), "<esc> exit");
        
    }


    if(time > 5 && time < 12 ){
        vg.strokeColor(ci::Color(0,0,0));
        vg.fillColor(ci::Color(0,0,0));
        vg.textAlign(NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE );
        
        // warning
        vg.fontSize(60);
        vg.text(ci::app::getWindowCenter(), "Warning do not scale or resize the app");
        vg.text(ci::app::getWindowCenter()  + vec2(0,50), "during the performance");
        
    }

    if(time > 11 && time < 18 ){
        vg.strokeColor(ci::Color(0,0,0));
        vg.fillColor(ci::Color(0,0,0));
        vg.textAlign(NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE );
        
        vg.fontSize(60);
        
        vg.text(ci::app::getWindowCenter(), "HAVE FUN");
        
    }


    if(showGifSavedTimer < 10 && showGifSavedTimer > 0){
        vg.strokeColor(ci::Color(1,1,1));
        
        float flash = fabs(sin(ci::app::getElapsedSeconds() *10.0));
        vg.fillColor(ci::ColorA(1,1,1,flash));
        vg.textAlign(NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE );
        
        vg.fontSize(160);
        
        vg.text(ci::app::getWindowCenter(), "GIF SAVED!");
    }
}






//CINDER_APP( LiveDrawApp, RendererGl( RendererGl::Options().msaa( 0 ) ))

CINDER_APP(LineDancer, RendererGl(RendererGl::Options().stencil().msaa(0)),
         [](App::Settings *settings) { settings->setHighDensityDisplayEnabled(); })
