#version 150


//uniform float uRadius;

in vec3 vColor;

out vec4 outColor;

void main()
{
    
    vec3 d = vec3(gl_PointCoord.xy,0) - vec3(0.5,0.5,0);
    
    float l = length(d);
    float c =  1 - smoothstep(0.1, .5, l);

    outColor = vec4(vColor*c,c);//vec4(1,0,0,0.1);
    
}
