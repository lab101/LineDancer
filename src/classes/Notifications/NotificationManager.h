#pragma once

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/app/AppBase.h"



#include "Singleton.hpp"

// CHART STUFF
class ChartPoint{

public:
	float mPoint;
	std::int64_t mTime;
	bool markedForRemoval;

	ChartPoint(float point, int64_t time){
		mPoint = point;
		mTime = time;// ci::app::getElapsedSeconds();
	}
};

class ChartData{
	std::list<ChartPoint> mPoints;
	ci::Color mColor;

public:
	void setColor(ci::Color color){
		mColor = color;
	}

	ci::Color& getColor(){
		return mColor;
	}

	void addPoint(float data, int64_t time){
		std::cout << data << " add time " << time << std::endl;;
		mPoints.push_back(ChartPoint(data, time));
	}


	std::list<ChartPoint>& getPoints(){
		return mPoints;
	};


};



// NOTIFICATION STUFF
class Notification{

	ci::gl::Texture2dRef	mTexture;
	ci::Anim<float>         mOpacity;
	ci::vec2                mPosition;
	bool					mFirstDraw = false;

public:

	bool markedForRemoval = false;

	void setTexture(ci::gl::Texture2dRef text){
		mTexture = text;
		markedForRemoval = false;
	}

	float getTextureHeight(){
		return mTexture->getSize().y;
	}

	void fadeIn(ci::vec2 position){
		mPosition = position;
		ci::app::timeline().apply(&mOpacity, 0.0f, 1.0f, 1.0f, ci::EaseOutSine());
	}

	void startFade(){
		ci::app::timeline().appendTo(&mOpacity, 0.0f, 1.0f, ci::EaseInSine())
			.delay(4)
			.finishFn([&](){markedForRemoval = true; });
	}

	void move(float amount){
		mPosition.y += amount;
	}

	void draw(){
		if (!mFirstDraw){
			startFade();
			mFirstDraw = true;
		}

		ci::gl::enableAlphaBlending();
//		cinder::gl::color(0, 0, 0, mOpacity());
//		ci::gl::draw(mTexture, mPosition + ci::vec2(1, 1));
		cinder::gl::color(1, 1, 1, mOpacity());
		ci::gl::draw(mTexture, mPosition);

	}


};

class NotificationManager{

	bool isFontLoaded = false;
	ci::Font mFont;
	std::list< std::shared_ptr<Notification> > notifications;
	std::map<std::string, ChartData> charts;
	std::string getTimeString();

public:

	void addMessage(std::string messagem, ci::Color);
	void addChartData(std::string name, float data, int64_t time, ci::Color color);
	void draw();

	void drawMessages();
	void drawCharts();


//	void setup();

	bool hasMessages();

	NotificationManager();

};


typedef Singleton<NotificationManager> NotificationManagerSingleton;

inline void notify(std::string message, ci::Color color = ci::Color(1, 1, 1)){
	NotificationManagerSingleton::Instance()->addMessage(message, color);
}


inline void addChart(std::string name, float data, int64_t time, ci::Color color){
	NotificationManagerSingleton::Instance()->addChartData(name, data, time, color);
}
