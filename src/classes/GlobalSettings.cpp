//
//  GlobalSettings.cpp
//  InnovationWall
//
//  Created by Kris Meeusen on 01/02/16.
//
//

#include <stdio.h>
#include "GlobalSettings.h"

#include "cinder/Utilities.h"

using namespace std;
using namespace ci;



GlobalSettings::GlobalSettings(){

    // in some cases the software can be used
    // to only send data not store, disabling output makes it faster.
    
    hasGifOutput = Setting<bool>("hasGifOutput",false);
    hasClearButton = Setting<bool>("hasClearButton",false);
    hasLayerButton = Setting<bool>("hasLayerButton",false);
    doFadeOut = Setting<bool>("doFadeout",false);
    zoomLevel = Setting<float>("zoomlevel",0.9).setSteps(0.05);
    
    compositionWidth = Setting<int>("compositionWidth",1200).setSteps(10);
    compositionHeight = Setting<int>("compositionHeight",800).setSteps(10);

    addSetting(&hasGifOutput);
    addSetting(&hasClearButton);
    addSetting(&hasLayerButton);
    addSetting(&doFadeOut);
    addSetting(&zoomLevel);
    addSetting(&compositionWidth);
    addSetting(&compositionHeight);

    compositionSize = ci::ivec2(compositionWidth.value(),compositionHeight.value());
    
    fadeoutFactor = 0.01f;
    
    blue = ci::Color(0,0.6,1.0);
    
    fboBackground = ci::ColorA(1.0,1.0,1.0,1.0);
    brushColor = ci::ColorA(1,1,1.0,1.0);

    
    fadeoutFactorDrawing = Setting<float>("fadeoutFactorDrawing", 1).setMin(0).setMax(1000).setSteps(1);
    
    fadeoutFactorReplay = Setting<float>("fadeoutFactorReplay", 4).setMin(0).setMax(1000).setSteps(1);
    
    
    replayStartTime = Setting<int>("replayStartTime", 10).setMin(0).setMax(60 * 10).setSteps(1);

    addSetting(&fadeoutFactorDrawing);
    addSetting(&fadeoutFactorReplay);
    addSetting(&replayStartTime);
    
    mLargeFont = gl::TextureFont::create( Font( "Helvetica", 72 * 2 ), gl::TextureFont::Format().enableMipmapping() );
    mSmallFont = Font( "Helvetica", 12 * 2 );


}








