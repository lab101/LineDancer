//
//  DotButton.hpp
//  LiveDraw
//
//  Created by lab101 on 14/09/16.
//
//

#pragma once

#include <stdio.h>
#include "BaseButton.hpp"

class ColorButton : public BaseButton{

  //  ci::Rectf textBoundingScaled;
    ci::Color mColor;
    
public:
    
    ColorButton(float radius, ci::Color color);
    void draw();
};
