//
//  GlobalSettings.cpp
//  InnovationWall
//
//  Created by Kris Meeusen on 01/02/16.
//
//

#include <stdio.h>
#include "GlobalSettings.h"

#include "cinder/Utilities.h"

using namespace std;
using namespace ci;



GlobalSettings::GlobalSettings(){

    // in some cases the software can be used
    // to only send data not store, disabling output makes it faster.
    hasGifOutput = false;
    hasClearButton = true;
    hasLayerButton = false;
    
    zoomLevel = 1.0;
    compositionSize = ci::ivec2(1200,800);
    blue = ci::Color(0,0.6,1.0);


}








