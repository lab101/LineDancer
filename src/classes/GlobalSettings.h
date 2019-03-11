#pragma once


#include "Singleton.hpp"
#include "cinder/app/App.h"
#include "cinder/Font.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Font.h"

#include "SettingManager.h"

class GlobalSettings{


public:


    float maxWidth;
    
    float fadeoutFactor;
    
    
    Setting<float> fadeoutFactorDrawing;
    Setting<float> fadeoutFactorReplay;

    Setting<int> replayStartTime;

    Setting<float> zoomLevel;
    Setting<float> performanceDownScale;
    Setting<int> compositionWidth;
    Setting<int> compositionHeight;
    Setting<int> maxGroups;

    
    Setting<bool>   hasGifOutput;
    Setting<bool>   hasClearButton;
    Setting<bool>   hasLayerButton;
    Setting<bool>   doFadeOut;
    Setting<bool>   debugMode;

    ci::ColorA blue;
    ci::ColorA fboBackground;
    ci::ColorA brushColor;
    std::string brushColorHex;

    ci::ivec2 compositionSize;
    ci::gl::TextureFontRef    mLargeFont;
    ci::Font    mSmallFont;

    GlobalSettings();
    

};



typedef Singleton<GlobalSettings> GlobalSettingsSingleton;

inline GlobalSettings* GS(){
	return GlobalSettingsSingleton::Instance();
}
