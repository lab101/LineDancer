//
//  Composition.hpp
//  LiveDraw
//
//  Created by lab101 on 06/09/16.
//
//

#ifndef Composition_hpp
#define Composition_hpp

#include <stdio.h>
#include "ci_nanovg_gl.hpp"
#include "cinder/Signals.h"

//#define magick;
//
//
//#ifdef magick
#include <Magick++.h>
//#endif


typedef std::vector<ci::vec3> pointVec;

class Composition{
   
    std::string                 mId;
    std::string                 mOutputFolder;
    int                         mImageId;
    bool                        mHasHistory;
    
    ci::gl::FboRef              mActiveFbo;
    
    float                       mWindowScale;
    
    float lastDrawDistance      = 0;
    float minDistance           = 0;
    
    int lastUsedHistoryFboIndex = 0;
    int nrOfundosAvailable      = 0;
    std::shared_ptr<ci::nvg::Context> vg;
    

    // history
    std::vector<ci::gl::FboRef> mFboHistory;
    std::vector<ci::Path2d>     pointHistory;
    std::vector<ci::Path2d>     depthHistory;
    
    void takeSnapshotInFboHistory();
    
    ci::gl::TextureRef glitch;
    std::vector<std::string> imageList;
    


public:
    
    
    ci::signals::Signal<void(pointVec)>   onNewPoints;

    ci::vec2                    mSize;

    
    ci::vec3 getNormalisedPositionAtIndex(ci::Path2d& points, ci::Path2d depths,int index);
    ci::vec3 getNormalisedPositionAtIndex(int index);
    
    ci::Path2d mPath;
    ci::Path2d mDepths;
    
    void setup(std::shared_ptr<ci::nvg::Context> nanoVGContext, bool hasHistory = true);
    void newComposition();

    void setNewSize(ci::ivec2 size,float windowScale);
    void setFbo(ci::gl::FboRef& fbo, ci::ivec2 size,float windowScale);
    
    void drawInFbo(ci::Path2d& path,ci::Path2d& depths);
    void drawHistory();
    void draw();
    
    
    void savePointsToHistory();
    void newLine(ci::vec3 pressurePoint);
    void lineTo(ci::vec3 pressurePoint);
    void endLine();
    
    void historyBack();
    void historyForward();
    
    
    // clear history when finished
    ci::gl::TextureRef getTexture();
    void finished();
    void newLayer();
    void clearFbo();
    void drawFadeout();
    
    
    
};

#endif /* Composition_hpp */
