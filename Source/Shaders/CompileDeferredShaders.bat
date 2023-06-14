

glslc --target-env=vulkan1.3 -fshader-stage=vert -o .\\deferredRendererShaders\\subpass0\\vert.spv .\\deferredRendererShaders\\subpass0\\subpass0.vert
glslc --target-env=vulkan1.3 -fshader-stage=frag -o .\\deferredRendererShaders\\subpass0\\frag.spv .\\deferredRendererShaders\\subpass0\\subpass0.frag 
glslc --target-env=vulkan1.3 -fshader-stage=vert -o .\\deferredRendererShaders\\subpass1\\vert.spv .\\deferredRendererShaders\\subpass1\\subpass1.vert
glslc --target-env=vulkan1.3 -fshader-stage=frag -o .\\deferredRendererShaders\\subpass1\\frag.spv .\\deferredRendererShaders\\subpass1\\subpass1.frag