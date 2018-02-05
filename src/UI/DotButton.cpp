//
//  DotButton.cpp
//  LiveDraw
//
//  Created by lab101 on 14/09/16.
//
//

#include "DotButton.hpp"


DotButton::DotButton(float radius, std::string text){
    mText       = text;
    mRadius     = radius;
    
    isPressed = false;
    isHover = false;
    mColor = ci::Color(0,0,0);
    
    mText = text;
}



void DotButton::setPosition(ci::vec2 position){
    mPosition = position;
}

void DotButton::setColor(ci::Color color){
    mColor = color;
}



void DotButton::draw(std::shared_ptr<ci::nvg::Context> vg){
    
    //auto& vg = *mNanoVG;

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
        vg->textAlign(NVG_ALIGN_TOP | NVG_ALIGN_CENTER);
        
        vg->fontSize(14);

        // NVG_ALIGN_MIDDLE wasn't working, manually v-alliging it.
        ci::Rectf rect = vg->textBoxBounds(mPosition.x - mRadius, mPosition.y - mRadius, mRadius * 2, mText);
        vg->textBox(mPosition.x - mRadius, mPosition.y - (rect.getHeight() *0.5) , mRadius * 2, mText);
        
        
    }
    

}


bool DotButton::checkHover(ci::vec2 point){
    isHover =  glm::distance(point, mPosition) < mRadius;
    return isHover;
}

bool DotButton::checkTouchDown(ci::vec2 point){
    isPressed =  glm::distance(point, mPosition) < mRadius;
    return isPressed;
}


bool DotButton::touchUp(){
    
    if(isPressed){
        onPressed.emit();
        return true;
    }
    
    isPressed = false;
    return false;
}

