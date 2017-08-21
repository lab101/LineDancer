//
//  BrushManager.cpp
//  LiveDraw
//
//  Created by lab101 on 06/09/16.
//
//

#include "BrushManager.hpp"
#include "cinder/app/app.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;

void BrushManager::loadBrushes(){
    
    
    surfaceWidth = 100;
    //mBrush = ci::Surface(surfaceWidth,surfaceWidth,true);
   
    
    
    // setup shader
    try {
        mShader = gl::GlslProg::create(loadAsset( "shaders/shader_es2.vert" ), loadAsset( "shaders/shader_es2.frag" ));
        mShader->uniform( "uRadius", 10.0f );

    }
    catch( gl::GlslProgCompileExc ex ) {
        CI_LOG_E("error loading brush shader");
        CI_LOG_E(ex.what());
    }


    CI_LOG_I("set GL_PROGRAM_POINT_SIZE");
    gl::enable(GL_PROGRAM_POINT_SIZE);
    CI_LOG_I("set GL_VERTEX_PROGRAM_POINT_SIZE");
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    
}


void BrushManager::drawBrush(std::vector<vec3>& points,float softness){

    ci::gl::VertBatchRef mBatch = gl::VertBatch::create();
    
    for(vec3& p : points){
        mBatch->vertex( p );
        mBatch->color( mActiveColor );
    }
    
    
    gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
    
    gl::ScopedGlslProg glslProg( mShader );

    mBatch->draw();


}


void BrushManager::generateBrush(float size,float softness){
    brushScale = size;
    
    
    ci::vec2 center(surfaceWidth/2,surfaceWidth/2);

    float startSmoothStep = size - (size*softness);
    
    for(float x=0;x< surfaceWidth; x++){
        for(int y=0;y< surfaceWidth; y++){
            
            ci::vec2 const p(x,y);
            
            float distance = glm::distance(center, p);
            float a = 1- glm::smoothstep(startSmoothStep, size, distance);
            
            a = fmaxf(0, a);
            
            mBrush.setPixel(p, ci::ColorA(0,0,0,a));
        }
    }
    
    
    dot = ci::gl::Texture::create(mBrush);

}

