#version 120

<<<<<<< HEAD
attribute vec2 position;

uniform mat3 mvpMat;

varying vec2 pos;

void main() 
{
	pos = ( mvpMat * vec3( position, 1 ) ).xy;
	gl_Position= vec4( pos, 0.0, 1.0);
=======
uniform mat4 mvpMat;

attribute vec3 pos;
varying vec3 pos2;

attribute vec4 color;
varying vec4 color2;

void main(){
	pos2 = (mvpMat*vec4(pos,1)).xyz;
	gl_Position = vec4(pos2,1);
	color2 = color;
>>>>>>> bf5b4d174ebe221c59537bbbfd5f22b1f808f8df
}