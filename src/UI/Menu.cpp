//
//  Menu.cpp
//  LiveDraw
//
//  Created by lab101 on 05/09/16.
//
//

#include "Menu.hpp"
#include "GlobalSettings.h"
#include "TextButton.hpp"
#include "ColorButton.hpp"

void Menu::setup(){

    if(GS()->hasLayerButton.value()){
        TextButton* btnLayer = new TextButton(28, "+LAYER", &(GS()->mSmallFont));
        btnLayer->setArgument("CLEAR");

        buttons.push_back(btnLayer);
    }
    
    if(GS()->hasClearButton.value()){
        TextButton* btnClear = new TextButton(28, "CLEAR",&(GS()->mSmallFont));
        btnClear->setArgument("CLEAR");

        buttons.push_back(btnClear);
    }

    if(GS()->hasGifOutput.value()){
        TextButton* btnGif = new TextButton(28,"GIF",&(GS()->mSmallFont));
        btnGif->setArgument("GIF");
        btnGif->setColor(ci::Color(0,0.6,1.0));
        buttons.push_back(btnGif);
    }
    
    
    
    // TEST PART for lennert
    TextButton* btnTest = new TextButton(28, "TEST",&(GS()->mSmallFont));
    buttons.push_back(btnTest);
    
    TextButton* test = new TextButton(28, "child lev1",&(GS()->mSmallFont));
    btnTest->addChildNode(test);
    test->setPosition(ci::vec2(-120,20));
    
    
//    TextButton* test2 = new TextButton(28, "child lev2",&(GS()->mSmallFont));
//    test->addChildNode(test2);
//    test2->setPosition(ci::vec2(-120,20));
//    
    ColorButton* colorTest = new ColorButton(28, ci::Color(1,0.5,0));
    
    test->addChildNode(colorTest);
    colorTest->setPosition(ci::vec2(-250,20));

    
    // END TEST
    
    
    
    
    // setup commands
    for(auto button : buttons){
        button->onPressed.connect([=]{
            onNewCommand.emit(button->getArgument());
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
    
    
    // drawing the round buttons.
    float yPos = 60;
    for(auto button : buttons){
        button->setPosition(ci::vec2(ci::app::getWindowWidth() - 40, yPos));
        button->draw();
        yPos += (button->mRadius * 2) + 20;
    }
    
    
    // drawing the brushselector
    ci::vec2 elementPos(ci::app::getWindowWidth() - 40, yPos);
    ci::vec2 btnMargin(0,00);
    
    brushScaleBoxOrig.set(elementPos.x -10, elementPos.y + btnMargin.y, elementPos.x + 10, elementPos.y + 460);
    brushScaleBoxOrigCurrent = brushScaleBoxOrig;
    
    
  
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


