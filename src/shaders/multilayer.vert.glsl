#version 400

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

in vec4 position;
in vec3 normal;
in vec2 uv;
uniform float normalOverride;

out vec3 interpolatedNormal;
out vec3 interpolatedPosition;
out vec3 interpolatedModelPosition;
out vec2 interpolatedUv;
out float metaball1Dist;
out float metaball2Dist;
out float metaball3Dist;

void main(void) {

    metaball1Dist = distance(position.xyz, vec3(10.0, 90.0, -160.0));
    metaball2Dist = distance(position.xyz, vec3(10.0, 99.0, -120.0));
    metaball3Dist = distance(position.xyz, vec3(0, 60.0, -170.0));

    mat4 modelViewMatrix = viewMatrix * modelMatrix;
    mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));

    if (normalOverride > 0) {
        interpolatedNormal = normalize(normalMatrix * vec3(1, 0, 0));
    } else {
        interpolatedNormal = normalize(normalMatrix * normal);
    }
    interpolatedPosition = (modelViewMatrix * position).xyz;
    interpolatedUv = uv;

    vec4 pos = projectionMatrix * modelViewMatrix * position;
    interpolatedModelPosition = (modelMatrix * position).xyz;
    gl_Position = pos;
}