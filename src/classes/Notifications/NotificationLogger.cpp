//
//  NoficationLogger.cpp
//  VideoPlayer
//
//  Created by Kris Meeusen on 10/09/15.
//
//

#include "NotificationLogger.h"
#include "NotificationManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void NotificationLogger::write(const cinder::log::Metadata &meta, const std::string &text){
	if (meta.mLevel == cinder::log::LEVEL_ERROR){
		notify(text, Color(1, 0.0, 0.2));
	}
	else if (meta.mLevel == cinder::log::LEVEL_WARNING){
		notify(text, Color(1, 1, 0));
	}
    else if (meta.mLevel == cinder::log::LEVEL_INFO){
		notify(text, Color(0, 0.6, 1.));

	}
}
