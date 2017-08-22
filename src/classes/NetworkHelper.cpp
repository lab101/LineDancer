//
//  NetworkHelper.cpp
//  LineDancer
//
//  Created by Kris Meeusen on 21/08/2017.
//
//

#include "NetworkHelper.hpp"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::osc;


bool NetworkHelper::setup(){
    
    try{
        setupOSCSender();
        mListener.setup(3000);
        
    }catch(...){
        return false;
    }
    
    return true;

}


void NetworkHelper::update(){
    
    if(ci::app::getElapsedSeconds() - lastBroadcast > 2){
        sendAlive();
        lastBroadcast = app::getElapsedSeconds();
    }
    
    
    while( mListener.hasWaitingMessages() ) {
        osc::Message message;
        mListener.getNextMessage( &message );
        std::string remoteIp = message.getArgAsString(0);
        
//        std::cout << message.getAddress() << std::endl;
        
        mAliveIps[remoteIp] = ci::app::getElapsedSeconds();
        
    }

}


std::string const NetworkHelper::getLastNummerIp(){

    return mLastIpNr;
    
}


std::string getLastIpNummer(std::string fullIp){
    
    std::vector<std::string> hostSplit = ci::split(fullIp, ".");
    return hostSplit.back();

}



void NetworkHelper::setupOSCSender(){
    
    
    std::vector<std::string> hostSplit = ci::split(System::getIpAddress(), ".");
    std::vector<std::string> subnetSplit = ci::split(System::getSubnetMask(), ".");
    
    mLastIpNr =  hostSplit.back(); // getLastNummerIp(System::getIpAddress());
    
    std::string broadcast = "";
    
    for(int i=0; i < subnetSplit.size();++i){
        broadcast += subnetSplit[i] == "0" ? "255" : hostSplit[i];
        if(i < subnetSplit.size()-1) broadcast += ".";
    }
    
    int port = 3000;
    
    mSender.setup( broadcast, port, true );
    
}



void NetworkHelper::sendAlive(){
    
    osc::Message message;
    message.setAddress("/alive");
    message.addStringArg(mLastIpNr);
    mSender.sendMessage(message);
}


void NetworkHelper::sendOscMessage(std::string command,vec3 point){
    
    osc::Message message;
    message.setAddress(command);
    message.addFloatArg(point.x);
    message.addFloatArg(point.y);
    message.addFloatArg(point.z);
    mSender.sendMessage(message);
    
}


