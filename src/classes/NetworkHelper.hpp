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
#include "cinder/Signals.h"

#include <map>



class NetworkHelper{
    
    
    // OSC
    ci::osc::Sender     mSender;
    ci::osc::Listener 	mListener;

    std::string mOwnIpAdress;
    std::string mLastIpNr;
    

    
    void setupOSCSender();

    
    std::string extractLastIpNr(std::string& fullIp);
    
    float lastBroadcast;
    void sendAlive();

public:
    
    ci::signals::Signal<void(std::vector<ci::vec3>&)>   onReceivePoints;

    
    std::map<std::string,float> mAliveIps;

    bool setup();
    void update();
    std::string const getLastMyIpNr();
    
    void sendOscMessage(std::string command,ci::vec3 point);
    void sendPoints(std::vector<ci::vec3>& points,bool isEraserOn);


    
};
