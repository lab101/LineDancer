//
//  DotButton.cpp
//  LiveDraw
//
//  Created by lab101 on 14/09/16.
//
//

#include "DotButton.hpp"


void DotButton::setup(float radius,std::string text, std::shared_ptr<ci::nvg::Context> nanoVGContext){
    
    mText       = text;
    mRadius     = radius;
    vg          = nanoVGContext;
    
    isPressed = false;
    isHover = false;
    mColor = ci::Color(1,1,1);
    
    mText = text;

}



void DotButton::setPosition(ci::vec2 position){
    mPosition = position;
}

void DotButton::setColor(ci::Color color){
    mColor = color;
}



void DotButton::draw(){
    
    vg->beginPath();
    vg->strokeColor(mColor);
    vg->circle(mPosition,mRadius);
    vg->strokeWidth(4);
    
    
    if(isPressed){
        vg->strokeWidth(10);
        vg->fillColor(mColor);
        vg->fill();
    }
    
    if(isHover){
        vg->strokeWidth(10);

    }
    
    vg->stroke();
    

    if(mText != ""){
        vg->strokeColor(mColor);
        vg->fillColor(mColor);
        vg->textAlign(NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE );
        vg->fontSize(16);
        vg->text(mPosition, mText);
    }
    

}


bool DotButton::checkHover(ci::vec2 point){
    isHover =  glm::distance(point, mPosition) < (mRadius * 2);
    return isHover;
}

bool DotButton::checkTouchDown(ci::vec2 point){
    isPressed =  glm::distance(point, mPosition) < (mRadius * 2);
    return isPressed;
}


void DotButton::touchUp(){
    
    if(isPressed){
        onPressed.emit();
    }
    
    isPressed = false;
}

