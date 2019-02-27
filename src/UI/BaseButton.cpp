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
    
    isChildrenVisible = false;
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
    isChildrenVisible = false;
}
void BaseButton::showChildren(){
   isChildrenVisible = true;
}

void BaseButton::toggleChildrenOnOff(){
    isChildrenVisible = !isChildrenVisible;
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
    if(isChildrenVisible){
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
    
    
    for(auto button : mChildren){
         bool isHit = button->checkTouchDown(point - mPosition);
        if(isHit){
             isPressed = true;
            return true;
        }
    }
     isPressed = glm::distance(point, mPosition) < mRadius;
    return isPressed;
}


bool BaseButton::touchUp(){
    for(auto button : mChildren){
        bool isHit = button->touchUp();
        if(isHit){
            isPressed = false;
            return true;
        }
    }
    if(isPressed){
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




