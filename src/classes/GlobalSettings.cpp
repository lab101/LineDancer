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
    
    hasGifOutput = Setting<bool>("hasGifOutput", true);
    hasClearButton = Setting<bool>("hasClearButton",true);
    hasLayerButton = Setting<bool>("hasLayerButton",true);
    doFadeOut = Setting<bool>("doFadeout",false);
    zoomLevel = Setting<float>("zoomlevel",0.9).setSteps(0.05);
    debugMode = Setting<bool>("debugMode",false);
    maxGroups = Setting<int>("maxGroups",2);
    
    
    performanceDownScale = Setting<float>("performanceDownScale",1.0).setSteps(0.5);
    
    compositionWidth = Setting<int>("compositionWidth",850).setSteps(10);
    compositionHeight = Setting<int>("compositionHeight",850).setSteps(10);
    
    addSetting(&hasGifOutput);
    addSetting(&hasClearButton);
    addSetting(&hasLayerButton);
    addSetting(&doFadeOut);
    addSetting(&zoomLevel);
    addSetting(&performanceDownScale);
    addSetting(&compositionWidth);
    addSetting(&compositionHeight);
    addSetting(&debugMode);
    addSetting(&maxGroups);
    
    compositionSize = ci::ivec2(compositionWidth.value(),compositionHeight.value());
    
    fadeoutFactor = 0.01f;
    
    blue = ci::Color(0,0.6,1.0);
    
    fboBackground = ci::ColorA(1.0,1.0,1.0,1.0);
    brushColor = ci::ColorA(0.0, 0.0,.0,1.0);
    
    
    fadeoutFactorDrawing = Setting<float>("fadeoutFactorDrawing", 1).setMin(0).setMax(1000).setSteps(1);
    
    fadeoutFactorReplay = Setting<float>("fadeoutFactorReplay", 4).setMin(0).setMax(1000).setSteps(1);
    
    
    replayStartTime = Setting<int>("replayStartTime", 10).setMin(0).setMax(60 * 10).setSteps(1);
    
    addSetting(&fadeoutFactorDrawing);
    addSetting(&fadeoutFactorReplay);
    addSetting(&replayStartTime);
    
    
    float displayScale =  ci::app::getWindowContentScale();
    
    mLargeFont = gl::TextureFont::create( Font( "Helvetica", 72 * displayScale ), gl::TextureFont::Format().enableMipmapping() );
    mSmallFont = Font( "Helvetica", 10 * displayScale );
    
    
}
std::vector<string> GlobalSettings::readColors(){
    
    std::string colorPath = ci::getDocumentsDirectory().string() + "lineDancer/colors.json";
    
    std::vector<string> colorsReturn;
    
    
    if (!fs::exists(colorPath)){
        CI_LOG_E("CAN'T FIND COLOR SETTING FILE " + colorPath);
        writeColors();
        
    }
    
    std::ifstream colorInput(colorPath.c_str());
    if (!colorInput.is_open())
    {
        CI_LOG_E("CAN'T OPEN COLOR SETTING FILE " + colorPath);
        
    }
    else{
        string file_contents { istreambuf_iterator<char>(colorInput), istreambuf_iterator<char>() };
        
        
        ci::JsonTree colorJson =JsonTree(file_contents);
        
        for(int i = 0; i< colorJson.getNumChildren(); i++){
            colorsReturn.push_back(colorJson[i].getValue());
        }
    }
    
    
    
    
    
    return colorsReturn;
}


void GlobalSettings::writeColors(){
    vector<std::string> defaultColors;
    defaultColors.push_back("#FFFFFF");
    defaultColors.push_back("#112F41");
    defaultColors.push_back("#F2B134");
    defaultColors.push_back("#ED553B");
    defaultColors.push_back("#000000");
    std::string colorPath = ci::getDocumentsDirectory().string() + "lineDancer/colors.json";
    
    JsonTree settingsJson = JsonTree::makeArray();
    
    
    
    for (std::string s : defaultColors){
        
        JsonTree color = JsonTree("",s);
        
        settingsJson.pushBack(color);
    }
    
    
    ofstream settingFile(colorPath, std::ios::out);
    settingFile << settingsJson.serialize();
    settingFile.close();
    
    CI_LOG_I("settings saved to " + colorPath);
}








