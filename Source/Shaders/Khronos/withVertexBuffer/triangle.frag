#version 420

precision highp float;

layout(location = 0)      in vec3 in_color;
layout(location = 1)      in vec3 in_normal;
layout(location = 2)      in vec3 in_positionOnSurface;
layout(location = 3) flat in vec3 in_lightPosition;

layout(location = 0) out vec3 fragColor;

void main()
{
    float distanceFromLight = distance(in_lightPosition, in_positionOnSurface);
	vec3  surfaceToLight    = normalize(in_lightPosition - in_positionOnSurface);
	float cosaoi            = dot(in_normal.xyz, surfaceToLight);

	float Ambient_Coefficient = 0.15;
	vec3  Ambient_Color       = vec3(0.4,0.4,0.4);
	vec3  Light_Intensities   = vec3(1.0,1.0,1.0);
	float Diffuse_Coefficient = max(0.0,cosaoi);

	vec3 Ambient = Ambient_Coefficient * Ambient_Color * Light_Intensities;
	vec3 Diffuse = in_color * Light_Intensities * Diffuse_Coefficient;


	fragColor = Diffuse + (Ambient_Color*Ambient_Coefficient);

	//out_color = in_color.rgb;
}
