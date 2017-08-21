//
//  Line.hpp
//  LiveDraw
//
//  Created by lab101 on 31/08/16.
//
//

#ifndef Line_hpp
#define Line_hpp

#include <stdio.h>

namespace  lab101 {
    class Line{
    public:
        
        std::vector<ci::vec3> points;
        float opacity;
        
        void addPoint(float x,float y,float pressure){
            points.push_back(ci::vec3(x,y,pressure));
        }
        
    };
}


#endif /* Line_hpp */
