//glslc --target-env=vulkan1.3 -fshader-stage=frag -o frag.spv subpass1.frag 

#version 420

precision highp float;


layout( binding = 0 ) uniform UniformBufferObject
{
    // Scene Transform
    mat4 sceneTransform;
    vec4 sceneScale;

    // Projection matrix
    mat4 projectionMatrix;

    // Scene ambient color
    vec4  ambient_color;

    // Light Info
    vec4 lightLocation;
    vec4 lightIntensities;
} ubo;

layout (input_attachment_index=0, set=0, binding=1 ) uniform subpassInput diffuseColorInputAttachment;
layout (input_attachment_index=1, set=0, binding=2 ) uniform subpassInput normalVectorInputAttachment;
layout (input_attachment_index=2, set=0, binding=3 ) uniform subpassInput worldPositionInputAttachment;
layout (input_attachment_index=3, set=0, binding=4 ) uniform subpassInput depthStencilInputAttachment;


// Assume this is a full screen quad?
layout(location = 0) in vec3 in_positionOnFullScreenQuad;

layout(location = 0) out vec3 fragColor;


void main()
{
    vec4 diffuseColor         = subpassLoad(diffuseColorInputAttachment);
    vec4 surfaceNormal        = subpassLoad(normalVectorInputAttachment);
    vec4 fragDepthStencil     = subpassLoad(depthStencilInputAttachment);
    vec4 surfaceWorldPosition = subpassLoad(worldPositionInputAttachment);

    // Assuming fragdepthStencil.r is depth
    //vec3 locationOnMeshSurface = vec3(in_positionOnFullScreenQuad.xy, fragDepthStencil.r);  // might still want to use this because it makes the lighting consistent potentially

    //vec4 lightLocation = ubo.lightLocation * ubo.sceneTransform;
    float distanceFromLight = distance(ubo.lightLocation.xyz,  surfaceWorldPosition.xyz);
	vec3  surfaceToLight    = normalize(ubo.lightLocation.xyz - surfaceWorldPosition.xyz);
	float cosaoi            = dot(surfaceNormal.xyz, surfaceToLight);

	float Diffuse_Coefficient = max(0.0,cosaoi);

	vec3 Diffuse = diffuseColor.rgb * ubo.lightIntensities.xyz * Diffuse_Coefficient;

	fragColor = Diffuse + ubo.ambient_color.rgb;
}
