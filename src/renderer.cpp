#include <renderer.h>
#include <vulkan/vulkan.h>

struct WLRenderer {

    VkInstance vulkan_instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkDebugUtilsMessengerEXT debug_messenger;
    // SwapChain
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkImage* pSwapchain_images;
    VkFormat swapChain_image_Format;
    VkExtent2D swapChain_extent;
    VkImageView* pSwapChain_image_views;
    // queueueueueueueueueueueueueueues
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;
    // PIPELINE WOOOO
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorPool descriptor_pool;
    VkPipelineLayout pipeline_layout;
    VkRenderPass render_pass;
    VkPipeline graphics_pipeline;
    VkVertexInputAttributeDescription* attribute_descs;
    // buffers yaaay
    VkFramebuffer* swapChain_framebuffers;
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;
    VkBuffer index_buffer;
    VkDeviceMemory index_buffer_memory;

    VkBuffer* pUniformBuffers;
    VkDeviceMemory* pUniformBuffersMemory;
    void** ppUniformBuffersMapped;

    // Command Stuff
    VkCommandPool graphicsCommandPool;
    VkCommandPool transferCommandPool;
    VkCommandBuffer* pCommandBuffers;
    VkSemaphore* pImageAvailableSemaphores;
    VkSemaphore* pRenderFinishedSemaphores;
    VkFence* pInFlightFences;
    uint32_t currentFrame = 0;

};

/*
this stuff will be used for rasterized projects: rendering 3d models, 2 games, etc
and will need to be rewritten for a raytraced/ raymarched renderer.

    Render pass

    Framebuffers

    Graphics pipelines

    Descriptor layouts/sets

    Vertex/index buffers

    Uniform buffer logicss

    Shader modules (unless shared with compute)
    
*/

//the functions needed for creating the VkInstance
const char** get_required_vulkan_extensions(uint32_t* pCount){

    //get the number of extensions
    uint32_t glfwExtensionCount = 0;  
    const char** glfw_extensions = wlGetRequiredWindowInstanceExtensions(&glfwExtensionCount);

    // this one will be allocted so it can be outside the scope of the function
    const char** extensions;

    //adding 
    #ifdef WL_DEBUG
    //allocating the extensions with the extra debug extension
    extensions = (const char**)wlAlloc((glfwExtensionCount+1)*sizeof(const char*));
    extensions[glfwExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    *pCount = glfwExtensionCount + 1;
    #else //WL_DEBUG
    extensions = (const char**)wlAlloc(glfwExtensionCount*sizeof(const char*));
    *pCount = glfwExtensionCount;
    #endif

    for(uint16_t i=0; i<glfwExtensionCount; i++){
        extensions[i] = glfw_extensions[i];
    }

    

    return extensions; 
}
bool is_device_suitable(VkDevice device){
    
}

WLRenderer* wlCreateRenderer(){
    WLRenderer* renderer = (WLRenderer*)wlAlloc(sizeof(WLRenderer));
    //renderer->vulkan_instance

    //creating the VK instance//
    VkApplicationInfo app_info;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "hello triangle (:";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "WL Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    uint32_t extension_count = 0;
    const char** extensions = get_required_vulkan_extensions(&extension_count);

    VkInstanceCreateInfo vulkan_instance_info;
    vulkan_instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vulkan_instance_info.pApplicationInfo = &app_info;
    vulkan_instance_info.enabledExtensionCount = extension_count;
    vulkan_instance_info.ppEnabledExtensionNames = extensions;
    vulkan_instance_info.flags = 0;
    vulkan_instance_info.pNext = NULL;

    #ifdef WL_DEBUG
    const char* validation_layers[] = { "VK_LAYER_KHRONOS_validation" };
    vulkan_instance_info.enabledLayerCount = SIZE_OF_ARRAY(validation_layers);
    vulkan_instance_info.ppEnabledLayerNames = validation_layers;
    #else
    vulkan_instance_info.enabledLayerCount = 0;
    vulkan_instance_info.ppEnabledLayerNames = NULL;
    #endif

    VkResult create_result;
    create_result = vkCreateInstance(&vulkan_instance_info, NULL, &renderer->vulkan_instance);
    if(create_result!= VK_SUCCESS){
        WL_LOG(WL_FATAL, "failed to create VkInstance");
        return NULL;
    }
/////////////////////////////////////////////////////  

    // getting the number of valid GPUs
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(renderer->vulkan_instance, &device_count, NULL);

    if(device_count==0){
        WL_LOG(WL_FATAL, "no GPUs with vulkan support");
        return NULL;
    }

    // gettings the list of GPUs
    VkPhysicalDevice devices[device_count];
    vkEnumeratePhysicalDevices(renderer->vulkan_instance, &device_count, devices);

    for(uint32_t i=0; i<device_count; i++){
        if()
    }


    VkDeviceCreateInfo device_info;
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    VkResult device_result;
    device_result = vkCreateDevice(renderer->physical_device, );
    if(device_result != VK_SUCCESS){
        WL_LOG(WL_FATAL, "failed to create device");
    }

    return NULL;
}

void wlDestroyRenderer(WLRenderer* renderer){
    vkDestroyInstance(renderer->vulkan_instance, NULL);
}