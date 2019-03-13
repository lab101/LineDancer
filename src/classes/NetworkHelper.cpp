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
using namespace ci::app;
using namespace std;
using namespace asio;
using namespace asio::ip;
using namespace ci::osc;

NetworkHelper::NetworkHelper()
: mIoService(new asio::io_service), mWork(new asio::io_service::work(*mIoService)),
mSocket( new udp::socket( App::get()->io_service(), udp::endpoint( udp::v4(), 3000 ) ) ),
mReceiver(3000, asio::ip::udp::v4(), *mIoService),
mSender( mSocket, udp::endpoint( address_v4::broadcast(), 3000 ) )
{
    mSocket->set_option( asio::socket_base::broadcast(true) );

}





bool NetworkHelper::setup(){
    
    setupOSCSender();
    
    mReceiver.setListener("/points",
                          [&](const osc::Message &msg){
                              //std::lock_guard<std::mutex> lock(mCirclePosMutex);
                              //mCurrentCirclePos.x = msg[0].int32();
                              //mCurrentCirclePos.y = msg[1].int32();
                              
                              
                              int totals = msg.getNumArgs();
                              int incomingGroupId = msg[0].int32();
                              if (incomingGroupId != groupId) return;
                              
                              // bool isEraserOn = msg.getArgInt32(1);
                              std::string color = msg.getArgString(2);
                              std::vector<ci::vec3> points;
                              for (int i = 3; i < totals; i += 3){
                                  points.push_back(ci::vec3(msg[i].flt(), msg[i + 1].flt(), msg[i + 2].flt()));
                              }
                              PointsPackage newPackage;
                              newPackage.setup(points, color);
                              newPackage.setEraser(false); ////// DE GOM
                              
                              mPointsQueueLock.lock();
                              pointsQueue.push(newPackage);
                              mPointsQueueLock.unlock();
                              
                          });
    
    mReceiver.setListener("/shape",
                          [&](const osc::Message &msg){
                              int totals = msg.getNumArgs();
                              
                              int incomingGroupId = msg[0].int32();
                              if (incomingGroupId != groupId) return;
                              
                              std::string shape = msg.getArgString(1);
                              std::string color = msg.getArgString(2);
                              std::vector<ci::vec3> points;
                              for (int i = 3; i < totals; i += 3){
                                  points.push_back(ci::vec3(msg[i].flt(), msg[i + 1].flt(), msg[i + 2].flt()));
                              }
                              PointsPackage newPackage;
                              newPackage.setup(points, color);
                              newPackage.setShape(shape);
                              mShapesQueueLock.lock();
                              shapesQueue.push(newPackage);
                              mShapesQueueLock.unlock();
                              
                          });
    
    // For a description of the below setup, take a look at SimpleReceiver. The only difference
    // is the usage of the mutex around the connection map.
    try {
        mReceiver.bind();
    }
    catch (const osc::Exception &ex) {
        CI_LOG_E("Error binding: " << ex.what() << " val: " << ex.value());
    }
    
    
    mReceiver.listen(
                     [](asio::error_code error, protocol::endpoint endpoint) -> bool {
                         if (error) {
                             CI_LOG_E("Error Listening: " << error.message() << " val: "
                                      << error.value() << " endpoint: " << endpoint);
                             return false;
                         }
                         else
                             return true;
                     });
    
    
    
    // Now that everything is setup, run the io_service on the other thread.
    mThread = std::thread(std::bind(
                                    [](std::shared_ptr<asio::io_service> &service){
                                        service->run();
                                    }, mIoService));
    
    
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
    
//    while( mListener.hasWaitingMessages() ) {
//        osc::Message message;
//        mListener.getNextMessage( &message );
//
//        // return from our own broadcast
//        std::string remoteIp = message.getRemoteIp();
//
//        if( remoteIp ==  mOwnIpAdress)
//            continue;
//
//        std::string remoteLastNr = extractLastIpNr(remoteIp);
//        std::string const adress = message.getAddress();
//
//        int incomingGroupId =  message.getArgAsInt32(0);
//
//        // discard packages from other groups
//        if(incomingGroupId == groupId){
//
//            if(mAliveIps.find(remoteLastNr) == mAliveIps.end()){
//                onNewConnection.emit(remoteLastNr);
//            }
//            mAliveIps[remoteLastNr] = ci::app::getElapsedSeconds();
//            onAlivePing.emit(remoteLastNr);
//            if(adress == "points"){
//                int totals = message.getNumArgs() ;
//                bool isEraserOn = message.getArgAsInt32(1);
//                std::string color = message.getArgAsString(2);
//                std::vector<ci::vec3> points;
//                for(int i=3;i < totals;i+=3){
//                    float brushSize = message.getArgAsFloat(i+2) / GS()->performanceDownScale.value();
//                    points.push_back(ci::vec3(message.getArgAsFloat(i),message.getArgAsFloat(i+1),brushSize));
//                }
//                PointsPackage newPackage;
//                newPackage.setup(points, color);
//                newPackage.setEraser(isEraserOn);
//
//                onReceivePoints.emit(newPackage);
//            }else if(adress == "shape"){
//                std::vector<ci::vec3> points;
//                std::string shape = message.getArgAsString(1);
//                std::string color = message.getArgAsString(2);
//                for(int i=3;i <message.getNumArgs();i+=3){
//                    float brushSize = message.getArgAsFloat(i+2) / GS()->performanceDownScale.value();
//                    points.push_back(ci::vec3(message.getArgAsFloat(i),message.getArgAsFloat(i+1),brushSize));
//                }
//                PointsPackage newPackage;
//                newPackage.setup(points, color);
//                newPackage.setShape(shape);
//
//                onReceiveShapes.emit(newPackage);
//
//            }
//        }
//    }
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
//    mOwnIpAdress = System::getIpAddress();
//    std::vector<std::string> hostSplit = ci::split(mOwnIpAdress, ".");
//    std::vector<std::string> subnetSplit = ci::split(System::getSubnetMask(), ".");
//    mLastIpNr =  hostSplit.back(); // getLastNummerIp(System::getIpAddress());
//    std::string broadcast = "";
//    for(int i=0; i < subnetSplit.size();++i){
//        broadcast += subnetSplit[i] == "0" ? "255" : hostSplit[i];
//        if(i < subnetSplit.size()-1) broadcast += ".";
//    }
//    int port = 3000;
//    mSender.setup( broadcast, port, true );
//    // mSender.setup( "127.0.0.1", port, false );
//    lastBroadcast = app::getElapsedSeconds();
}



void NetworkHelper::sendAlive(){
    osc::Message message;
    message.setAddress("/alive");
    message.append(groupId);
    mSender.send(message);
}

void NetworkHelper::sendPoints(std::vector<ci::vec3>& points, bool isEraserOn,std::string color){
    osc::Message message;
    message.setAddress("/points");
    message.append(groupId);
    message.append(isEraserOn);
    message.append(color);
    for(vec3& p : points){
        message.append(p.x);
        message.append(p.y);
        message.append(p.z * GS()->performanceDownScale.value());
    }
    mSender.send(message);
    lastBroadcast = app::getElapsedSeconds();
}

void NetworkHelper::sendTwoPointShape(vec3& point1,vec3& point2, std::string shape,std::string color){
    osc::Message message;
    message.setAddress("/shape");
    
    message.append(groupId);
    message.append(shape);
    message.append(color);
    
    message.append(point1.x);
    message.append(point1.y);
    message.append(point1.z * GS()->performanceDownScale.value());
    message.append(point2.x);
    message.append(point2.y);
    message.append(point2.z * GS()->performanceDownScale.value());
    
    mSender.send(message);
    lastBroadcast = app::getElapsedSeconds();
    
}

void NetworkHelper::cleanup()
{
    mWork.reset();
    mIoService->stop();
    mThread.join();
}

