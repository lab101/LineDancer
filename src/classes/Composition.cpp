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
    interpolatedPointsToSave.clear();
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
    
    mPath.clear();
    mDepths.clear();
    
    mPath.moveTo(vec2(pressurePoint.x   ,pressurePoint.y));
    mDepths.moveTo(vec2(pressurePoint.x ,pressurePoint.z));
    
    lastDrawDistance = 0;
    minDistance = 0;
}


void Composition::endLine(){

    if(GS()->hasGifOutput){
        saveLineSegmentForGif();
    }
    mStepId++;
}


void Composition::lineTo(ci::vec3 pressurePoint){
    mPath.lineTo(vec2(pressurePoint.x,pressurePoint.y));
    mDepths.lineTo(vec2(pressurePoint.x,pressurePoint.z));
    
    calculatePath(mPath,mDepths);
}




void Composition::setFbo(ci::gl::FboRef& fbo,ci::ivec2 size,float windowScale){
    
    gl::Fbo::Format format;
    //format.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
    
    gl::enableAlphaBlending();
   // format.setSamples( 4 );
    fbo = gl::Fbo::create(size.x, size.y ,format );
    
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
       
        minDistance = fmax(.8f,(newPoint.z * .17));
        
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

    if(mLastDrawingTexture){
        gl::color(1, 1, 1, 0.3);
        gl::draw(mLastDrawingTexture);
    }
    
    gl::color(1, 1, 1, 1);

    ci::gl::enableAdditiveBlending();
    //GL_BLEND(GL_ONE,GL_ONE_MINUS_DST_ALPHA);
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
   // glBlendFunc(GL_ONE,GL_ONE_MINUS_DST_ALPHA);
    
    ci::gl::pushMatrices();
  //  ci::gl::translate(10, 10, 0);
    gl::draw(mActiveFbo->getColorTexture());
    ci::gl::enableAlphaBlending();
    ci::gl::popMatrices();
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


}


void Composition::writeDataFile(){
    
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
    
    for( fs::directory_iterator it( mOutputFolder); it != fs::directory_iterator(); ++it ){
        {
            if( is_directory( *it )){
                std::cout << "layer: " << it->path() << std::endl;
                std::vector<std::string> stepImages;

                // found a layer folder now read the files;
                for( fs::directory_iterator it2( it->path()); it2 != fs::directory_iterator(); ++it2 ){
                    {
                        if(it2->path().extension() == ".gif"){
                            std::cout << "step: " << it2->path() << std::endl;
                            stepImages.push_back(it2->path().string());
                            
                        }
                        
                    }
                }
                
                framesToGif(stepImages, mOutputFolder + "/layer_" + getStringWithLeadingZero(layerImages.size(),3) + "_final.gif");
                layerImages.push_back(stepImages.back());
            }
        }
    }
    // only write a layered final if user has been using layers
    if(layerImages.size() > 1){
        framesToGif(layerImages, mOutputFolder + "/_" + mCompositionId + "_final.gif");
    }
    
}


void Composition::clearFbo(){
    // clear the screen;
    if(!mActiveFbo) return;
    
    gl::ScopedFramebuffer fbScp( mActiveFbo );
    gl::clear( ColorA( 1, 1, 1, 1.0 ) );
}


void Composition::saveLineSegmentForGif(){
    // check if output folder exists
    if(!fs::exists(mOutputFolder + "/layers/")){
        fs::create_directories(mOutputFolder);
    }
    
    // write the current drawing to a png image
    std::string path = mOutputFolder + "/layer_" + getStringWithLeadingZero(mImageLayerId, 3)+ "/" + getStringWithLeadingZero(mStepId, 3) + ".gif";
    
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



