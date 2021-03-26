#version 400

attribute vec4 position;
attribute vec2 uv0;

uniform mat4 worldViewProj;

out vec2 uv;

void main()
{
	gl_Position = worldViewProj * position;
	uv = uv0;
}