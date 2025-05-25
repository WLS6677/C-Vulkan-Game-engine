glfwWindow >/

vulkan instance >/
debugger validation layer >/
swapchain
phyiscal device >/
logical device >/

pipelinelayout >/
renderpass
shaders
pipeline
command buffer
render function that does a render pass with command bufffers and intakes render objects

render our first triangle

make an SVO >/
SVO sample function based generation >/
make a perlin noise based sample function
SVO read function with feedbackk
assemble render objects from SVO using the callback from ReadSVO
render vertex buffer using the renderer

gamescripts aka exposed API being used to run a simple game

UI pipeline that takes UIrenderObjects

// audio
fix FL studio
get FL studio instruments
make some FL studio music





WL_LOG(WL_LOG_TRACE, "creating  ...");
    VkResult layout_result;
    layout_result = ;
    if(layout_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create ");
        return;
    }
WL_LOG(WL_LOG_TRACE, " created successfully!");



