//
//  Menu.cpp
//  LiveDraw
//
//  Created by lab101 on 05/09/16.
//
//

#include "Menu.hpp"
#include "GlobalSettings.h"

void Menu::setup(){

    if(GS()->hasLayerButton.value()){
        DotButton* btnLayer = new DotButton(28, "+LAYER");
        buttons.push_back(btnLayer);
    }
    
    if(GS()->hasClearButton.value()){
        DotButton* btnClear = new DotButton(28, "CLEAR");
        buttons.push_back(btnClear);
    }

    if(GS()->hasGifOutput.value()){
        DotButton* btnGif = new DotButton(28,"GIF");
        btnGif->setColor(ci::Color(0,0.6,1.0));
        buttons.push_back(btnGif);
    }
    
    // setup commands
    for(auto button : buttons){
        button->onPressed.connect([=]{
            onNewCommand.emit(button->mText);
        });
    }
    
    brushScale = 0.5;
    isBrushHover = false;
}



void Menu::update(){
    
}

void Menu::setPosition(ci::vec2 position){
    
    mPosition = position;
}



void Menu::draw(){
    float yPos = 60;
    for(auto button : buttons){
        button->setPosition(ci::vec2(ci::app::getWindowWidth() - 40, yPos));
        button->draw();
        yPos += (button->mRadius * 2) + 20;
    }
    
    
    
    ci::vec2 elementPos(ci::app::getWindowWidth() - 40, yPos);
    ci::vec2 btnMargin(0,00);
    
    
    brushScaleBoxOrig.set(elementPos.x -10, elementPos.y + btnMargin.y, elementPos.x + 10, elementPos.y + 460);
    brushScaleBoxOrigCurrent = brushScaleBoxOrig;
    
    
    //    vg->beginPath();
    
    float width = brushScaleBoxOrigCurrent.getWidth();
    float height = brushScaleBoxOrigCurrent.getHeight();
    
    ci::gl::color(0,0,0);
    ci::gl::drawStrokedRect(brushScaleBoxOrigCurrent,3);

    ci::Rectf cop = brushScaleBoxOrig;
    cop.set(cop.x1, cop.y2 - ((cop.y2 - cop.y1) *  brushScale), cop.x2, cop.y2);
    ci::gl::drawSolidRect(cop);

}



void Menu::setBrushScale(float newScale){
    brushScale = newScale;
}


bool Menu::checkHover(ci::vec2 point){
    // check for pressure box
    
    for(auto button : buttons){
        button->checkHover(point);
    }
    
    
    isBrushHover = false;
    brushScaleBoxOrigCurrent = brushScaleBoxOrig;
    
    if(brushScaleBoxOrigCurrent.contains(point)){
        brushScale = ci::lmap<float>(point.y, brushScaleBoxOrigCurrent.y2, brushScaleBoxOrigCurrent.y1, 0.0f, 1.0f);
        brushScale = fmaxf(0, brushScale);
        brushScale = fminf(1, brushScale);
        
        onBrushSizeChanged.emit(brushScale);
        
        isBrushHover = true;
        brushScaleBoxOrigCurrent.scaleCentered(ci::vec2(1,1));
        return true;
    }
    
    return false;

}


bool Menu::checkTouchDown(ci::vec2 point){
    
    bool isPressed = false;
    
    for(auto button : buttons){
        isPressed += button->checkTouchDown(point);
    }
    
    return isPressed;
}


bool Menu::checkTouchUp(){
    bool isPressed = false;

    for(auto button : buttons){
       isPressed+= button->touchUp();
    }
    
    return isPressed;
}


