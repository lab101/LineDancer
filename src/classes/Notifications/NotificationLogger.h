//
//  NoficationLogger.h
//  VideoPlayer
//
//  Created by Kris Meeusen on 10/09/15.
//
//

#ifndef __VideoPlayer__NoficationLogger__
#define __VideoPlayer__NoficationLogger__

#include <stdio.h>

#include "cinder/Cinder.h"
#include "cinder/Filesystem.h"
#include "cinder/CurrentFunction.h"
#include "cinder/CinderAssert.h"
#include "cinder/System.h"
#include "cinder/Log.h"


class NotificationLogger : public cinder::log::Logger{
public:
	void write(const cinder::log::Metadata &meta, const std::string &text) override;

};



//typedef cinder::log::ThreadSafeT<NotificationLogger>		NotificationLoggerThreadSafe;

#endif /* defined(__VideoPlayer__NoficationLogger__) */
