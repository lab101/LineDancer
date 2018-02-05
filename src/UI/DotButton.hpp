//
//  DotButton.hpp
//  LiveDraw
//
//  Created by lab101 on 14/09/16.
//
//

#ifndef DotButton_hpp
#define DotButton_hpp
#include "ci_nanovg_gl.hpp"

#include <stdio.h>

class DotButton{

    std::shared_ptr<ci::nvg::Context> vg;

    ci::vec2 mPosition;
    bool isPressed;
    bool isHover;
    
    ci::Color mColor;
    
    
public:
    
    float mRadius;

    std::string mText;
    ci::signals::Signal<void()>         onPressed;

    DotButton(float radius, std::string text);

    void draw(std::shared_ptr<ci::nvg::Context> nanoVGContext);
    bool checkTouchDown(ci::vec2 point);
    bool checkHover(ci::vec2 point);
    
    bool touchUp();
    
    void setPosition(ci::vec2 position);
    void setColor(ci::Color color);

};

#endif /* DotButton_hpp */
