//
//  NotificationManager.cpp
//  VideoPlayer
//
//  Created by Kris Meeusen on 31/08/15.
//
//

#include "NotificationManager.h"
#include <string>
#include <algorithm>
#include "cinder/Utilities.h"
#include <chrono>

using namespace ci;
using namespace ci::app;
using namespace std::chrono;


NotificationManager::NotificationManager(){

}



std::string NotificationManager::getTimeString(){
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), " %H-%M-%S");
    return ss.str();
}



void NotificationManager::addMessage(std::string message, Color color){

    // set uppercase
    std::transform(message.begin(), message.end(),message.begin(), ::toupper);

    if(!isFontLoaded){
        mFont = Font("Arial",20);
        isFontLoaded = true;
    }
    
    try{
    
    std::shared_ptr<Notification> p =  std::make_shared<Notification>();
    notifications.push_back(p);
    
    TextLayout simple;
    simple.setFont( mFont );
    simple.setColor( color );
    simple.addLine(getTimeString() + "    " +  message );


    
    p->setTexture(gl::Texture2d::create( simple.render( true, false ) ));
    
    
    const float textureHeight = p->getTextureHeight();
    for (std::shared_ptr<Notification> n  : notifications)
    {
        n->move(textureHeight);
    }

    p->fadeIn(vec2(10,10));
    }catch(...){
            
        }

}



void NotificationManager::addChartData(std::string name,float data ,int64_t time, ci::Color color){
    charts[name].addPoint(data,time);
    charts[name].setColor(color);
}


void NotificationManager::drawMessages(){
    // clear items
    for(auto it = notifications.begin(), ite = notifications.end(); it != ite;)
    {
        if((*it)->markedForRemoval)
            it = notifications.erase(it);
        else
            ++it;
    }
    
    
    gl::enableAlphaBlending();
    for (std::shared_ptr<Notification> n  : notifications)
    {
        if(!n->markedForRemoval){
            n->draw();
        }
    }


}



void NotificationManager::drawCharts(){
    typedef std::map<std::string, ChartData>::iterator charIt;
    for(charIt iterator = charts.begin(); iterator != charts.end(); iterator++) {
        
        
        
        for(auto it = iterator->second.getPoints().begin(), ite = iterator->second.getPoints().end(); it != ite;)
        {
            if((*it).markedForRemoval)
                it = iterator->second.getPoints().erase(it);
            else
                ++it;
            
        }
        
    }
    
    
    float xOffset = 0;
    
    typedef std::map<std::string, ChartData>::iterator charIt;
    for(charIt iterator = charts.begin(); iterator != charts.end(); iterator++) {
        
        
        vec2 firstPoint;
        bool isFirst = true;
        
                ci::gl::color(iterator->second.getColor());
                gl::drawStringCentered(iterator->first, vec2(getWindowCenter().x, 100 + xOffset),iterator->second.getColor());
        
        
        
             //   milliseconds time =
        
                for(ChartPoint& p  : iterator->second.getPoints()){
        
                    int64_t const t = duration_cast < milliseconds>(system_clock::now().time_since_epoch()).count();
        
                    float div = (t - p.mTime) * 0.1f;
        
                    vec2 point(div, 500 + p.mPoint  );
                    //std::cout << point << " // " <<  p.mTime << std::endl;
        
                    p.markedForRemoval = point.x > 1080;
        
                    if (isFirst) {
                        isFirst = false;
                    }else{
                   //   gl::drawLine(firstPoint, point);
                    }
                    
                    gl::drawSolidCircle(point, 2);
        
                    
                    firstPoint = point;
                }
        
        xOffset += 40;
        
    }

}


bool NotificationManager::hasMessages(){
    return notifications.size() > 0;
}



void NotificationManager::draw(){
    
    //drawCharts();
    ci::gl::ScopedGlslProg shaderScp( ci::gl::getStockShader( ci::gl::ShaderDef().texture().color()));
    drawMessages();
    cinder::gl::color(1, 1, 1, 1);

}
