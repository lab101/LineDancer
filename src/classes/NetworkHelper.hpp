//
//  NetworkHelper.hpp
//  LineDancer
//
//  Created by Kris Meeusen on 21/08/2017.
//
//

#pragma once

//osc
#include "OscSender.h"
#include "OscListener.h"
#include "cinder/System.h"



class NetworkHelper{
    
    
    // OSC
    ci::osc::Sender     mSender;
    ci::osc::Listener 	mListener;

    std::string mLastIpNr;
    
    void setupOSCSender();
    void sendOscMessage(std::string command,ci::vec3 point);

    
    float lastBroadcast;
    void sendAlive();
    
public:
    
    bool setup();
    void update();
    std::string const getLastNummerIp();

    
};