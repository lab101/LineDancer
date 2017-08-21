// Copyright (c) 2014 Matt Ebb
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once
#include "tabletData.h"


class ofxTablet {
public:
   // static ofEvent<TabletData> tabletEvent;
    static TabletData tabletData;
    static ci::signals::Signal<void (TabletData& )> onData;

    static void start();
	static void stop();
    static void handle();
};

