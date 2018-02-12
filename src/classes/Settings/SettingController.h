
#include "cinder/app/App.h"
#include "cinder/font.h"

class SettingController{
    
    int mIndex=0;
    ci::Font mFont;
    
    ci::Color activeColor;
    ci::Color normalColor;
    
    ci::Color activeBGColor;
    ci::Color normalBGColor;
    
public:
    
    void setup();
    void draw();
    bool checkKeyDown(ci::app::KeyEvent event);
    
    
    
};
