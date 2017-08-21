//
//  Menu.cpp
//  LiveDraw
//
//  Created by lab101 on 05/09/16.
//
//

#include "Menu.hpp"


void Menu::setup(std::shared_ptr<ci::nvg::Context> nanoVGContext){
    
    vg = nanoVGContext;
    
    btnSave.setup(26,"NEW",nanoVGContext);
    btnUndo.setup(26,"UNDO",nanoVGContext);
    btnGif.setup(26,"GIF", nanoVGContext);
    
    btnUndo.setColor(ci::Color(1,0.3,0.6));
    btnGif.setColor(ci::Color(1,1,0.6));
    
    brushScale = 0.5;
    isBrushHover = false;
}



void Menu::update(){
    
}

void Menu::setPosition(ci::vec2 position){
    
    mPosition = position;
    
    ci::vec2 elementPos = position;
    ci::vec2 btnMargin(0,80);
    
    btnSave.setPosition(elementPos);
    btnUndo.setPosition(elementPos += btnMargin);
    btnGif.setPosition(elementPos += btnMargin);
    
    brushScaleBoxOrig.set(elementPos.x -10, elementPos.y + btnMargin.y, elementPos.x + 10, elementPos.y + 460);
    brushScaleBoxOrigCurrent= brushScaleBoxOrig;
}



void Menu::draw(){
    
    btnUndo.draw();
    btnSave.draw();
    btnGif.draw();
    // bar
    vg->beginPath();
    
    float width = brushScaleBoxOrigCurrent.getWidth();
    float height = brushScaleBoxOrigCurrent.getHeight();
    
    vg->rect(brushScaleBoxOrigCurrent.x2,brushScaleBoxOrigCurrent.y2, -width, -height * brushScale);
    vg->fillColor(ci::Color(1,1,1));
    vg->strokeColor(ci::Color(1,1,1));
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
    
    btnSave.checkHover(point);
    btnUndo.checkHover(point);
    btnGif.checkHover(point);
    
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
    

    isPressed = btnSave.checkTouchDown(point);
    isPressed += btnUndo.checkTouchDown(point);
    isPressed += btnGif.checkTouchDown(point);
    
   
    return isPressed;
}


bool Menu::touchUp(){

    btnSave.touchUp();
    btnUndo.touchUp();
    btnGif.touchUp();
    
    bool wasPressed = isPressed;
    isPressed = false;
    
    return wasPressed;
}


