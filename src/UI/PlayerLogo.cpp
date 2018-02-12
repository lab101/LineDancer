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

void PlayerLogo::setup(bool fullShape, std::string ipNr){
    mRadius = 20;
    mText = ipNr;
    
    renderText();
}


void PlayerLogo::update(){
    
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


void PlayerLogo::draw(float time){
    
  
    ci::gl::color(0,0,0);


//    if(isClient){
//        float div = powf(ci::app::getElapsedSeconds() - time,1.6);
//
//        if(div > mRadius) div =mRadius;
//
//        ci::gl::drawStrokedCircle(position, mRadius -div);
////
////        vg.beginPath();
////        vg.circle(position, 20 - div);
////
////        vg.fillColor(ci::Color(0.0,0.4,1.0));
////        vg.fill();
////        vg.fillColor(ci::Color(1.0,1.0,1.0));
//
//
//    }else{
        ci::gl::drawStrokedCircle(mPosition, mRadius,3, 90);
//
////        vg.beginPath();
////
////        vg.circle(position, 24 );
////
////
////        vg.strokeColor(ci::Color(0.0,0.0,0.0));
////        vg.strokeWidth(4);
////        vg.stroke();
////        vg.fillColor(ci::Color(0.0,0.0,0.0));
//
//        //vg->strokeColor(ci::Color(1.0,0.0,0.0));
//
//    }

    
    ci::gl::draw(mTexture, textBoundingScaled);

//    vg.textAlign(NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE );
//    vg.fontSize(16);
//
//    vg.text(position, ipNr);


}
