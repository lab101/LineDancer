//
//  BaseButton.cpp
//  LineDancer
//
//  Created by Kris Meeusen on 18/02/2019.
//

#include "BaseButton.hpp"


using namespace ci;

BaseButton::BaseButton(){
    
    mRadius  = 28;

    isPressed = false;
    isHover = false;
    mColor = ci::Color(0,0,0);
    mArgument = "";
    
    hideShowChildren = false;
}


void BaseButton::setPosition(ci::vec2 position){
    mPosition = position;
    
    // calculate boundingbox for retina.
    // todo add windowscale.
    if(mTexture){
        calculateBoundingBox();
    }
}

void BaseButton::calculateBoundingBox(){
    
    float displayScale = 1 /  ci::app::getWindowContentScale();
    
    vec2 size = mTexture->getSize();
    mBoundingScaled = Rectf(vec2(size.x * -displayScale, size.y * -displayScale), vec2(size.x * displayScale, size.y * displayScale) );
    mBoundingScaled.scaleCentered(displayScale);

}


void BaseButton::setColor(ci::Color color){
    mColor = color;
}


void BaseButton::addChildNode(BaseButton* childNode){
    mChildren.push_back(childNode);
}

void BaseButton::hideChildren(){
    hideShowChildren = false;
}
void BaseButton::showChildren(){
   hideShowChildren = true;
}


void BaseButton::draw(){
    
    gl::pushMatrices();
    gl::translate(mPosition);
    
    
   
    
    ci::gl::color(mColor);
    if(isPressed){
        ci::gl::drawSolidCircle(vec2(0,0), mRadius);
    }
    
    gl::color(mColor);
    
    ci::gl::drawStrokedCircle(vec2(0,0), mRadius,3, 60);
    
    
    gl::color(1,1,1);
    gl::draw(mTexture,mBoundingScaled);

    
    // DRAW children last
    if(hideShowChildren){
    for(BaseButton* button : mChildren){
        button->draw();
    }
    }
    
    gl::popMatrices();
    
 }



bool BaseButton::checkHover(ci::vec2 point){
    isHover =  glm::distance(point, mPosition) < mRadius;
    return isHover;
}

bool BaseButton::checkTouchDown(ci::vec2 point){
    
    int pressCount = 0;
    
    for(auto button : mChildren){
        isPressed+= button->checkTouchDown(point - mPosition);
    }
    

    if(pressCount > 0) return true;
    
    pressCount +=  glm::distance(point, mPosition) < mRadius;
    isPressed = pressCount > 0;
    return isPressed;
}


bool BaseButton::touchUp(){
    
    
    for(auto button : mChildren){
        button->touchUp();
    }
    
    if(isPressed){
        if(hideShowChildren){
            hideChildren();
        }else{
            showChildren();
        }
        
        onPressed.emit();
        isPressed = false;
        
        return true;
    }
    
    isPressed = false;
    return false;
}


std::string BaseButton::getArgument(){
    return mArgument;
}

std::vector<BaseButton*> BaseButton::getChildren(){
    return mChildren;
}

void BaseButton::setArgument(std::string argument){
    mArgument = argument;
}



void BaseButton::setTexture(ci::gl::TextureRef texture){
    mTexture = texture;
    calculateBoundingBox();
}




