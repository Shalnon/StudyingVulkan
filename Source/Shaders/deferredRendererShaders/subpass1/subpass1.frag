//glslc --target-env=vulkan1.3 -fshader-stage=frag -o frag.spv triangle.frag 

#version 420

precision highp float;


layout( binding = 0 ) uniform UniformBufferObject
{
    // Scene Transform
    mat4 sceneTransform;
    vec4 sceneScale;

    // Scene ambient color
    vec4  ambient_color;

    // Light Info
    vec4 lightLocation;
    vec4 lightIntensities;
} ubo;

layout (input_attachment_index=0, set=0, binding=1 ) uniform subpassInput diffuseColorInputAttachment;
layout (input_attachment_index=1, set=0, binding=2 ) uniform subpassInput normalVectorInputAttachment;
layout (input_attachment_index=2, set=0, binding=3 ) uniform subpassInput depthStencilInputAttachment;


// Assume this is a full screen quad?
layout(location = 0) in vec3 in_positionOnFullScreenQuad;

layout(location = 0) out vec3 fragColor;


void main()
{
    vec4 diffuseColor     = subpassLoad(diffuseColorInputAttachment);
    vec4 surfaceNormal    = subpassLoad(normalVectorInputAttachment);
    vec4 fragDepthStencil = subpassLoad(depthStencilInputAttachment);

    // Assuming fragdepthStencil.r is depth
    vec3 locationOnMeshSurface = vec3(in_positionOnFullScreenQuad.xy, fragDepthStencil.r);


    float distanceFromLight = distance(ubo.lightLocation.xyz,  locationOnMeshSurface);
	vec3  surfaceToLight    = normalize(ubo.lightLocation.xyz- locationOnMeshSurface);
	float cosaoi            = dot(surfaceNormal.xyz, surfaceToLight);

	float Diffuse_Coefficient = max(0.0,cosaoi);

	vec3 Diffuse = diffuseColor.rgb * ubo.lightIntensities.xyz * Diffuse_Coefficient;

	fragColor = Diffuse + ubo.ambient_color.rgb;
}
