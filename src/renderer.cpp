#include <renderer.h>
#include <vulkan/vulkan.h>
#include <glfw3.h>

struct WLRenderer{

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
    VkQueue compute_queue;
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
typedef struct WLQueueFamilyIndices{
    uint32_t graphics_family;
    uint32_t present_family;
    uint32_t compute_family;
    uint32_t transfer_family;
    bool are_complete;
    uint32_t family_count = 4;
} WLQueueFamilyIndices;
typedef struct WLSwapChainSupportDetails{
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR* formats;
    VkPresentModeKHR* present_modes;
} WLSwapChainSupportDetails;

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
WLQueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface){
    WLQueueFamilyIndices family_indices;
    family_indices.graphics_family = UINT32_MAX;
    family_indices.present_family = UINT32_MAX;
    family_indices.compute_family = UINT32_MAX;
    family_indices.transfer_family = UINT32_MAX;
    family_indices.are_complete = false;

    // this is what the GPU supports
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);
    VkQueueFamilyProperties* family_properties = (VkQueueFamilyProperties*)wlAlloc(sizeof(VkQueueFamilyProperties)*queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, family_properties);

    #ifdef WL_DEBUG
    printf("the gpu has: %u queues\n", queue_family_count);
    #endif

    for(size_t i=0; i<queue_family_count; i++){
        if( family_indices.graphics_family==UINT32_MAX && family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ){
            family_indices.graphics_family = i;
        }
        if( family_indices.transfer_family==UINT32_MAX && family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT){
            family_indices.transfer_family = i;
        }
        if( family_indices.compute_family==UINT32_MAX && family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT){
            family_indices.compute_family = i;
        }
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if(presentSupport){ 
        family_indices.present_family = i;
        }           
    }

    if( family_indices.graphics_family!=UINT32_MAX &&
        family_indices.present_family!=UINT32_MAX &&
        family_indices.compute_family!=UINT32_MAX &&
        family_indices.transfer_family!=UINT32_MAX ){
        family_indices.are_complete = true;
    }

    return family_indices;
}
bool check_device_extensions_support(VkPhysicalDevice device){
    return true;
}
WLSwapChainSupportDetails query_swap_chain_support_details(VkPhysicalDevice device, VkSurfaceKHR surface){
    WLSwapChainSupportDetails details = {};
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, NULL);
    details.formats = (VkSurfaceFormatKHR*)wlAlloc(format_count*sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats);

    uint32_t present_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_count, NULL);
    details.present_modes = (VkPresentModeKHR*)wlAlloc(format_count*sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_count, details.present_modes);
    

    #ifdef WL_DEBUG 
    printf("avaialable format count: %u     available present count:%u\n",format_count ,present_count);
    #endif

    return details;
}
bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface){

    //checking for device properties
    VkPhysicalDeviceProperties device_properties = {};
    vkGetPhysicalDeviceProperties(device, &device_properties);
    if(device_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
        WL_LOG(WL_LOG_WARNING, "GPU isnt discrete");
        return false;
    }

    //checking for family queues availablity
    WLQueueFamilyIndices queue_indices = find_queue_families(device, surface);
    if(!queue_indices.are_complete){
        WL_LOG(WL_LOG_WARNING, "GPU doesnt support required family queues");
        return false;
    }

    //checking for device extension support
    if(!check_device_extensions_support(device)){
        WL_LOG(WL_LOG_WARNING,"GPU doesnt support required extensions");
        return false;
    }

    //checking for swapchain support
    WLSwapChainSupportDetails swapchain_details = query_swap_chain_support_details(device, surface);
    if(swapchain_details.formats==NULL || swapchain_details.present_modes==NULL){
        WL_LOG(WL_LOG_WARNING, "GPU doesnt support swapcahin present formats or mods");
        return false;
    }

    return true;
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
        WL_LOG(WL_LOG_WARNING, "vulkan debug messenger extension not present");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
    return VK_SUCCESS;
}

///////////////////////////////////////////////
        // API FUNCTIONS //

WLRenderer* wlCreateRenderer(void* window_handle){
    WLRenderer* renderer = (WLRenderer*)wlAlloc(sizeof(WLRenderer));

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

    WL_LOG(WL_LOG_TRACE,"creating vulkan instance...");

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
        WL_LOG(WL_LOG_FATAL, "failed to create VkInstance");
        return NULL;
    }

/////////////////////////////////////////////////////
    //      debug messenger     //

    #ifdef WL_DEBUG
    WL_LOG(WL_LOG_TRACE,"creating debug messenger...");
    LoadDebugUtilsMessengerEXTFunctions(renderer->vulkan_instance);

    VkResult debugger_result;
    debugger_result = fpCreateDebugUtilsMessengerEXT(renderer->vulkan_instance,&debugger_info, NULL, &renderer->debug_messenger);

    if(debugger_result != VK_SUCCESS){
        WL_LOG(WL_LOG_WARNING, "failed to create debug messenger");
    }
    WL_LOG(WL_LOG_TRACE,"debug messenger created successfully!");
    #endif //WL_DEBUG

/////////////////////////////////////////////
    //      creating surface        //

    WL_LOG(WL_LOG_TRACE, "creating surfaceKHR...");
    VkResult surface_result;
    if(window_handle==NULL){
        WL_LOG(WL_LOG_FATAL, "window handle is NULL");
        return NULL;
    }
    surface_result = glfwCreateWindowSurface(renderer->vulkan_instance, (GLFWwindow*)window_handle, NULL, &renderer->surface);
    if(surface_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create surfaceKHR");
        return NULL;
    }
    WL_LOG(WL_LOG_TRACE, "surfaceKHR created successfully!");



/////////////////////////////////////////////////////  
    //      choosing the physicsal device   //

    WL_LOG(WL_LOG_TRACE,"gettings valid GPUs...");
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(renderer->vulkan_instance, &device_count, NULL);

    if(device_count==0){
        WL_LOG(WL_LOG_FATAL, "no GPUs with vulkan support");
        return NULL;
    }
    printf("found %u GPUs with vulkan support", device_count);


    // gettings the list of GPUs
    VkPhysicalDevice physical_devices[device_count];
    vkEnumeratePhysicalDevices(renderer->vulkan_instance, &device_count, physical_devices);

    WL_LOG(WL_LOG_TRACE,"choosing GPU...");
    renderer->physical_device = VK_NULL_HANDLE;
    for(uint32_t i=0; i<device_count; i++){
        if(is_device_suitable(physical_devices[i], renderer->surface)){
            renderer->physical_device = physical_devices[i];
            break;
        }
    }
    if(renderer->physical_device==VK_NULL_HANDLE){
        WL_LOG(WL_LOG_FATAL, "no suitable gpu for vulkan");
        return NULL;
    }
    WL_LOG(WL_LOG_TRACE,"successfully! found a suitable GPU!");

/////////////////////////////////////////////////////////////
        //      creating logical device         //

    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // getting the queue indices
    WLQueueFamilyIndices family_indices;
    family_indices = find_queue_families(renderer->physical_device, renderer->surface);

    // putting the indices into an array
    uint32_t* family_indices_array = (uint32_t*)wlAlloc(family_indices.family_count*sizeof(uint32_t));
    family_indices_array[0] = family_indices.graphics_family;
    family_indices_array[1] = family_indices.present_family;
    family_indices_array[2] = family_indices.compute_family;
    family_indices_array[3] = family_indices.transfer_family;

    // making the queue infos
    uint32_t* used_indices = (uint32_t*)wlAlloc(family_indices.family_count*sizeof(uint32_t));
    for(size_t i=0; i < family_indices.family_count; i++){
            used_indices[i] = UINT32_MAX;
    }
    const float queue_priority = 1.0f;
    uint32_t unique_family_count = family_indices.family_count;
    uint32_t duplicate_offset = 0;
    bool there_is_duplicate = false;

    VkDeviceQueueCreateInfo* queue_create_infos = (VkDeviceQueueCreateInfo*)wlAlloc(family_indices.family_count*sizeof(VkDeviceQueueCreateInfo));
    for (size_t i=0; i < family_indices.family_count; i++){
        bool is_duplicate = false;
        for(size_t j=0; j < family_indices.family_count; j++){
            if(family_indices_array[i] == used_indices[j]){
                there_is_duplicate=true;
                break;
            }
        }
        if(there_is_duplicate){
            unique_family_count--;
            duplicate_offset++;
            there_is_duplicate=false;
            continue;
        }
        used_indices[i] = family_indices_array[i];

        queue_create_infos[i-duplicate_offset].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i-duplicate_offset].queueFamilyIndex = family_indices_array[i];
        queue_create_infos[i-duplicate_offset].queueCount = 1;
        queue_create_infos[i-duplicate_offset].pQueuePriorities = &queue_priority;
    }

    // putting the queue info into the device create info
    device_info.queueCreateInfoCount = unique_family_count;
    device_info.pQueueCreateInfos = queue_create_infos;

    // idk what this is
    VkPhysicalDeviceFeatures deviceFeatures = {};
    device_info.pEnabledFeatures = &deviceFeatures;

    // device extensions
    char* device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    device_info.enabledExtensionCount = SIZE_OF_ARRAY(device_extensions);
    device_info.ppEnabledExtensionNames = device_extensions;

    // validation layers
    #ifdef WL_DEBUG
    device_info.enabledLayerCount = SIZE_OF_ARRAY(validation_layers);
    device_info.ppEnabledLayerNames = validation_layers;
    #else
    device_info.enabledLayerCount = 0;
    #endif

    WL_LOG(WL_LOG_TRACE,"creating logical device...");
    VkResult device_result;
    device_result = vkCreateDevice(renderer->physical_device, &device_info, NULL, &renderer->device);
    if(device_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create logical device");
        return NULL;
    }
    WL_LOG(WL_LOG_TRACE,"logical device created successfully!");

    vkGetDeviceQueue(renderer->device, family_indices.graphics_family, 0, &renderer->graphics_queue);
    vkGetDeviceQueue(renderer->device, family_indices.present_family, 0, &renderer->present_queue);
    vkGetDeviceQueue(renderer->device, family_indices.compute_family, 0, &renderer->compute_queue);
    vkGetDeviceQueue(renderer->device, family_indices.transfer_family, 0, &renderer->transfer_queue);

    return renderer;
}
void wlDestroyRenderer(WLRenderer* renderer){
    vkDestroyDevice(renderer->device, NULL);
    vkDestroySurfaceKHR(renderer->vulkan_instance, renderer->surface, NULL);
    fpDestroyDebugUtilsMessengerEXT(renderer->vulkan_instance, renderer->debug_messenger, NULL);
    vkDestroyInstance(renderer->vulkan_instance, NULL);
}