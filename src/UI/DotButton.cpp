//
//  DotButton.cpp
//  LiveDraw
//
//  Created by lab101 on 14/09/16.
//
//

#include "DotButton.hpp"
#include "GlobalSettings.h"

using namespace ci;

DotButton::DotButton(float radius, std::string text){
    mText       = text;
    mRadius     = radius;
    
    isPressed = false;
    isHover = false;
    mColor = ci::Color(0,0,0);
    
    mText = text;
    
    
    renderText();

}


void DotButton::renderText(){
    TextBox tbox = TextBox().alignment( TextBox::CENTER ).font( GS()->mSmallFont ).size( ivec2( mRadius * 4, TextBox::GROW ) ).text( mText );
    tbox.setColor( mColor );
    mTexture = gl::Texture2d::create( tbox.render() );
}


void DotButton::setPosition(ci::vec2 position){
    mPosition = position;
    
    textBoundingScaled = Rectf(mPosition - vec2(mTexture->getWidth() * 0.5,mTexture->getHeight() * 0.5), mPosition + vec2(mTexture->getWidth() * 0.5,mTexture->getHeight() * 0.5));
    textBoundingScaled.scaleCentered(0.5);
}

void DotButton::setColor(ci::Color color){
    mColor = color;
    renderText();
}

void DotButton::draw(){
    
    ci::gl::color(mColor);
    if(isPressed){
        ci::gl::drawSolidCircle(mPosition, mRadius);
    }
    
    ci::gl::color(mColor);

    ci::gl::drawStrokedCircle(mPosition, mRadius,3, 90);

    ci::gl::color(1,1,1);
    ci::gl::draw(mTexture,textBoundingScaled);
    
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
        isPressed = false;

        return true;
    }
    
    isPressed = false;
    return false;
}

