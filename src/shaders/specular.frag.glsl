#version 400

uniform vec3 lightPosition;

in vec3 interpolatedNormal;
in vec3 interpolatedPosition;
in vec2 interpolatedUv;
in vec3 interpolatedModelPosition;

out vec4 fragColor;


void main(void) {
    vec3 n = normalize(interpolatedNormal);

    vec3 lightColor = vec3(1.0,1.0,1.0);

    vec3 reflection = normalize(reflect(normalize(interpolatedPosition - lightPosition), n));
    vec3 reflectionColor = vec3(1.0,1.0,1.0);
    vec3 specular = lightColor * reflectionColor * pow(max(dot(normalize(vec3(0,0,0) - interpolatedPosition), reflection), 0), 500);

    fragColor = vec4(specular, 1.0);
}