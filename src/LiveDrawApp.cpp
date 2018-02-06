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
    
    bool   isPenDown;
    bool   isPenClose;
    bool   isDrawing;
    bool   isMouseOnly;
    bool   isCursorVisible;
    bool   isMovingPaper;
    
    float lastUpdateTime;
    
    vec2 penMoveStart;
    
    NetworkHelper   mNetworkHelper;
    
    Menu menu;
    PlayerLogo logo;
    
    std::shared_ptr<Composition>    mActiveComposition;
    
    
    ci::Anim<int> showGifSavedTimer;


    ci::mat4 screenMatrix;
    ci::vec3 localCoordinate;
    ci::vec2 zoomAnchor;
    
    int zoomDirection = 0;
    float zoomLevel;
    vec2 zoomCenterPoint;
    
public:
    
    void setup() override;

    void mouseDown( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void mouseMove( MouseEvent event ) override;

    
    void penDown(vec3 point,std::shared_ptr<Composition>& composition);
    void penMove(vec3 point,std::shared_ptr<Composition>& composition);
    void penUp(std::shared_ptr<Composition>& composition);
    void penHover(vec3 point,std::shared_ptr<Composition>& composition);
    
    
    void keyUp( KeyEvent event ) override;
    void keyDown( KeyEvent event ) override;
    
    void onWacomData(TabletData& data);
    void resize() override;
    
    void update() override;
    void draw() override;
    void drawInFbo();
    void drawGrid();
    void drawTextMessages();
    
    void setupComposition(std::shared_ptr<Composition>& composition,bool hasHistory = false);
    void toggleCursor();
    
    vec3 getLocalPoint(vec3& screenPoint);
};




void LineDancer::toggleCursor(){
    isCursorVisible = !isCursorVisible;
    
    if(isCursorVisible){ hideCursor();}
    else{ showCursor(); }
}


void LineDancer::setup()
{
    
    setFullScreen(true);

    
    showGifSavedTimer = -1;
    
    
    log::makeLogger<log::LoggerFileRotating>(getAppPath(), "lineDancer.%Y.%m.%d.log", true);
    CI_LOG_I("START application");

    
   // setWindowSize(1600, 800);

    CI_LOG_I("START ofxTablet");
    ofxTablet::start();
    ofxTablet::onData.connect(bind(&LineDancer::onWacomData,this,std::placeholders::_1));
    CI_LOG_I("finished ofxTablet");

    
    CI_LOG_I("SETUP nanogvg");
    mNanoVG = std::make_shared<nvg::Context>(nvg::createContextGL());
    auto  font = ci::app::getResourceDirectory().string() + "/Roboto-Regular.ttf";
    mNanoVG->createFont("standard", font);
    mNanoVG->fontFace("standard");

    menu.setup();
    menu.onNewCommand.connect([=](std::string command){
        if(command == "NEW LAYER"){
            mActiveComposition->saveLayer();
            mActiveComposition->clearScene();
        }
        else if(command == "CLEAR"){
            mActiveComposition->clearScene(true);
        }
        else if(command == "GIF"){
            mActiveComposition->finished();
            mActiveComposition->newComposition();
            
            timeline().apply(&showGifSavedTimer,1,11,3.0f);
        }
        
    });
    

    
    menu.onBrushSizeChanged.connect([&](float brushScale){
        BrushManagerSingleton::Instance()->brushScale = 120.0f * brushScale;
    });
    
    
    CI_LOG_I("finished menu setup");

    
    isPenDown =     false;
    isDrawing =     false;
    isMouseOnly =   false;
    isPenClose =    false;
    isCursorVisible  =false;
    zoomLevel = GS()->zoomLevel;
    zoomCenterPoint = ci::app::getWindowCenter();
    zoomAnchor = vec2(0.5,0.5);
    isMovingPaper = false;
    lastUpdateTime = ci::app::getElapsedSeconds();
    
    
    CI_LOG_I("SETUP brush");
    BrushManagerSingleton::Instance()->setup();
    

    CI_LOG_I("SETUP composition with FBO");
    setupComposition(mActiveComposition);
    

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
    composition->setup(GS()->compositionSize);

    // when the new points with correct spacing are calculated we send them to the other
    // clients we don't send rawpoints.
    composition->onNewPoints.connect([=] (pointVec p){
        mNetworkHelper.sendPoints(p, BrushManagerSingleton::Instance()->isEraserOn);
    });
}



void LineDancer::onWacomData(TabletData& data){
//    std::cout << " --- " << std::endl;
//    std::cout << data.pressure << std::endl;
//    std::cout << data.pointerType << std::endl;
//    std::cout << data.buttonMask << std::endl;

    
    lastDataPoint = data;
    
    isPenClose = data.in_proximity;
    BrushManagerSingleton::Instance()->isEraserOn = data.buttonMask >= 2;
    
    vec3 point(data.abs_screen[0] *getWindowWidth(), getWindowHeight() - data.abs_screen[1]*getWindowHeight(), data.pressure * BrushManagerSingleton::Instance()->brushScale);
    lastWacomPoint = point;
    
    
    bool isMenuHit = false;
    
    localCoordinate =  getLocalPoint(lastWacomPoint);
    
    
    // hovering
    penHover(point, mActiveComposition);
    
    if(data.pressure > 0 && !isPenDown){
        isPenDown = true;
        
        isMenuHit = menu.checkTouchDown(ci::vec2(point.x,point.y));
        
        if(!isMenuHit){
            penDown(localCoordinate,mActiveComposition);
        }else{
            return;
        }
    }
    
    
    
    // exit line
    if(data.pressure == 0 && isPenDown){
        isPenDown = false;
        isDrawing = false;
        
        isMenuHit = menu.checkTouchUp();
        
        if(!isMenuHit) penUp(mActiveComposition);
        return;
    }
    
    
    
    // add data
    if((isDrawing || isMovingPaper) && !isMenuHit){
        penMove(localCoordinate,mActiveComposition);
    }
}




void LineDancer::penDown(vec3 point,std::shared_ptr<Composition>& composition){
    if(isMovingPaper)
    {
        vec2 p2 = vec2(lastWacomPoint.x,lastWacomPoint.y);
        penMoveStart = p2;

        return;
    }
    
    isDrawing=true;
    composition->newLine(point);
}


void LineDancer::penMove(vec3 point,std::shared_ptr<Composition>& composition){
   
    if(isMovingPaper){
        vec2 p2 = vec2(lastWacomPoint.x,lastWacomPoint.y);
        vec2 div =(penMoveStart - p2) ;
        zoomCenterPoint -=div;
        penMoveStart = p2;
        return;
    }
    
    composition->lineTo(point);
}


void LineDancer::penUp(std::shared_ptr<Composition>&  composition){
    if(isMovingPaper) return;

    isDrawing = false;
    composition->endLine();
}


void LineDancer::penHover(vec3 point,std::shared_ptr<Composition>& composition){
    menu.checkHover(ci::vec2(point.x,point.y));
}

void LineDancer::keyDown( KeyEvent event ){
    
    bool calculateAnchor =false;
    
    if(!isMovingPaper&&  event.getCode() == event.KEY_SPACE ){
        isMovingPaper = true;
        penMoveStart = vec2(lastWacomPoint.x,lastWacomPoint.y);
    }
    else if(event.getCode() == event.KEY_v){

        zoomDirection = 1;
        calculateAnchor = true;
    }
    else if(event.getCode() == event.KEY_n){

        zoomDirection = -1;
        calculateAnchor = true;
    }
    
    
    if(calculateAnchor){
        ivec2 size = mActiveComposition->getTexture()->getSize();
        
        zoomCenterPoint = vec2(lastWacomPoint);
        vec3 localPointCapped = localCoordinate;
        localPointCapped.x = fmax(0,fmin(localPointCapped.x,size.x));
        localPointCapped.y = fmax(0,fmin(localPointCapped.y,size.y));
        
        zoomAnchor = vec2(localPointCapped.x / size.x , localPointCapped.y / size.y);
    }

}

void LineDancer::keyUp(KeyEvent event ){
    
    zoomDirection = 0;
    
    if(event.getCode() == event.KEY_SPACE){
        isMovingPaper = false;
    }
    else if(event.getChar() == 'f'){
        setFullScreen(!isFullScreen());
        if(isFullScreen() && !isMouseOnly) toggleCursor();
    }else if(event.getCode() == event.KEY_ESCAPE){
        quit();
    }else if(event.getCode() == event.KEY_TAB){
        mNetworkHelper.setNextGroup();
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
    
    lastWacomPoint = vec3(event.getPos(),10);
    localCoordinate = getLocalPoint(lastWacomPoint);

    if(!menu.checkTouchDown(ci::vec2(lastWacomPoint.x,lastWacomPoint.y))){
        penDown(localCoordinate, mActiveComposition);
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
    
    lastWacomPoint = vec3(event.getPos(), 10);
    localCoordinate =  getLocalPoint(lastWacomPoint);
    
    penMove(localCoordinate,mActiveComposition);
}


void LineDancer::mouseMove( MouseEvent event )
{
}


void LineDancer::update()
{
    const float div = ci::app::getElapsedSeconds() - lastUpdateTime;
    lastUpdateTime = ci::app::getElapsedSeconds();
    
    // provides smooth in & out zoom
    if(zoomDirection != 0){
        
        if(fabs(zoomDirection) < 2){
            zoomDirection *= 2.0;
        }
        
        zoomLevel += zoomDirection * div * 0.7;
        if(zoomLevel < 0.1) zoomLevel = 0.1;
    }

    menu.update();
    mNetworkHelper.update();
}


void LineDancer::resize()
{
    menu.setPosition(vec2(getWindowWidth()-90, 60));
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
  
    //gl::clear( ColorA( 1.0, 1.0, 0.0, 0.0 ) );
    gl::clear(ColorA(249.0f / 255.0f, 242.0f / 255.0f, 160.0f / 255.0f,0.0f));

    ivec2 size = mActiveComposition->getTexture()->getSize();
    
    // Drawing "the paper" at zoomlevel with offset.
    ci::gl::pushMatrices();

        ci::gl::translate(zoomCenterPoint.x, zoomCenterPoint.y, 0);
    
        ci::gl::scale(zoomLevel, zoomLevel);
        ci::gl::translate(-size.x  * zoomAnchor.x , -size.y * zoomAnchor.y , 0);

        mActiveComposition->draw();
        ci::gl::color(1,0,0);
    
        // get the screenmatrix when all the transformations on the "paper" (fbo) or done.
        screenMatrix = ci::gl::getModelViewProjection();
   

    ci::gl::popMatrices();

    
    // draw the screen pointer.
    if(BrushManagerSingleton::Instance()->isEraserOn) ci::gl::color(1, 0.0, 0.0);
    else ci::gl::color(0, 0.3, 1.0);
    ci::gl::drawStrokedCircle(vec2(lastWacomPoint.x,lastWacomPoint.y), 8, 2, 12);
    
    
    auto& vg = *mNanoVG;

    vg.beginFrame(getWindowSize(), getWindowContentScale());
    
    menu.draw(mNanoVG);
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


// some parts based on http://discourse.libcinder.org/t/screen-to-world-coordinates/1014/2
vec3 LineDancer::getLocalPoint(vec3& screenPoint){
    
    int w = ci::app::getWindowWidth();
    int h = ci::app::getWindowHeight();
    vec4 viewport = vec4( 0, h, w, -h ); // vertical flip is required
    
    vec3 localPoint = glm::unProject( screenPoint, mat4(), screenMatrix, viewport );
    localPoint.z = screenPoint.z;
    
    return localPoint;
}

void LineDancer::drawTextMessages(){
    
    const int time = ci::app::getElapsedSeconds();
    auto& vg = *mNanoVG;

    if(time < 6){
        vg.strokeColor(GS()->blue);
        vg.fillColor(GS()->blue);

        vg.fontFace("standard");
        vg.fontSize(40);
        vg.text(ci::app::getWindowCenter(), "KEYS:");
        vg.fontSize(30);

        vg.text(ci::app::getWindowCenter() +vec2(0,50), "toggle (f)ullscreen");
        vg.text(ci::app::getWindowCenter() +vec2(0,80), "toggle mouse (c)ursor on");
        vg.text(ci::app::getWindowCenter() +vec2(0,120), "toggle (m)ouse drawing on");
        vg.text(ci::app::getWindowCenter() +vec2(0,150), "<esc> exit");
    }



    if(showGifSavedTimer < 10 && showGifSavedTimer > 0){
        vg.strokeColor(GS()->blue);
        
        float flash = fabs(sin(ci::app::getElapsedSeconds() *10.0));
        
        vg.fillColor(ci::ColorA(GS()->blue.r,GS()->blue.g,GS()->blue.b,flash));
        vg.textAlign(NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE );
        
        vg.fontSize(160);
        
        vg.text(ci::app::getWindowCenter(), "GIF SAVED!");
    }
}




CINDER_APP(LineDancer, RendererGl(RendererGl::Options().stencil().msaa(0)),
         [](App::Settings *settings) { settings->setHighDensityDisplayEnabled(); })
