//
//  PlayerLogo.hpp
//  LiveDraw
//
//  Created by Kris Meeusen on 18/08/2017.
//
//

#ifndef PlayerLogo_hpp
#define PlayerLogo_hpp

#include <stdio.h>


class PlayerLogo{
    
    
    ci::gl::TextureRef mTexture;
    void renderText();
    std::string mText;
    
    int mBaseRadius;
    
    float currentRadius;
    float targetRadius;
    
    float lastAlive;
    ci::Rectf textBoundingScaled;
    ci::vec2 mPosition;
    
    
    bool isClient;

public:
    
    void setPosition(ci::vec2 position);
    void setText(std::string text);
    void setup(bool fullShape, std::string ipNr, int radius);
    void update();
    void draw();
    void alive(float lastAlive);

    
};


#endif /* PlayerLogo_hpp */
