#version 400

uniform vec3 lightPosition;

in vec3 interpolatedNormal;
in vec3 interpolatedPosition;
in vec2 interpolatedUv;
in vec3 interpolatedModelPosition;

out vec4 fragColor;

float numberOfTilesWidth = 1;
float numberOfTilesHeight = 1;
float amplitude = 40.0;
vec3 jointColor = vec3(0.72, 0.72, 0.72);

vec3 tileSize = vec3(1.1, 1.0, 1.1);
vec3 tilePct = vec3(0.98, 1.0, 0.98);

float rand(vec2 n) {
	return fract(cos(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 n) {
	vec2 d = vec2(0.0, 1.0);
	vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

float turbulence(vec2 P)
{
	float val = 0.0;
	float freq = 1.0;
	for (int i = 0; i < 4; i++)
	{
		val += abs(noise(P*freq) / freq);
		freq *= 2.07;
	}
	return val;
}

float roundF(float number){
	return sign(number)*floor(abs(number) + 0.5);
}

vec3 marble_color(float x)
{
	vec3 col;
	x = 0.5*(x + 1.);
	x = sqrt(x);             
	x = sqrt(x);
	x = sqrt(x);
	col = vec3(.2 + .75*x);  
	col.b *= 0.95;           
	return col;
}

vec3 marbleColor() {
    vec2 uv = ((interpolatedModelPosition - vec3(0, -150, -300)) / 500).yz;
    float brickW = 1.0 / numberOfTilesWidth;
	float brickH = 1.0 / numberOfTilesHeight;
	float jointWPercentage = 0.01;
	float jointHPercentage = 0.01;
	vec3 color = vec3(0.5, 0.5, 0.5);
	float yi = uv.y / brickH;
	float nyi = roundF(yi);
	float xi = uv.x / brickW;

	// if (mod(floor(yi), 2.0) == 0.0){
	// 	xi = xi - 0.5;
	// }

	float nxi = roundF(xi);
	vec2 brickvUV = vec2((xi - floor(xi)) / brickH, (yi - floor(yi)) / brickW);
    float t = 6.28 * brickvUV.x / (tileSize.x + noise(vec2(uv)*6.0));
    t += amplitude * turbulence(brickvUV.xy);
    t = sin(t);
    color = marble_color(t);
    return color;
}

void main(void) {
    
    vec3 interpolatedColor = marbleColor();

    vec3 n = normalize(interpolatedNormal);

    // vec3 ambientColor = vec3(1.0,1.0,1.0);
    // vec3 ambientReflection = vec3(0.3,0.3,0.3);
    // vec3 ambient = ambientReflection * ambientColor;

    vec3 lightColor = vec3(1.0,1.0,1.0);
    vec3 diffuse = lightColor * interpolatedColor * max(dot(n, normalize(lightPosition - interpolatedPosition)), 0);

    vec3 reflection = normalize(reflect(normalize(interpolatedPosition - lightPosition), n));
    vec3 reflectionColor = vec3(1.0,1.0,1.0);
    vec3 specular = lightColor * reflectionColor * pow(max(dot(normalize(vec3(0,0,0) - interpolatedPosition), reflection), 0), 500);

    vec3 color = diffuse + specular;

    //vec3 pos = (interpolatedModelPosition - vec3(0, -200, -250)) / 300; // 300

    fragColor = vec4(color, 1.0);// * 0.00001 + vec4( interpolatedNormal, 1.0);

}