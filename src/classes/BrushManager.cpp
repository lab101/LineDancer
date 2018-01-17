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

void BrushManager::setup(){
    
    
    isEraserOn = false;
    
    
    // setup shader
    try {
        mShader = gl::GlslProg::create(loadResource( "shader_es2.vert" ), loadResource("shader_es2.frag" ));
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
        mBatch->color(isEraserOn ? ci::ColorA(1,1,1,1)  : mActiveColor );
    }
    
    
//    gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
    
    gl::ScopedGlslProg glslProg( mShader );

    mBatch->draw();


}

