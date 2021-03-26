#version 400


varying vec2 uv;

uniform sampler2D Scene;
uniform sampler2D Depth;
uniform int passId;


void main ()
{
	bool firstPass = true;
	if (passId > 0) {
		firstPass = false;
	}

	vec4 color = vec4(texture2D(Depth, uv)) * 0.00001 + texture2D(Scene, uv);

	if (firstPass) {
		color = color + 0.000001;
	}

	if (uv.x == 0 && uv.y == 0){
		color = vec4(1, 0, 0, 1);
	}

	gl_FragColor = color;
}