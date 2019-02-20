
//  TextButton.cpp
//  LiveDraw
//
//  Created by lab101 on 14/09/16.
//
//

#include "ColorButton.hpp"

using namespace ci;

ColorButton::ColorButton(float radius, ci::Color color){
    mColor      = color;
    mRadius     = radius;
}


void ColorButton::draw(){
    
    gl::pushMatrices();
    gl::translate(mPosition);
    
    
    gl::color(isPressed ? Color(0,0,0) : mColor);
    gl::drawSolidCircle(vec2(0,0), mRadius);
    
    if(!isPressed){
        gl::color(0,0,0);
        gl::drawStrokedCircle(vec2(0,0), mRadius,3,60 );
    }

    
    // DRAW children last
    for(BaseButton* button : mChildren){
        button->draw();
    }

    
    gl::color(1,1,1);
    
    gl::popMatrices();
    
}





