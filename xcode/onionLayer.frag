
#version 150

out vec4 oColor;

uniform sampler2D     uTex0;
uniform sampler2D     uTex1;


in vec2             TexCoord0;

void main(void)
{
    vec4 color = texture( uTex0, TexCoord0 );
    vec4 mask = texture( uTex1, TexCoord0 );
    
    color.r *= mask.r;
    color.g *= mask.g;
    //color.b *= mask.b;
    oColor =  color;
}
