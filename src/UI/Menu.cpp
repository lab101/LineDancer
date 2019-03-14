//
//  Menu.cpp
//  LiveDraw
//
//  Created by lab101 on 05/09/16.
//
//

#include "Menu.hpp"
#include "Lab101Utils.h"
#include "GlobalSettings.h"
#include "SettingManager.h"
#include "TextButton.hpp"
#include "ColorButton.hpp"


void Menu::setup(){

    if(GS()->hasLayerButton.value()){
        TextButton* btnLayer = new TextButton(28, "+LAYER", &(GS()->mSmallFont));
        btnLayer->setArgument("LAYER");
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
    
    

//add color btns
    TextButton* btnColor = new TextButton(28, "COLOR",&(GS()->mSmallFont));
    buttons.push_back(btnColor);
   
    
//-- color children
    std::vector<std::string> color = GS()->readColors();
    std::reverse(color.begin(),color.end());
    
    for(int i = 0; i< color.size();i++){
        ColorButton* btnTemp = new ColorButton(28, hexStringToColor(color[i]));
        btnColor->addChildNode(btnTemp);
        btnTemp->setPosition(ci::vec2(((-2*btnTemp->mRadius)-10)*(i+1),0));
        btnTemp->setArgument(color[i]);
        
        if(i == color.size()-1){
            GS()->brushColor = hexStringToColor(color[i]);
            GS()->brushColorHex = color[i];
            btnTemp->selectBtn();
        }
       
    }
   
    

//add shape buttons
    TextButton* btnShape = new TextButton(28, "SHAPES",&(GS()->mSmallFont));
    buttons.push_back(btnShape);
    
//--shape children
    BaseButton* btnCircle = new BaseButton();
    btnShape->addChildNode(btnCircle);
    btnCircle->setPosition(ci::vec2((-2*28)-10,0));
    ci::gl::TextureRef texCircle = ci::gl::Texture::create(ci::loadImage(ci::app::loadResource("Circle.png")));
    btnCircle->setTexture(texCircle);
    btnCircle->setArgument("CIRCLE");
    
    BaseButton* btnLine = new BaseButton();
    btnShape->addChildNode(btnLine);
    btnLine->setPosition(ci::vec2(((-2*28)-10)*2,0));
    ci::gl::TextureRef texLine = ci::gl::Texture::create(ci::loadImage(ci::app::loadResource("Line.png")));
    btnLine->setTexture(texLine);
    btnLine->setArgument("LINE");
    
    BaseButton* btnRect = new BaseButton();
    btnShape->addChildNode(btnRect);
    btnRect->setPosition(ci::vec2(((-2*28)-10)*3,0));
    ci::gl::TextureRef texRect = ci::gl::Texture::create(ci::loadImage(ci::app::loadResource("Rect.png")));
    btnRect->setTexture(texRect);
    btnRect->setArgument("RECT");
    
    BaseButton* btnBrush = new BaseButton();
    btnShape->addChildNode(btnBrush);
    btnBrush->setPosition(ci::vec2(((-2*28)-10)*4,0));
    ci::gl::TextureRef textBrush = ci::gl::Texture::create(ci::loadImage(ci::app::loadResource("Brush.png")));
    btnBrush->setTexture(textBrush);
    btnBrush->selectBtn();
    btnBrush->setArgument("BRUSH");
    
//    BaseButton* btnPoly = new BaseButton();
//    btnShape->addChildNode(btnPoly);
//    btnPoly->setPosition(ci::vec2(((-2*28)-10)*4,0));
//    ci::gl::TextureRef texPoly = ci::gl::Texture::create(ci::loadImage(ci::app::loadResource("Polygon.png")));
//    btnPoly->setTexture(texPoly);
    
    
    // setup commands
    for(auto button : buttons){
        ConnectEvents(button);
        
    }
    for(auto btnColorChild : btnColor->getChildren()){
        btnColorChild->onPressed.connect([=]{
            for(auto btnColorChild : btnColor->getChildren()){
                btnColorChild->unSelectBtn();
            }
            btnColorChild->selectBtn();
            btnColor->toggleChildrenOnOff();
        });
    }
    for(auto btnShapeChild : btnShape->getChildren()){
        btnShapeChild->onPressed.connect([=]{
            for(auto btnShapeChild : btnShape->getChildren()){
                btnShapeChild->unSelectBtn();
            }
            btnShapeChild->selectBtn();
            btnShape->toggleChildrenOnOff();
        });
    }
    btnColor->onPressed.connect([=]{
        btnColor->toggleChildrenOnOff();
    });
    btnShape->onPressed.connect([=]{
        btnShape->toggleChildrenOnOff();
    });
    brushScale = 0.5;
    isBrushHover = false;
}


void Menu::ConnectEvents(BaseButton* button){
    std::string btnArg = button->getArgument();
    
    if(btnArg.length()> 0){
        if(std::string(1,btnArg[0]) == "#"){
            button->onPressed.connect([=]{
                GS()->brushColor = hexStringToColor(btnArg);
                 GS()->brushColorHex = btnArg;
            });
        }else{
            button->onPressed.connect([=]{
                onNewCommand.emit(button->getArgument());
            });
        }
        
    }
    for(auto childButton :  button->getChildren()){
        ConnectEvents(childButton);
    }
    
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



 


