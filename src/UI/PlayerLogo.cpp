//
//  PlayerLogo.cpp
//  LiveDraw
//
//  Created by Kris Meeusen on 18/08/2017.
//
//

#include "PlayerLogo.hpp"
#include <string>     // std::string, std::stoi



void PlayerLogo::setup(std::string uniqueId){
    mColor = ci::Color(1.0,1.0,1.0);

    mUniqueId = uniqueId;
    
    
}


void PlayerLogo::update(){
    
}



void PlayerLogo::draw(bool fullShape, ci::vec2 position,ci::nvg::Context& vg){
    
    vg.beginPath();
    vg.circle(position,20);
    
    vg.textAlign(NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE );
    vg.fontSize(16);

    if(!fullShape){
        vg.strokeColor(mColor);
        vg.strokeWidth(4);
        vg.stroke();
        
    }else{
        vg.fillColor(mColor);
        vg.fill();
        vg.fillColor(ci::Color(0.0,0.0,0.0));

        //vg->strokeColor(ci::Color(1.0,0.0,0.0));

    }
    
    vg.text(position, mUniqueId);


}
