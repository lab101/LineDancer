#pragma once


#include "Singleton.hpp"
#include "cinder/app/App.h"
#include "cinder/Font.h"

class GlobalSettings{


public:


    float scale;
    float maxWidth;
    
	GlobalSettings();
   

};


typedef Singleton<GlobalSettings> GlobalSettingsSingleton;

inline GlobalSettings* GS(){
	return GlobalSettingsSingleton::Instance();
}
