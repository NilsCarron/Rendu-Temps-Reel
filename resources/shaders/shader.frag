#version 450

out vec4 color;
in vec3 normalOut ;
in vec3 positionOut;
uniform vec4 colorPerTime;
uniform vec3 Le;
uniform vec3 albedo;
uniform vec3 positionLight;


void main()
{ 
vec3 directionToLight = positionLight - positionOut;
float d2 = dot(directionToLight, directionToLight);
vec3 omegaI = normalize(directionToLight);
color = vec4(Le/d2  * dot(normalOut,omegaI ) * albedo, 1.0);
   // color = vec4(abs(normalOut), 1.0);
}
