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
    int mRadius;
    
    ci::Rectf textBoundingScaled;
    ci::vec2 mPosition;

public:
    
    void setPosition(ci::vec2 position);

    void setup(bool fullShape, std::string ipNr);
    void update();
    void draw(float time);

    
};


#endif /* PlayerLogo_hpp */
