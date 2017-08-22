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
#include "ci_nanovg_gl.hpp"


class PlayerLogo{
    
    
    
public:
    
    void setup();
    void update();
    void draw(bool fullShape, ci::vec2 position, std::string ipNr,float time, ci::nvg::Context& vg);

    
};


#endif /* PlayerLogo_hpp */
