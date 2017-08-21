//
//  Menu.hpp
//  LiveDraw
//
//  Created by lab101 on 05/09/16.
//
//

#ifndef Menu_hpp
#define Menu_hpp

#include <stdio.h>
#include "cinder/Signals.h"
#include "ci_nanovg_gl.hpp"

#include "DotButton.hpp"

class Menu{
  
    std::shared_ptr<ci::nvg::Context> vg;
    ci::vec2 mPosition;
    
    ci::Rectf brushScaleBoxOrig;
    ci::Rectf brushScaleBoxOrigCurrent;
    bool isBrushHover;
    bool isPressed;
    
    
public:
    float brushScale;

    DotButton btnSave;
    DotButton btnUndo;
    DotButton btnGif;

    ci::signals::Signal<void(float)>    onBrushSizeChanged;
    
    void setup(std::shared_ptr<ci::nvg::Context> nanoVGContext);
    void update();
    void draw();
    bool checkTouchDown(ci::vec2 point);
    bool checkHover(ci::vec2 point);
    bool touchUp();
    
    void setPosition(ci::vec2 position);
    void setBrushScale(float newScale);
};




#endif /* Menu_hpp */
