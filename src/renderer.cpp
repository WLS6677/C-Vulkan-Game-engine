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
typedef struct WLQueueFamilyIndices {
    uint32_t graphics_family;
    uint32_t present_family;
    uint32_t compute_family;
    uint32_t transfer_family;
} WLQueueFamilyIndices;

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
WLQueueFamilyIndices find_queue_families(VkPhysicalDevice device){
    WLQueueFamilyIndices family_indices;
    family_indices.graphics_family = UINT32_MAX;
    family_indices.present_family = UINT32_MAX;
    family_indices.compute_family = UINT32_MAX;
    family_indices.transfer_family = UINT32_MAX;

    // this is what the GPU supports
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);
    VkQueueFamilyProperties* family_properties = (VkQueueFamilyProperties*)wlAlloc(sizeof(VkQueueFamilyProperties)*queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, family_properties);

    printf("the gpu has: %u queues", queue_family_count);

    for(size_t i=0; i<queue_family_count; i++){
        if(family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
            family_indices.graphics_family = i;
        }
        
    }
}
bool is_device_suitable(VkPhysicalDevice device){
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(device, &device_properties);
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(device, &device_features);

    WLQueueFamilyIndices queue_indices = find_queue_families(device);
}


// debugger functions
static PFN_vkCreateDebugUtilsMessengerEXT fpCreateDebugUtilsMessengerEXT = NULL;
static PFN_vkDestroyDebugUtilsMessengerEXT fpDestroyDebugUtilsMessengerEXT = NULL;
typedef VkResult (VKAPI_PTR *PFN_vkCreateDebugUtilsMessengerEXT)(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pMessenger);


VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    (void)messageSeverity;
    (void)messageType;
    (void)pUserData;

    printf("validation layer: %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}
VkResult LoadDebugUtilsMessengerEXTFunctions(VkInstance instance) {
    fpCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    fpDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (fpCreateDebugUtilsMessengerEXT == NULL) {
        WL_LOG(WL_WARNING, "vulkan debug messenger extension not present");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
    return VK_SUCCESS;
}

WLRenderer* wlCreateRenderer(){
    WLRenderer* renderer = (WLRenderer*)wlAlloc(sizeof(WLRenderer));
    //renderer->vulkan_instance

    #ifdef WL_DEBUG

    VkDebugUtilsMessengerCreateInfoEXT debugger_info = {};
    debugger_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugger_info.pfnUserCallback = debug_callback; 
    debugger_info.pUserData = NULL;
    debugger_info.flags = 0;

    #endif //WL_DEBUG

////////////////////////////////////////////////////////
    //      creating the VK instance        //

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "hello triangle (:";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "WL Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    uint32_t extension_count = 0;
    const char** extensions = get_required_vulkan_extensions(&extension_count);

    VkInstanceCreateInfo vulkan_instance_info = {};
    vulkan_instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vulkan_instance_info.pApplicationInfo = &app_info;
    vulkan_instance_info.enabledExtensionCount = extension_count;
    vulkan_instance_info.ppEnabledExtensionNames = extensions;
    vulkan_instance_info.flags = 0;
    

    #ifdef WL_DEBUG
    const char* validation_layers[] = { "VK_LAYER_KHRONOS_validation" };
    vulkan_instance_info.enabledLayerCount = SIZE_OF_ARRAY(validation_layers);
    vulkan_instance_info.ppEnabledLayerNames = validation_layers;
    vulkan_instance_info.pNext = &debugger_info;
    #else //WL_DEBUG
    vulkan_instance_info.enabledLayerCount = 0;
    vulkan_instance_info.ppEnabledLayerNames = NULL;
    vulkan_instance_info.pNext = NULL;
    #endif //WL_DEBUG

    VkResult create_result;
    create_result = vkCreateInstance(&vulkan_instance_info, NULL, &renderer->vulkan_instance);
    if(create_result!= VK_SUCCESS){
        WL_LOG(WL_FATAL, "failed to create VkInstance");
        return NULL;
    }

/////////////////////////////////////////////////////
    //      debug messenger     //

    #ifdef WL_DEBUG
    LoadDebugUtilsMessengerEXTFunctions(renderer->vulkan_instance);

    VkResult debugger_result;
    debugger_result = fpCreateDebugUtilsMessengerEXT(renderer->vulkan_instance,&debugger_info, NULL, &renderer->debug_messenger);

    if(debugger_result != VK_SUCCESS){
        WL_LOG(WL_WARNING, "failed to create debug messenger");
    }
    #endif //WL_DEBUG

/////////////////////////////////////////////////////  
    //      creating the physicsal device   //

    // getting the number of valid GPUs
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(renderer->vulkan_instance, &device_count, NULL);

    if(device_count==0){
        WL_LOG(WL_FATAL, "no GPUs with vulkan support");
        return NULL;
    }

    // gettings the list of GPUs
    VkPhysicalDevice physical_devices[device_count];
    vkEnumeratePhysicalDevices(renderer->vulkan_instance, &device_count, physical_devices);

    // choosing the GPU
    renderer->physical_device = NULL;
    for(uint32_t i=0; i<device_count; i++){
        if(is_device_suitable(physical_devices[i])){
            renderer->physical_device = physical_devices[i];
        }
    }
    if(renderer->physical_device==NULL){
        WL_LOG(WL_FATAL, "no suitable gpu for vulkan");
        return NULL;
    }


    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    VkResult device_result;
    device_result = vkCreateDevice(renderer->physical_device, NULL, NULL, NULL);
    if(device_result != VK_SUCCESS){
        WL_LOG(WL_FATAL, "failed to create device");
    }

    return NULL;
}

void wlDestroyRenderer(WLRenderer* renderer){

    fpDestroyDebugUtilsMessengerEXT(renderer->vulkan_instance, renderer->debug_messenger, NULL);
    vkDestroyInstance(renderer->vulkan_instance, NULL);
}