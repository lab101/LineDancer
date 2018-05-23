//
//  BrushManager.cpp
//  LiveDraw
//
//  Created by lab101 on 06/09/16.
//
//

#include "BrushManager.hpp"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
#include "GlobalSettings.h"

using namespace ci;
using namespace ci::app;

void BrushManager::setup(){
    
    
    isEraserOn = false;
    
    
    // setup shader
    try {
        mShader = gl::GlslProg::create(loadResource( "shader_es2.vert" ), loadResource("shader_es2.frag" ));

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
        mBatch->color(isEraserOn ? GS()->fboBackground  : GS()->brushColor );
    }
    
    
    
    gl::ScopedGlslProg glslProg( mShader );

    mBatch->draw();


}

