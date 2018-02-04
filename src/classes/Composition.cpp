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
#include "cinder/Rand.h"
#include "cinder/Log.h"
#include "GlobalSettings.h"

#include <iostream>
#include <fstream>

using namespace ci;


void Composition::setup(std::shared_ptr<ci::nvg::Context> nanoVGContext,bool hasHistory){
    vg = nanoVGContext;
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


void Composition::newComposition(){
    
    mPath.clear();
    mDepths.clear();

    interpolatedPointsToSave.clear();
    
    mGifInputFiles.clear();
    mId = getDateString();
    mOutputFolder = getDocumentsDirectory().string() + "lineDancer/" + mId;
    
    clearFbo();
}


void Composition::newLine(ci::vec3 pressurePoint){
    
    mPath.clear();
    mDepths.clear();
    
    mPath.moveTo(vec2(pressurePoint.x,pressurePoint.y));
    mDepths.moveTo(vec2(pressurePoint.x,pressurePoint.z));
    
    lastDrawDistance = 0;
    minDistance = 0;
}


void Composition::endLine(){
    newGifStep();
}


void Composition::lineTo(ci::vec3 pressurePoint){
    mPath.lineTo(vec2(pressurePoint.x,pressurePoint.y));
    mDepths.lineTo(vec2(pressurePoint.x,pressurePoint.z));
    
    calculatePath(mPath,mDepths);
}


void Composition::setNewSize(ci::ivec2 size, float windowScale){
    mSize        = size;
    mWindowScale = windowScale;
    
    setFbo(mActiveFbo,size,windowScale);
}


void Composition::setFbo(ci::gl::FboRef& fbo,ci::ivec2 size,float windowScale){
    
    //setup fbo

    gl::Fbo::Format format;
    //format.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
    
    gl::enableAlphaBlending();
   // format.setSamples( 4 );
    fbo = gl::Fbo::create(size.x  * GS()->scale, size.y * GS()->scale ,format );
    
    clearFbo();

}


void Composition::drawInFbo(std::vector<ci::vec3>& points){
    
    gl::ScopedFramebuffer fbScp( mActiveFbo );
    gl::ScopedViewport fbVP (mActiveFbo->getSize());
    gl::setMatricesWindow( mActiveFbo->getSize() );

    gl::color(1, 1, 1, 1);

    if(points.size() > 0){
        BrushManagerSingleton::Instance()->drawBrush(points, 0.98);
    }
    
    gl::setMatricesWindow( ci::app::getWindowSize() );

}




void Composition::calculatePath(ci::Path2d& path,ci::Path2d& depths){
    
    
    float length = path.calcLength();
    if(length <= minDistance) return;
    
    
    float newDrawPosition = lastDrawDistance + minDistance;
    
    pointVec pointsToDraw;
    
    while(newDrawPosition + minDistance < length){
        
        float newTime = path.calcTimeForDistance(newDrawPosition);
        if(newDrawPosition == 0) newTime = 0;
        
        
        vec3 newPoint(path.getPosition(newTime),depths.getPosition(newTime).y);
        
        pointsToDraw.push_back(newPoint);
        // save them later to a file.
        interpolatedPointsToSave.push_back(newPoint);
       
        minDistance = fmax(.8f,(newPoint.z * .17 /GS()->scale));// (scale*10);
        
        lastDrawDistance = newDrawPosition;
        newDrawPosition = (lastDrawDistance + minDistance);
    }
    
    
    if(pointsToDraw.size() > 0){
        // emmit to other listner in this case network
        onNewPoints.emit(pointsToDraw);
        // draw the new points into the fbo.
        drawInFbo(pointsToDraw);
    }
}



void Composition::draw(){
    
    gl::color(1, 1, 1, 1);
    gl::draw(mActiveFbo->getColorTexture());
}



void Composition::finished(){
    
    // write interpolated points to a data file in the output folder
    std::string dataFilePath = mOutputFolder + "/data.txt";
    try{
        std::ofstream dataFile;
        
        dataFile.open(dataFilePath);

        for(vec3& p :  interpolatedPointsToSave){
            dataFile << p.x << "," << p.y << "," << p.z << std::endl;
        }
        
        dataFile.close();
    }catch(...){
        CI_LOG_E( "couldn't write to path: " + dataFilePath);
    }


    //writing out the gif file.
    std::vector<Magick::Image> frames;
    
    for(auto &i: mGifInputFiles){
            Magick::Image img;
            img.read(i);
            img.animationDelay(9);  // increase the delay if you want a slower gif.
            img.animationIterations(-1);
            frames.push_back(img);
    }
    
    try{
            std::string path = mOutputFolder + "/__" +  mId + "composition.gif";
            CI_LOG_I(path);
            Magick::writeImages(frames.begin(), frames.end(),path);
    }
    catch ( Magick::WarningConfigure & error)
    {
            CI_LOG_E( error.what());
    }
 
    
    
}

void Composition::clearFbo(){
    // clear the screen;
    if(!mActiveFbo) return;
    
    gl::ScopedFramebuffer fbScp( mActiveFbo );
    gl::clear( ColorA( 1, 1, 1, 1.0 ) );
}


void Composition::newGifStep(){
    
    // check if output folder exists
    if(!fs::exists(mOutputFolder)){
        fs::create_directories(mOutputFolder);
    }
    
    // write the current drawing to a png image
    std::string path = mOutputFolder + "/" + getStringWithLeadingZero(mImageLayerId,5) + ".gif";

    auto source = mActiveFbo->getColorTexture()->createSource();

    std::thread threadObj([=]{
     
            try{
                writeImage(path, source);
                mGifInputFiles.push_back(path);
            }catch(...){
                CI_LOG_E("error writing GIF image file: " + path);
            }
       
    });

    threadObj.detach();
    mImageLayerId++;
}


ci::gl::TextureRef Composition::getTexture(){
    return mActiveFbo->getColorTexture();
}



