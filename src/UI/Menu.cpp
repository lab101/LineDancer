//
//  Menu.cpp
//  LiveDraw
//
//  Created by lab101 on 05/09/16.
//
//

#include "Menu.hpp"


void Menu::setup(){

    DotButton* btnLayer = new DotButton(28, "NEW LAYER");
    buttons.push_back(btnLayer);

    DotButton* btnGif = new DotButton(28,"GIF");
    btnGif->setColor(ci::Color(0,0.6,1.0));
    buttons.push_back(btnGif);
    
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



void Menu::draw(std::shared_ptr<ci::nvg::Context> vg){
    
    float yPos = 60;
    for(auto button : buttons){
        button->setPosition(ci::vec2(ci::app::getWindowWidth() - 40, yPos));
        button->draw(vg);
        yPos += (button->mRadius * 2) + 20;
    }
    
 
    
    ci::vec2 elementPos(ci::app::getWindowWidth() - 40, yPos);
    ci::vec2 btnMargin(0,00);
    
    
    brushScaleBoxOrig.set(elementPos.x -10, elementPos.y + btnMargin.y, elementPos.x + 10, elementPos.y + 460);
    brushScaleBoxOrigCurrent= brushScaleBoxOrig;
    
    
    vg->beginPath();
    
    float width = brushScaleBoxOrigCurrent.getWidth();
    float height = brushScaleBoxOrigCurrent.getHeight();
    
    vg->rect(brushScaleBoxOrigCurrent.x2,brushScaleBoxOrigCurrent.y2, -width, -height * brushScale);
    
    ci::ColorA barColor(0,0.0,0.0);
    vg->fillColor(barColor);
    vg->strokeColor(barColor);
    vg->fill();
    
    vg->strokeWidth(isBrushHover ? 3 : 2);

    vg->beginPath();
    vg->rect(brushScaleBoxOrigCurrent);
    vg->stroke();


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


