glfwWindow >/

vulkan instance >/
debugger validation layer >/
swapchain >/
phyiscal device >/
logical device >/

pipelinelayout >/
renderpass >/
shaders >/
pipeline >/
vertex buffer assembler >/
rendering command buffer >/
render function that does a render pass with command bufffers >/

render our first triangle >/

make uniforms with their descriptor sets to have a camera view
make a prespective camera >/

make an SVO >/
SVO sample function based generation >/
make a perlin noise based sample function
SVO read function with callback to generate the mesh >/
assemble render objects from SVO using the callback from ReadSVO
render vertex buffer using the renderer >/

generate 4 chunks and render them successfuly
implement player navigation with inputs to change camera



gamescripts aka exposed API being used to run a simple player controller
    pos/direction/fov camera handle
    update function
    startup function

UI pipeline that takes UIrenderObjects

// audio
fix FL studio
get FL studio instruments
make some FL studio music









///////////////////////////////////////////////////////////
        // code copy + paste //
//////////////////////////////////////////


    WL_LOG(WL_LOG_TRACE, "creating  ...");
    VkResult layout_result;
    layout_result = ;
    if(layout_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create ");
        return;
    }
    WL_LOG(WL_LOG_TRACE, " created successfully!");




    #ifdef WL_DEBUG
    printf("\n");
    #endif



    for (size_t i = 0; i < ; i++){
        
    }



