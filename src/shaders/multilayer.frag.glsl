#version 400

uniform vec3 lightPosition;

in vec3 interpolatedNormal;
in vec3 interpolatedPosition;
in vec2 interpolatedUv;
in vec3 interpolatedModelPosition;
in float metaball1Dist;
in float metaball2Dist;
in float metaball3Dist;

out vec4 fragColor;

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v){ 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y
  i = mod289(i); 
  vec4 p = permute( permute( permute(   i.z + vec4(0.0, i1.z, i2.z, 1.0 ))+ i.y + vec4(0.0, i1.y, i2.y, 1.0 )) + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;
  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)
  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)
  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);
  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));
  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;
  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;
  vec4 m = max(0.5 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 105.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), dot(p2,x2), dot(p3,x3) ) )  / 1.4 + 0.6;
}

const int colorCount = 2;

const vec4 colors[] = vec4[colorCount](
  vec4(0.0745,0.0196,0.0196, -0.00),
  vec4(0.6706,0.6902,0.6706, 0.07)
);

vec3 colorMap(float val) {
  if (val < colors[0].a) {
    return colors[0].xyz;
  }

  if (val > colors[colorCount - 1].a) {
    return colors[colorCount - 1].xyz;
  }

  for(int i = 0; i < colorCount - 1; i++)
  {
    if (val >= colors[i].a && val < colors[i + 1].a) {
      float a = (val - colors[i].a) / (colors[i + 1].a - colors[i].a);

      return mix(colors[i].xyz, colors[i + 1].xyz, a);
    }
  }

  return vec3(0, 0, 1);
}

void main(void) {
    
    vec3 uv = interpolatedModelPosition / 500;
    vec3 interpolatedColor = colorMap(uv.x * 10  + 0.1);

    float metaballStrength = 0;
    float minimum = 77;
    float maximum = 90;
    float maxVal = 1;
    if (metaball1Dist > minimum && metaball1Dist < maximum) {
      metaballStrength = 1 - min(maxVal,(metaball1Dist - minimum) / (maximum - minimum));
    }

    minimum = 83;
    maximum = 97;
    if (metaball2Dist > minimum && metaball2Dist < maximum) {
      float str = 1 - min(maxVal,(metaball2Dist - minimum) / (maximum - minimum));
      if (metaballStrength > 0) {
          metaballStrength = max(str, metaballStrength);
      } else {
        metaballStrength = str;
      }
    }

    minimum = 73;
    maximum = 100;
    if (metaball3Dist > minimum && metaball3Dist < maximum) {
      float str = 1 - min(maxVal,(metaball3Dist - minimum) / (maximum - minimum));
      if (metaballStrength > 0) {
          metaballStrength = max(str, metaballStrength);
      } else {
        metaballStrength = str;
      }
    }

    vec3 highlightColor = vec3(0.2863,0.1137,0.0078);
    interpolatedColor = mix(interpolatedColor, highlightColor, metaballStrength * (snoise(uv * 2) / 2 + 0.5) * 1.5);

    vec3 n = normalize(interpolatedNormal);

    vec3 lightColor = vec3(1.0,1.0,1.0);
    vec3 diffuse = lightColor * interpolatedColor * max(dot(n, normalize(lightPosition - interpolatedPosition)), 0);

    vec3 reflection = normalize(reflect(normalize(interpolatedPosition - lightPosition), n));
    vec3 reflectionColor = vec3(1.0,1.0,1.0);
    vec3 specular = lightColor * reflectionColor * pow(max(dot(normalize(vec3(0,0,0) - interpolatedPosition), reflection), 0), 500);

    vec3 color = diffuse + specular;

    fragColor = vec4(color, 1.0);

}