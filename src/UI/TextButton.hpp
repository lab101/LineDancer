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
#include "cinder/Font.h"

class TextButton : public BaseButton{

  //  ci::Rectf textBoundingScaled;
    
    ci::Font* mTextFont;
    void renderText();
    
public:
    
    std::string mText;
    TextButton(float radius, std::string text , ci::Font* font);

};
