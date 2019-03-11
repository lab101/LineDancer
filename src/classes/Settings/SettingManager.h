//
//  SettingManager.h
//
//  Created by Kris Meeusen on 17/09/15.
//
//
//  Usage
//
//
//      Setting<float>  globalVolume = Setting<float>("global audio volume", 0.1);
//      addSetting(&globalVolume);
//
//      std::cout << globalVolume.mValue;


#ifndef __VideoPlayer__SettingManager__
#define __VideoPlayer__SettingManager__

#include <stdio.h>
#include "cinder/Utilities.h"
#include "cinder/Filesystem.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"
#include "cinder/Signals.h"
#include "cinder/Json.h"




class SettingBase{
    std::string mKey;
    bool isTweakable;
    
protected:
public:
    
    
    std::string getKey(){
        return mKey;
    }
    
    void setKey(std::string key){
        mKey = key;
    }
    
    virtual std::string getValueAsString() { return "not implemented!"; };
    
    virtual ci::JsonTree toJson(){ return ci::JsonTree("data",""); };
    virtual void setFromJson(ci::JsonTree json){  };
    virtual void increaseStep(float multiplyier = 1){  };
    virtual void decreaseStep(float multiplyier = 1){  };
    
    
    
    
    // needed for tweakable settings
    // should find a better way to remove them here.
    virtual std::string getTweakableSettings(){ return ""; };
    virtual int  getSortOrder(){ return 0; };
    virtual std::string getCategory(){ return "";};
    
    
};





// SETTING tweakable
template <typename T>
class Setting : public SettingBase {
    
    T mValue;
    
    T mTweakableMin;
    T mTweakableMax;
    T mTweakableSteps;
    
    int mSortOrder = 0;
    std::string mCategory = "default";
    
    
public:
    
    Setting<T>(){}
    
    Setting<T>(std::string key, T defaultValue){
        Setting<T>::mValue = defaultValue;
        mTweakableMin = 0;
        mTweakableMax = defaultValue * 2;
        mTweakableSteps = 1;
        
        SettingBase::setKey(key);
    }
    
    Setting<T> setMin(T value){
        mTweakableMin = value;
        return *this;
    }
    
    Setting<T> setMax(T value){
        mTweakableMax = value;
        return *this;
    }
    
    Setting<T> setSteps(T value){
        mTweakableSteps = value;
        return *this;
    }
    
    Setting<T> setCategory(std::string value){
        mCategory = value;
        return *this;
    }
    
    Setting<T> setSortOrder(int value){
        mSortOrder = value;
        return *this;
    }
    
    int getSortOrder() override{
        return mSortOrder;
    }
    
    std::string getCategory() override{
        return mCategory;
    }
    
    void setValue(T value){
        mValue = value;
    }
    
    void increaseStep(float multiplyier) override{
        mValue += mTweakableSteps * multiplyier;
    }
    
    void decreaseStep(float multiplyier) override{
        mValue -= mTweakableSteps * multiplyier;
    }
    
    T& value(){
        return mValue;
    }
    
    
    // overrride / type specific stuff
    
    void setFromJson(ci::JsonTree value) override{
        mValue = value.getValue<T>();
    }
    
    
    ci::JsonTree toJson() override{
        ci::JsonTree json = ci::JsonTree(getKey(), mValue);
        return json;
    }
    
    
    std::string getValueAsString() override {
        return ci::toString(mValue);
    };
    
    
 
    
};









class SettingManager{
    std::vector<SettingBase*> settings;
    //map with setting from the settings file form disk.
    ci::JsonTree storedSettingsJson;
    
    
    static SettingManager* instance;
    
    
    
    
public:

    std::string const getSettingPath(){
        return ci::getDocumentsDirectory().string() + "lineDancer/lineDancer_settings.json";
    }
    
    static SettingManager* Instance();
    
    
    std::vector<SettingBase*> getSettings(){
        return settings;
    }
    
    SettingManager(){
        readSettings();
    }
    
    
    void setSettingByJSON(ci::JsonTree data);
   
    void addSetting(SettingBase* setting);
    
    std::string getSettingStringDataByKey(std::string key)
    {
        
        for (SettingBase* s : settings){
            if (s->getKey() == key){
                //FIX//return s->valueToString();
                break;
            }
        }
        
        CI_LOG_E("SETTING NOT FOUND " + key);
        return "";
    }
    
    
    void readSettings();
    void writeSettings();
   
   
    
    
};




inline void addSetting(SettingBase* setting){
    SettingManager::Instance()->addSetting(setting);
}






#endif /* defined(__VideoPlayer__SettingManager__) */
