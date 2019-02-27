//
//  BaseButton.hpp
//  LineDancer
//
//  Created by Kris Meeusen on 18/02/2019.
//

#pragma once

#include <stdio.h>
#include "cinder/gl/gl.h"
#include "cinder/Signals.h"

class BaseButton{
    
    std::string mArgument;
    ci::gl::TextureRef mTexture;

    
    bool isHover;
    ci::Rectf mBoundingScaled;
    bool isChildrenVisible;
    
    void calculateBoundingBox();
    
   
    
protected:
    ci::Color mColor;
    std::vector<BaseButton*> mChildren;
    bool isPressed;
    ci::vec2 mPosition;


    
public:
    
    float mRadius;


    BaseButton();
    ~BaseButton(){};

    virtual void draw();
    void setTexture(ci::gl::TextureRef texture);
    
    // incoming events
    bool checkTouchDown(ci::vec2 point);
    bool checkHover(ci::vec2 point);
    bool touchUp();
    
    
    // outgoing events
    ci::signals::Signal<void()>       onPressed;
    
    void setArgument(std::string argument);
    std::string getArgument();
    std::vector<BaseButton*> getChildren();
    
    void addChildNode(BaseButton* childNode);
    void hideChildren();
    void showChildren();
    void toggleChildrenOnOff();
    
    void setPosition(ci::vec2 position);
    void setColor(ci::Color color);


};


