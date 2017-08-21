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
    
    
    std::shared_ptr<ci::nvg::Context> vg;
    
    std::string mUniqueId;
    ci::Color mColor;

    
public:
    
    void setup(std::string uniqueId);
    void update();
    void draw(bool fullShape, ci::vec2 position, ci::nvg::Context& vg);

    
};


#endif /* PlayerLogo_hpp */
