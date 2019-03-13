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
#include "../classes/Lab101Utils.h"


using namespace ci;

void PlayerLogo::setup(bool fullShape, std::string ipNr, int radius){
    mBaseRadius = radius;
    mText = ipNr;
    isClient = fullShape;
    renderText();
    
}


void PlayerLogo::update(){
    
}


void PlayerLogo::alive(float lastAliveUpdate){
    lastAlive = lastAliveUpdate;
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
    TextBox tbox = TextBox().alignment( TextBox::CENTER ).font( GS()->mSmallFont ).size( ivec2( mBaseRadius * 4, TextBox::GROW ) ).text( mText );
    tbox.setColor( Color::black() );
    mTexture = gl::Texture2d::create( tbox.render() );
}


void PlayerLogo::draw(){

    if(isClient){

        
        
        float div = powf((ci::app::getElapsedSeconds() - lastAlive) * 0.6, 1.2);
        float colorMap = ofMap(div, 0, 6, 1.0, 0.3, true);

        targetRadius = mBaseRadius +  ofMap(div, 0, 4, 8, 0, true);

        currentRadius +=  (targetRadius -  currentRadius) * 0.08;
        
        ci::gl::color(0.,0.,0.,colorMap);
        ci::gl::drawStrokedCircle(mPosition, currentRadius  , 3, 30);

    }else{
        ci::gl::color(0.,0.,0.);
        ci::gl::drawStrokedCircle(mPosition, mBaseRadius,3, 30);
    }
    
    ci::gl::draw(mTexture, textBoundingScaled);

}
