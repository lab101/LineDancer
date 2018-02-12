//
//  DotButton.hpp
//  LiveDraw
//
//  Created by lab101 on 14/09/16.
//
//

#ifndef DotButton_hpp
#define DotButton_hpp

#include <stdio.h>

class DotButton{

    ci::vec2 mPosition;
    bool isPressed;
    bool isHover;
    
    ci::Color mColor;
    ci::gl::TextureRef mTexture;
    
    ci::Rectf textBoundingScaled;
    
    void renderText();
    
public:
    
    float mRadius;

    std::string mText;
    ci::signals::Signal<void()>         onPressed;

    DotButton(float radius, std::string text);

    void draw();
    bool checkTouchDown(ci::vec2 point);
    bool checkHover(ci::vec2 point);
    
    bool touchUp();
    
    void setPosition(ci::vec2 position);
    void setColor(ci::Color color);

};

#endif /* DotButton_hpp */
