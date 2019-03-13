//
//  PointsPack.hpp
//  LiveDraw
//
//  Created by lab101 on 06/09/16.
//
//

#pragma once

#include <stdio.h>


class PointsPackage{
    
    
    
public:
    std::vector<ci::vec3> points;
    std::string color;
    std::string shape;
    bool isEraserOn;
    
    void setup(std::vector<ci::vec3> points, std::string color);
    void setShape(std::string shape);
    void setEraser(bool isEraserOn);
};




