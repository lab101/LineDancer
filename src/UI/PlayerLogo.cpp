//
//  PlayerLogo.cpp
//  LiveDraw
//
//  Created by Kris Meeusen on 18/08/2017.
//
//

#include "GlobalSettings.h"

#include "PlayerLogo.hpp"
#include <string>


using namespace ci;

void PlayerLogo::setup(bool fullShape, std::string ipNr, int radius){
    mRadius = radius;
    mText = ipNr;
    isClient = fullShape;
    renderText();
    
}


void PlayerLogo::update(){
    
}


void PlayerLogo::alive(){
    lastAlive = ci::app::getElapsedSeconds();
}

void PlayerLogo::setText(std::string newText){
    mText = newText;
    renderText();
}

void PlayerLogo::setPosition(ci::vec2 position){
    mPosition = position;
    
    textBoundingScaled = Rectf(mPosition - vec2(mTexture->getWidth() * 0.5,mTexture->getHeight() * 0.5), mPosition + vec2(mTexture->getWidth() * 0.5,mTexture->getHeight() * 0.5));
    textBoundingScaled.scaleCentered(0.5);
}


void PlayerLogo::renderText(){
    TextBox tbox = TextBox().alignment( TextBox::CENTER ).font( GS()->mSmallFont ).size( ivec2( mRadius * 4, TextBox::GROW ) ).text( mText );
    tbox.setColor( Color::black() );
    mTexture = gl::Texture2d::create( tbox.render() );
}


void PlayerLogo::draw(){

    if(isClient){

        float div = powf((ci::app::getElapsedSeconds() - lastAlive) * 0.5,1.6);
        div = glm::clamp(1-div, 0.15f , 1.0f);
      
        ci::gl::color(0.,0.,0.,div);
        ci::gl::drawStrokedCircle(mPosition, mRadius  , 3 - div, 90);
        
    }else{
        ci::gl::color(0.,0.,0.);
        ci::gl::drawStrokedCircle(mPosition, mRadius,3, 90);
    }
    
    ci::gl::draw(mTexture, textBoundingScaled);

}
