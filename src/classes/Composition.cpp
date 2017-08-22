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
#include <iostream>
#include <fstream>

using namespace ci;


void Composition::setup(std::shared_ptr<ci::nvg::Context> nanoVGContext,bool hasHistory){
    vg = nanoVGContext;
    
    mHasHistory = hasHistory;

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

    pointHistory.clear();
    depthHistory.clear();
    imageList.clear();
    mId = getDateString();
    mOutputFolder = getDocumentsDirectory().string() + "lineDancer/" + mId;
    lastUsedHistoryFboIndex =0;
    
    clearFbo();
}


void Composition::newLine(ci::vec3 pressurePoint){
    
    nrOfundosAvailable =  fmin(++nrOfundosAvailable,mFboHistory.size());
    if(mHasHistory) takeSnapshotInFboHistory();

    mPath.clear();
    mDepths.clear();
    
    mPath.moveTo(vec2(pressurePoint.x,pressurePoint.y));
    mDepths.moveTo(vec2(pressurePoint.x,pressurePoint.z));
    
    lastDrawDistance = 0;
    minDistance = 0;
}


void Composition::endLine(){
    savePointsToHistory();
    newLayer();
}


void Composition::historyBack(){

    if(nrOfundosAvailable <= 0) return;
    
    nrOfundosAvailable--;
    
    pointHistory.pop_back();
    depthHistory.pop_back();

      // go back in FBO's
    
    lastUsedHistoryFboIndex-=1;
    if(lastUsedHistoryFboIndex <0) lastUsedHistoryFboIndex += mFboHistory.size();
    if(lastUsedHistoryFboIndex >= mFboHistory.size()) lastUsedHistoryFboIndex = lastUsedHistoryFboIndex - mFboHistory.size();
    
    
    clearFbo();
    

    gl::ScopedFramebuffer fbScp( mActiveFbo );
    gl::ScopedViewport fbVP (mActiveFbo->getSize());
    gl::color(1, 1, 1);

    gl::enableAlphaBlending();
    gl::draw(mFboHistory[lastUsedHistoryFboIndex]->getColorTexture());

}

void Composition::takeSnapshotInFboHistory(){
    
    ci::gl::FboRef fbo = mFboHistory[lastUsedHistoryFboIndex];
    gl::ScopedFramebuffer fbScp( fbo );
    gl::ScopedViewport fbVP (mActiveFbo->getSize());

    gl::clear( ColorA( 0, 0, .0 ,0.0 ) );

    
    gl::draw(mActiveFbo->getColorTexture());

    if(++lastUsedHistoryFboIndex >=  mFboHistory.size()){
        lastUsedHistoryFboIndex = 0;
    }
    
}

void Composition::savePointsToHistory(){
    
   
    if(mPath.getNumPoints() > 0){
        pointHistory.push_back(mPath);
        depthHistory.push_back(mDepths);
    }
    
}

void Composition::lineTo(ci::vec3 pressurePoint){
    mPath.lineTo(vec2(pressurePoint.x,pressurePoint.y));
    mDepths.lineTo(vec2(pressurePoint.x,pressurePoint.z));
    
    drawInFbo(mPath,mDepths);
}


void Composition::setNewSize(ci::ivec2 size,float windowScale){
    mSize        = size;
    mWindowScale = windowScale;
    
    setFbo(mActiveFbo,size,windowScale);
    
    mFboHistory.clear();
    for(int i =0;i < 6;++i){
        ci::gl::FboRef historyFbo;
        setFbo(historyFbo, size, windowScale);

        mFboHistory.push_back(historyFbo);
    }
}


void Composition::setFbo(ci::gl::FboRef& fbo,ci::ivec2 size,float windowScale){
    
    //setup fbo

    gl::Fbo::Format format;
    //format.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
    
    gl::enableAlphaBlending();
   // format.setSamples( 4 );
    fbo = gl::Fbo::create(size.x,size.y ,format );
    
    clearFbo();

}






void Composition::drawInFbo(ci::Path2d& path,ci::Path2d& depths){
    
    gl::ScopedFramebuffer fbScp( mActiveFbo );
    gl::ScopedViewport fbVP (mActiveFbo->getSize());
//    gl::enableAlphaBlendingPremult();
   // gl::enableAlphaBlending();
    gl::color(1, 1, 1, 1);
    
    
    
    float length = path.calcLength();
    if(length <= minDistance) return;
    
    
    
    float newDrawPosition = lastDrawDistance + minDistance;
    
    std::vector<vec3> points;
    
    while(newDrawPosition + minDistance < length){
        
        float newTime = path.calcTimeForDistance(newDrawPosition);
        if(newDrawPosition == 0) newTime = 0;
        
        
        vec3 newPoint(path.getPosition(newTime),depths.getPosition(newTime).y);
        
        points.push_back(newPoint);
       
        minDistance = fmax(.8f,pow(newPoint.z * .17,1.2));// (scale*10);
        
        lastDrawDistance = newDrawPosition;
        newDrawPosition = (lastDrawDistance + minDistance);
        
    }
    
    if(points.size() > 0){
        BrushManagerSingleton::Instance()->drawBrush(points, 0.98);
    }
    


}


void Composition::draw(){
    
    gl::color(1, 1, 1, 1);
    gl::draw(mActiveFbo->getColorTexture());

}


void Composition::drawHistory(){


    for(int i = 0 ; i < mFboHistory.size(); i++){

        ci::gl::TextureRef tex =  mFboHistory[i]->getColorTexture();
        ci::vec2 size =  tex->getSize();

        float maxWidth = 200;
        float height = maxWidth * size.y / size.x;

        vec2 pos( maxWidth ,400 + height * i);

        ci::Rectf b(pos.x,pos.y,pos.x + maxWidth, pos.y +  height);
        
        ci::gl::draw(tex,b);

    }
}


void Composition::finished(){
    
    
   // try{
    // write data files
    std::ofstream dataFile;
    
    dataFile.open (mOutputFolder + "/data.txt");

    for(int i=0;i < pointHistory.size();++i){
        for(int pathIndex =0; pathIndex<  pointHistory[i].getNumPoints(); ++pathIndex){
            auto p =  getNormalisedPositionAtIndex(pointHistory[i], depthHistory[i], pathIndex);
            dataFile << p.x << "," << p.y << "," << p.z << std::endl;
        }
        

        dataFile << "--------------------" << std::endl;

    }
    dataFile.close();



        std::vector<Magick::Image> frames;
    
        for(auto i: imageList){
            Magick::Image img;
            img.read(i);
            img.animationDelay(8);
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
            // Handle problem while rotating or writing outfile.
            CI_LOG_E( error.what());
        }
 
    
        
        
    
    
}

void Composition::clearFbo(){
    // clear the screen;
    if(!mActiveFbo) return;
    
    gl::ScopedFramebuffer fbScp( mActiveFbo );
    
    gl::clear();
    gl::clear( ColorA( 0, 0, 0, 0.0 ) );
    

}

void Composition::newLayer(){
    
    

    // check if output folder exists
    if(!fs::exists(mOutputFolder)){
        fs::create_directories(mOutputFolder);
    }
    
    // write the current drawing to a png image
    std::string path = mOutputFolder + "/" + getStringWithLeadingZero(mImageId,5) +".gif";
    
    
    
    float scale = 0.25;
    
        auto f = ci::gl::Fbo::create(mActiveFbo->getWidth() * scale, mActiveFbo->getHeight() * scale);
        f->bindFramebuffer();
        gl::ScopedViewport scpVp( ivec2( 0 ), f->getSize() );
    
        gl::clear();
        gl::clear( ColorA( 1, 1, 1, 1.0 ) );
        
        gl::color(1, 1, 1, 1);
        gl::draw(mActiveFbo->getColorTexture());
        f->unbindFramebuffer();
        

    auto source = f->getColorTexture()->createSource();

    std::thread threadObj([=]{
     
            try{
                writeImage(path, source);
                imageList.push_back(path);
            }catch(...){
                CI_LOG_E("error writing image file: " + path);
            }
       
    });

    threadObj.detach();
    mImageId++;
}


ci::gl::TextureRef Composition::getTexture(){
    return mActiveFbo->getColorTexture();
}



