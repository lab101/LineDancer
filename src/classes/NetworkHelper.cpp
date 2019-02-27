//
//  NetworkHelper.cpp
//  LineDancer
//
//  Created by Kris Meeusen on 21/08/2017.
//
//

#include "NetworkHelper.hpp"
#include "cinder/Utilities.h"
#include "GlobalSettings.h"

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

void NetworkHelper::setNextGroup(){
    if(++groupId > GS()->maxGroups.value()-1) groupId = 0;
}


void NetworkHelper::update(){
    
    if(ci::app::getElapsedSeconds() - lastBroadcast > 4){
        sendAlive();
        lastBroadcast = app::getElapsedSeconds();
    }
    
    
    while( mListener.hasWaitingMessages() ) {
        
        osc::Message message;
        mListener.getNextMessage( &message );
        
        // return from our own broadcast
        std::string remoteIp = message.getRemoteIp();
        if( remoteIp ==  mOwnIpAdress)
            continue;
        
        std::string remoteLastNr = extractLastIpNr(remoteIp);
        std::string const adress = message.getAddress();
        
        
        
        int incomingGroupId =  message.getArgAsInt32(0);
        
        // discard packages from other groups
        if(incomingGroupId == groupId){

            
            if(mAliveIps.find(remoteLastNr) == mAliveIps.end()){
                onNewConnection.emit(remoteLastNr);
            }
            
            mAliveIps[remoteLastNr] = ci::app::getElapsedSeconds();
            onAlivePing.emit(remoteLastNr);


            if(adress == "points"){
                int totals = message.getNumArgs() ;
                bool isEraserOn = message.getArgAsInt32(1);

                
                std::vector<ci::vec3> points;
                for(int i=2;i < totals;i+=3){
                    points.push_back(ci::vec3(message.getArgAsFloat(i),message.getArgAsFloat(i+1),message.getArgAsFloat(i+2)));
                }
                
                onReceivePoints.emit(points,isEraserOn);
            }else if(adress == "shape"){
                std::vector<ci::vec3> points;
                for(int i=2;i < 6;i+=3){
                    points.push_back(ci::vec3(message.getArgAsFloat(i),message.getArgAsFloat(i+1),message.getArgAsFloat(i+2)));
                }
                std::string shape = message.getArgAsString(7);
               // onReceiveShapes.emit(point1,point2,shape);
            }
        }
        
        
    }

}


std::string const NetworkHelper::getLastMyIpNr(){
    return mLastIpNr;
}

int const NetworkHelper::getGroupId(){
    // add one makes it nicer for non programmers
    return groupId +1;
}


std::string NetworkHelper::extractLastIpNr(std::string& fullIp){

    std::vector<std::string> hostSplit = ci::split(fullIp, ".");
    return hostSplit.back();

}



void NetworkHelper::setupOSCSender(){
    
    mOwnIpAdress = System::getIpAddress();
    std::vector<std::string> hostSplit = ci::split(mOwnIpAdress, ".");
    std::vector<std::string> subnetSplit = ci::split(System::getSubnetMask(), ".");
    
    mLastIpNr =  hostSplit.back(); // getLastNummerIp(System::getIpAddress());
    
    std::string broadcast = "";
    
    for(int i=0; i < subnetSplit.size();++i){
        broadcast += subnetSplit[i] == "0" ? "255" : hostSplit[i];
        if(i < subnetSplit.size()-1) broadcast += ".";
    }
    
    int port = 3000;
    
    mSender.setup( broadcast, port, true );
    lastBroadcast = app::getElapsedSeconds();
    
}



void NetworkHelper::sendAlive(){
    
    osc::Message message;
    message.setAddress("alive");
    message.addIntArg(groupId);
    mSender.sendMessage(message);
}



void NetworkHelper::sendPoints(std::vector<ci::vec3>& points, bool isEraserOn){
    osc::Message message;
    message.setAddress("points");
    message.addIntArg(groupId);
    message.addIntArg(isEraserOn);

    for(vec3& p : points){
        message.addFloatArg(p.x);
        message.addFloatArg(p.y);
        message.addFloatArg(p.z);
    }
    
    mSender.sendMessage(message);
    lastBroadcast = app::getElapsedSeconds();


}

void NetworkHelper::sendTwoPointShape(vec3& point1,vec3& point2, std::string shape){
    osc::Message message;
    message.setAddress("shape");
    message.addIntArg(groupId);
    
    message.addStringArg(shape);
    
    message.addFloatArg(point1.x);
    message.addFloatArg(point1.y);
    message.addFloatArg(point1.z);
    message.addFloatArg(point2.x);
    message.addFloatArg(point2.y);
    message.addFloatArg(point2.z);
    
    mSender.sendMessage(message);
    lastBroadcast = app::getElapsedSeconds();
}



