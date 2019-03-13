//
//  Composition.cpp
//  LiveDraw
//
//  Created by lab101 on 06/09/16.
//
//

#include "Composition.hpp"
#include "BrushManager.hpp"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "Lab101Utils.h"
#include "cinder/Log.h"
#include "GlobalSettings.h"

#include <iostream>
#include <fstream>

using namespace ci;


void Composition::setup(ivec2 size){
    
    //setup onion shader
    try{
        mOnionShader = gl::GlslProg::create(ci::app::loadResource( "onionLayer.vert" ), ci::app::loadResource( "onionLayer.frag" ));
    }
    catch( gl::GlslProgCompileExc ex ) {
        CI_LOG_E("error loading mask shader");
        CI_LOG_E(ex.what());
        exit(-1);
    }
    
    mSize        = size;
    setFbo(mActiveFbo,size, 1);
    
    newComposition();
}


ci::vec3 Composition::getNormalisedPositionAtIndex(int index){
    return getNormalisedPositionAtIndex(mPath,mDepths,index);
}


ci::vec3 Composition::getNormalisedPositionAtIndex(ci::Path2d& points, ci::Path2d depths,int index){
    ci::vec2 p = points.getPoints()[index] / mSize;
    ci::vec2 d = depths.getPoints()[index];
    
    return ci::vec3(p.x,p.y,d.y);
    
}


void Composition::clearScene(bool clearOnionLayer){
    
    if(!clearOnionLayer && mActiveFbo){
        auto source = mActiveFbo->getColorTexture()->createSource();
        mLastDrawingTexture = ci::gl::Texture::create(source);
    }
    
    mPath.clear();
    mDepths.clear();
    strokes.clear();
    clearFbo();
    
    if(clearOnionLayer && mActiveFbo){
        auto source = mActiveFbo->getColorTexture()->createSource();
        mLastDrawingTexture = ci::gl::Texture::create(source);
    }
    
}

void Composition::newComposition(){
    
    clearScene();
    
    if(mActiveFbo){
        auto source = mActiveFbo->getColorTexture()->createSource();
        mLastDrawingTexture = ci::gl::Texture::create(source);
    }
    
    mStepId =0;
    
    mGifInputFiles.clear();
    mCompositionId = getDateString();
    mOutputFolder = getDocumentsDirectory().string() + "lineDancer/" + mCompositionId;
}


void Composition::newLine(ci::vec3 pressurePoint){
    
    clearPath();
    
    mPath.moveTo(vec2(pressurePoint.x   ,pressurePoint.y));
    mDepths.moveTo(vec2(pressurePoint.x ,pressurePoint.z));
    
}


void Composition::endLine(){
    
    
    if(GS()->hasGifOutput.value()){
        saveLineSegmentForGif();
    }
    
    mStepId++;
    clearPath();
}


void Composition::clearPath(){
    
    mPath.clear();
    mDepths.clear();
    
    pointVec newStrokes;
    strokes.push_back(newStrokes);
    
    lastDrawDistance = 0;
    minDistance = 0;

}

void Composition::lineTo(ci::vec3 pressurePoint,ci::ColorA color){
    if(!isInsideComp(pressurePoint)) return;
    
    if(mPath.empty()){
        newLine(pressurePoint);
    }else{
    
        mPath.lineTo(vec2(pressurePoint.x,pressurePoint.y));
        mDepths.lineTo(vec2(pressurePoint.x,pressurePoint.z));
        calculatePath(mPath,mDepths,true,color);
    }
}

void Composition::drawCircle(ci::vec3 point1,ci::vec3 point2, bool recieved, ci::Color color ){
    
    //------------------------------------------------------------------------FBO
    gl::ScopedFramebuffer fbScp( mActiveFbo );
    gl::ScopedViewport fbVP (mActiveFbo->getSize());
    gl::setMatricesWindow( mActiveFbo->getSize() );
    
    gl::ScopedBlendPremult scpBlend;
    //------------------------------------------------------------------------DRAW
    gl::color(color);
    ci::gl::drawSolidCircle(vec2(point1.x,point1.y), glm::distance(point1, point2));
    
    gl::setMatricesWindow(ci::app::getWindowSize());//------------------------FBO END
    //------------------------------------------------------------------------DRAW STROKE
    std::vector<vec3> circumference;
    const int brushSize = point1.z;
    for(float i = 0; i< 362.0f ; i+=1){
        float x = point1.x + (glm::distance(point1, point2) * glm::cos(glm::radians(i)));
        float y = point1.y + (glm::distance(point1, point2) * glm::sin(glm::radians(i)));
        circumference.push_back(vec3(x,y,brushSize));
    }
    newLine(circumference[0]);
    for(int j =1 ; j< circumference.size();j++){
        mPath.lineTo(vec2(circumference[j].x,circumference[j].y));
        mDepths.lineTo(vec2(circumference[j].x,circumference[j].z));
        calculatePath(mPath,mDepths,false,color);
    }
    endLine();
    
    //-----------------------------------------------------------------------OSC
    if(!recieved ){
        std::vector<vec3> pointsToDrawNormalised;
        
        vec3 newPoint1 = vec3(point1);
        vec3 newPoint2 = vec3(point2);
        
        newPoint1.x /= mSize.x;
        newPoint1.y /= mSize.y;
        
        newPoint2.x /= mSize.x;
        newPoint2.y /= mSize.y;
        
        pointsToDrawNormalised.push_back(newPoint1);
        pointsToDrawNormalised.push_back(newPoint2);
        
        onNewCircle.emit(pointsToDrawNormalised);
    }
}


void Composition::drawLine(ci::vec3 point1,ci::vec3 point2 , bool recieved, ci::Color color){
    
    newLine(point1);
    mPath.lineTo(vec2(point2.x,point2.y));
    mDepths.lineTo(vec2(point2.x,point2.z));
    calculatePath(mPath,mDepths,false,color);
    
    //-----------------------------------------------------------------------OSC
    if(!recieved){
        std::vector<vec3> pointsToDrawNormalised;
        
        vec3 newPoint1 = vec3(point1);
        vec3 newPoint2 = vec3(point2);
        
        newPoint1.x /= mSize.x;
        newPoint1.y /= mSize.y;
        
        newPoint2.x /= mSize.x;
        newPoint2.y /= mSize.y;
        
        pointsToDrawNormalised.push_back(newPoint1);
        pointsToDrawNormalised.push_back(newPoint2);
        
        onNewLine.emit(pointsToDrawNormalised);
    }
    
}

void Composition::drawRectangle(ci::vec3 point1,ci::vec3 point2, bool recieved, ci::Color color){
    //------------------------------------------------------------------------FBO
    gl::ScopedFramebuffer fbScp( mActiveFbo );
    gl::ScopedViewport fbVP (mActiveFbo->getSize());
    gl::setMatricesWindow( mActiveFbo->getSize() );
    gl::ScopedBlendPremult scpBlend;
    //------------------------------------------------------------------------DRAW
     gl::color(color);
    Rectf rect( point1.x, point1.y, point2.x , point2.y);
    ci::gl::drawSolidRect(rect);
    
    gl::setMatricesWindow( ci::app::getWindowSize() );//----------------------FBO END
    //------------------------------------------------------------------------DRAW STROKE
   
    newLine(point1);
    mPath.lineTo(vec2(point2.x,point1.y));
    mDepths.lineTo(vec2(point2.x,point1.z));
  //  calculatePath(mPath,mDepths,false,color);
    mPath.lineTo(vec2(point2.x,point2.y));
    mDepths.lineTo(vec2(point2.x,point1.z));
   // calculatePath(mPath,mDepths,false,color);
    mPath.lineTo(vec2(point1.x,point2.y));
    mDepths.lineTo(vec2(point1.x,point1.z));
   // calculatePath(mPath,mDepths,false,color);
    mPath.lineTo(vec2(point1.x,point1.y));
    mDepths.lineTo(vec2(point1.x,point1.z));
    calculatePath(mPath,mDepths,false,color);
    endLine();
    //-----------------------------------------------------------------------OSC
    if(!recieved){
        std::vector<vec3> pointsToDrawNormalised;
        
        vec3 newPoint1 = vec3(point1);
        vec3 newPoint2 = vec3(point2);
        
        newPoint1.x /= mSize.x;
        newPoint1.y /= mSize.y;
        
        newPoint2.x /= mSize.x;
        newPoint2.y /= mSize.y;
        
        pointsToDrawNormalised.push_back(newPoint1);
        pointsToDrawNormalised.push_back(newPoint2);
        
        onNewRectangle.emit(pointsToDrawNormalised);
    }
}




void Composition::setFbo(ci::gl::FboRef& fbo,ci::ivec2 size,float windowScale){
    
    gl::Fbo::Format format;
    format.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
    
    gl::enableAlphaBlending();
    // format.setSamples( 4 );
    fbo = gl::Fbo::create(size.x, size.y ,format );
    
    clearFbo();
    
}


void Composition::drawInFbo(std::vector<ci::vec3>& points,ci::ColorA color){
    
    
    if(points.size() > 0){
        
        gl::ScopedFramebuffer fbScp( mActiveFbo );
        gl::ScopedViewport fbVP (mActiveFbo->getSize());
        gl::setMatricesWindow( mActiveFbo->getSize() );
        
        gl::ScopedBlendPremult scpBlend;
        
        
        gl::color(1, 1, 1, 1);
        
        BrushManagerSingleton::Instance()->drawBrush(points, 0.98,color);
        
        gl::setMatricesWindow( ci::app::getWindowSize() );
    }
    
}

void Composition::drawFadeOut(){
    
    
    
    gl::ScopedFramebuffer fbScp( mActiveFbo );
    gl::ScopedViewport fbVP (mActiveFbo->getSize());
    gl::setMatricesWindow( mActiveFbo->getSize() );
    
    
    // Enable pre-multiplied alpha blending.
    //  gl::ScopedBlendPremult scpBlend;
    
    ci::ColorA fade = GS()->fboBackground;
    fade.a = GS()->fadeoutFactor;
    gl::color(fade);
    ci::gl::drawSolidRect(Rectf(0,0, mActiveFbo->getSize().x, mActiveFbo->getSize().y));
    
    gl::setMatricesWindow( ci::app::getWindowSize() );
    
    
}

void Composition::emitShape(ci::vec3 point1 , ci::vec3 point2){
    
}


void Composition::calculatePath(ci::Path2d& path,ci::Path2d& depths, bool emmitTrueOrFalse, ci::ColorA color){
    
    float length = path.calcLength();
    if(length <= minDistance) return;
    
    float newDrawPosition = lastDrawDistance + minDistance;
    
    pointVec pointsToDraw;
    pointVec pointsToDrawNormalised;
    
    while(newDrawPosition + minDistance < length){
        
        float newTime = path.calcTimeForDistance(newDrawPosition);
        if(newDrawPosition == 0) newTime = 0;
        
        vec3 newPoint(path.getPosition(newTime),depths.getPosition(newTime).y);
        
        pointsToDraw.push_back(newPoint);
        
        minDistance = fmax(.8f,(newPoint.z * .17));
        
        lastDrawDistance = newDrawPosition;
        newDrawPosition = (lastDrawDistance + minDistance);
        
        // save them normalised later to a file.
        newPoint.x /= mSize.x;
        newPoint.y /= mSize.y;
        
        pointsToDrawNormalised.push_back(newPoint);
        strokes.back().push_back(newPoint);
        
    }
    
    if(pointsToDraw.size() > 0 ){
        // emmit to other listner in this case network
        if(emmitTrueOrFalse) onNewPoints.emit(pointsToDrawNormalised);
        // draw the new points into the fbo.
        drawInFbo(pointsToDraw,color);
    }
}



void Composition::draw(){
    
    
    gl::color(1, 1, 1, 1);
    
    ci::gl::enableAlphaBlending();
    
    ci::gl::pushMatrices();
    
    ci::gl::GlslProgRef textureShader = ci::gl::getStockShader(ci::gl::ShaderDef().texture().color());
    ci::gl::ScopedGlslProg glslProg(textureShader);
    
   // gl::ScopedGlslProg glslProg( mOnionShader );
   // mOnionShader->uniform( "uTex0", 0 );
   // mOnionShader->uniform( "uTex1", 1 );
    
    mActiveFbo->getColorTexture()->bind(0);
    mLastDrawingTexture->bind(1);
    
    gl::drawSolidRect(ci::Rectf(0,0,mActiveFbo->getWidth(),mActiveFbo->getHeight()));
    
    // for debugging only
    //    ci::gl::draw(mActiveFbo->getColorTexture());
    ci::gl::popMatrices();
    
    
}


void Composition::writeDataFile(){
    
    // write interpolated points to a data file in the output folder
    std::string dataFilePath = mOutputFolder + "/data.txt";
    try{
        std::ofstream dataFile;
        
        dataFile.open(dataFilePath);
        
        for(pointVec s :  strokes){
            for(vec3& p :  s){
                dataFile << p.x << "," << p.y << "," << p.z << ";";
            }
            dataFile <<  std::endl;
        }
        
        dataFile.close();
    }catch(...){
        CI_LOG_E( "couldn't write to path: " + dataFilePath);
    }
    
}


void Composition::framesToGif(std::vector<std::string>& inputFiles, std::string gifPath){
    
    //writing out the gif file.
    std::vector<Magick::Image> frames;
    
    for(auto &i: inputFiles){
        Magick::Image img;
        img.read(i);
        img.animationDelay(19);  // increase the delay if you want a slower gif.
        img.animationIterations(-1);
        frames.push_back(img);
    }
    
    try{
        CI_LOG_I("writing gif:" + gifPath);
        Magick::writeImages(frames.begin(), frames.end(), gifPath);
    }
    catch ( Magick::WarningConfigure & error)
    {
        CI_LOG_E( error.what());
    }
}



void Composition::finished(){
    
    writeDataFile();
    
    std::string path = mOutputFolder + "/__" +  mCompositionId + "composition.gif";
    
    std::vector<std::string> layerImages;
    
    if(ci::fs::exists(mOutputFolder)){
        for( fs::directory_iterator it( mOutputFolder); it != fs::directory_iterator(); ++it ){
            {
                if( is_directory( *it )){
                    std::string layerName  = it->path().filename().string();
                    std::vector<std::string> stepImages;
                    
                    // found a layer folder now read the files;
                    for( fs::directory_iterator it2( it->path()); it2 != fs::directory_iterator(); ++it2 ){
                        {
                            if(it2->path().extension() == ".gif"){
                                std::cout << "--- step: " << it2->path() << std::endl;
                                stepImages.push_back(it2->path().string());
                                
                            }
                            
                        }
                    }
                    std::cout << "-------" << std::endl;
                    std::cout << "---- writing ---" << std::endl;
                    std::sort (stepImages.begin(), stepImages.end());
                    framesToGif(stepImages, mOutputFolder + "/" + layerName + "_final.gif");
                    for(auto s : stepImages){
                        std::cout << s << std::endl;
                    }
                    layerImages.push_back(stepImages.back());
                    std::cout << "---- end ---" << std::endl; 
                }
            }
        }
    }
    // only write a layered final if user has been using layers
    if(layerImages.size() > 1){
        std::sort (layerImages.begin(), layerImages.end());
        framesToGif(layerImages, mOutputFolder + "/_" + mCompositionId + "_final.gif");
    }
    
}


void Composition::clearFbo(){
    // clear the screen;
    if(!mActiveFbo) return;
    
    gl::ScopedFramebuffer fbScp( mActiveFbo );
    gl::clear( GS()->fboBackground );
}


void Composition::saveLineSegmentForGif(){
    // check if output folder exists
    if(!fs::exists(mOutputFolder + "/layers/")){
        fs::create_directories(mOutputFolder);
    }
    
    // write the current drawing to a png image
    std::string path = mOutputFolder + "/layer_" + getStringWithLeadingZero(mImageLayerId, 5)+ "/" + getStringWithLeadingZero(mStepId, 5) + ".gif";
    
    writeGifStep(path);
    
}

void Composition::saveLayer(){
    mImageLayerId++;
    mStepId = 0;
}



void Composition::writeGifStep(std::string fileName){
    
    auto source = mActiveFbo->getColorTexture()->createSource();
    
    std::thread threadObj([=]{
        
        try{
            writeImage(fileName, source);
            mGifInputFiles.push_back(fileName);
        }catch(...){
            CI_LOG_E("error writing GIF image file: " + fileName);
        }
        
    });
    
    threadObj.detach();
}


ci::gl::TextureRef Composition::getTexture(){
    return mActiveFbo->getColorTexture();
}

bool Composition::isInsideComp(glm::vec3 point){
    if((0.0f < point.x && point.x  < mSize.x) && (0.0f < point.y && point.y  < mSize.y)) return true;
    newLine(point);
    return false;
}



