//
//  PointsPackage.cpp
//  LineProjector2
//
//  Created by Lenn Vh on 12/03/2019.
//

#include "PointsPackage.hpp"


void PointsPackage::setup(std::vector<ci::vec3> _points, std::string _color){
    points = _points;
    color = _color;
    shape = "";
    isEraserOn = false;
};

void PointsPackage::setShape(std::string _shape){
    shape =_shape;
}

void PointsPackage::setEraser(bool _isEraserOn){
    isEraserOn =_isEraserOn;
}

