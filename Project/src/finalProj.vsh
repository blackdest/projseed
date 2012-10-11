#version 120

uniform mat4 mvpMat;

attribute vec3 pos;
varying vec3 pos2;

attribute vec4 color;
varying vec4 color2;

void main(){
	pos2 = (mvpMat*vec4(pos,1)).xyz;
	gl_Position = vec4(pos2,1);
	color2 = color;
}