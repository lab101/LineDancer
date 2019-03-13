//
//  TextButton.cpp
//  LiveDraw
//
//  Created by lab101 on 14/09/16.
//
//

#include "TextButton.hpp"

using namespace ci;

TextButton::TextButton(float radius, std::string text, ci::Font* font){
    mText       = text;
    mTextFont   = font;
    mRadius  = radius;

    renderText();
}


void TextButton::renderText(){
    
    // always do retina.
    float displayScale = 2;// ci::app::getWindowContentScale();

    
    TextBox tbox = TextBox().alignment( TextBox::CENTER ).font( (*mTextFont) ).size( ivec2( mRadius * 2 * displayScale, TextBox::GROW ) ).text( mText );
    tbox.setColor( mColor );
    gl::TextureRef  newTexture = gl::Texture2d::create( tbox.render() );
    setTexture(newTexture);
}





