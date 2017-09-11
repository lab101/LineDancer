//
//  PlayerLogo.cpp
//  LiveDraw
//
//  Created by Kris Meeusen on 18/08/2017.
//
//

#include "PlayerLogo.hpp"
#include <string>     // std::string, std::stoi



void PlayerLogo::setup(){
}


void PlayerLogo::update(){
    
}



void PlayerLogo::draw(bool isClient, ci::vec2 position, std::string ipNr,float time ,ci::nvg::Context& vg){
    
    
    

    if(isClient){
        float div = powf(ci::app::getElapsedSeconds() - time,1.2);

        if(div > 20) div =20;
        
        vg.beginPath();
        vg.circle(position, 20 - div);

        vg.fillColor(ci::Color(1.0,1.0,1.0));
        vg.fill();
        vg.fillColor(ci::Color(0.0,0.0,0.0));
        

    }else{
        vg.beginPath();

        vg.circle(position, 20 );
        
        
        vg.strokeColor(ci::Color(1.0,1.0,1.0));
        vg.strokeWidth(4);
        vg.stroke();

        //vg->strokeColor(ci::Color(1.0,0.0,0.0));

    }

    

    
    vg.textAlign(NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE );
    vg.fontSize(16);

    vg.text(position, ipNr);


}
