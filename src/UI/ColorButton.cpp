
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
    
    if(!isActive)return;
    
    gl::pushMatrices();
    gl::translate(mPosition);
    
    
    gl::color(isPressed ? Color(0,0,0) : mColor);
    gl::drawSolidCircle(vec2(0,0), mRadius);
    
    if(!isPressed){
        if(isSelected){
            float drawingRadius = mRadius + std::abs(std::sin(app::getElapsedSeconds()*2))*2;
           gl::drawSolidCircle(vec2(0,0), drawingRadius);
            gl::color(ci::Color(0,0.6,1.0));
            ci::gl::drawStrokedCircle(vec2(0,0),drawingRadius  ,4, 60);
        }else{
            gl::color(0,0,0);
            ci::gl::drawStrokedCircle(vec2(0,0), mRadius,3, 60);
        }
    }

    
    // DRAW children last
    for(BaseButton* button : mChildren){
        button->draw();
    }

    
    gl::color(1,1,1);
    
    gl::popMatrices();
    
}





