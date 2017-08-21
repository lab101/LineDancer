#version 150

in vec4 ciPosition;
in vec3 ciColor;

uniform mat4 ciModelViewProjection;

out vec3 vColor;

void main()
{
    
    gl_PointSize = max(ciPosition.z,2.0);
    vec4 pos = ciPosition;
    pos.z=0;
    
    gl_Position = ciModelViewProjection * pos;
    vColor = ciColor;
    

}