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
    
    
    
  //add color btns
    TextButton* btnColor = new TextButton(28, "COLOR",&(GS()->mSmallFont));
    buttons.push_back(btnColor);
    
    ColorButton* btnR = new ColorButton(28, hexStringToColor("#ff0000"));
    btnColor->addChildNode(btnR);
    btnR->setPosition(ci::vec2((-2*btnR->mRadius)-10,0));
    btnR->setArgument("#ff0000");
    
    ColorButton* btnG = new ColorButton(28, hexStringToColor("#00ff00"));
    btnColor->addChildNode(btnG);
    btnG->setPosition(ci::vec2(((-2*btnG->mRadius)-10)*2,0));
     btnG->setArgument("#00ff00");
    
    ColorButton* btnB = new ColorButton(28, hexStringToColor("#0000ff"));
    btnColor->addChildNode(btnB);
    btnB->setPosition(ci::vec2(((-2*btnB->mRadius)-10)*3,0));
    btnB->setArgument("#0000ff");
    
    //add shape buttons
    TextButton* btnShape = new TextButton(28, "SHAPES",&(GS()->mSmallFont));
    buttons.push_back(btnShape);
    
    
    BaseButton* btnCircle = new BaseButton();
    btnShape->addChildNode(btnCircle);
    btnCircle->setPosition(ci::vec2((-2*28)-10,0));
    ci::gl::TextureRef texCircle = ci::gl::Texture::create(ci::loadImage(ci::app::loadResource("Circle.png")));
    btnCircle->setTexture(texCircle);
    
    BaseButton* btnLine = new BaseButton();
    btnShape->addChildNode(btnLine);
    btnLine->setPosition(ci::vec2(((-2*28)-10)*2,0));
    ci::gl::TextureRef texLine = ci::gl::Texture::create(ci::loadImage(ci::app::loadResource("Line.png")));
    btnLine->setTexture(texLine);
    
    BaseButton* btnRect = new BaseButton();
    btnShape->addChildNode(btnRect);
    btnRect->setPosition(ci::vec2(((-2*28)-10)*3,0));
    ci::gl::TextureRef texRect = ci::gl::Texture::create(ci::loadImage(ci::app::loadResource("Rect.png")));
    btnRect->setTexture(texRect);
    
    // setup commands
    for(auto button : buttons){
        ConnectEvents(button);
    }
    //std::cout << hexStringToColor("#ffffff") << std::endl;
    hexStringToColor("#2a5df7");
    
    
    
    brushScale = 0.5;
    isBrushHover = false;
}


void Menu::ConnectEvents(BaseButton* button){
    std::string btnArg = button->getArgument();
    
    if(btnArg.length()> 0){
        if(std::string(1,btnArg[0]) == "#"){
            button->onPressed.connect([=]{
                GS()->brushColor = hexStringToColor(btnArg);
            });
        }
    }
    
    if(btnArg.length()> 0){
    button->onPressed.connect([=]{
        onNewCommand.emit(button->getArgument());
    });
    }
    
    for(auto childButton :  button->getChildren()){
        ConnectEvents(childButton);
    }
    
   
    
    
    //check all children
    //check if arg > 0
    //check argument of button if 1 char is #
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



 int Menu::HexadecimalToDecimal(std::string hex) { //https://www.programmingalgorithms.com/algorithm/hexadecimal-to-rgb?lang=C%2B%2B
    int hexLength = hex.length();
    double dec = 0;
     std::string hexChars[6] =  {"a","b","c","d","e","f"};
     
     
     
     
    for (int i = 0; i < hexLength; ++i)
    {
        
       char b = hex[i];
        for(int j = 0;j<6;j++){
            if(std::string(1,hex[i]) == hexChars[j]){
                b = j+10;
            }
        };
        
       

        if (b >= 48 && b <= 57)
            b -= 48;
        else if (b >= 65 && b <= 70)
            b -= 55;

        dec += b * std::pow(16, ((hexLength - i) - 1));
        
    }
    
    return (int)dec;
}

ci::Color Menu::hexStringToColor(std::string hex){
    if (hex[0] == '#')
        hex = hex.erase(0, 1);
    
    unsigned char r = (unsigned char)HexadecimalToDecimal(hex.substr(0, 2));
    unsigned char g = (unsigned char)HexadecimalToDecimal(hex.substr(2, 2));
    unsigned char b = (unsigned char)HexadecimalToDecimal(hex.substr(4, 2));
    
    return ci::Color(r, g, b);
}


