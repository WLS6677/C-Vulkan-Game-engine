#include <renderer.h>
#include <vulkan/vulkan.h>
#include <glfw3.h>

struct WLSwapChain{
    VkSwapchainKHR swapchain;

    uint32_t image_count;
    // all of these arrays are size(image_count)
    VkImage* pImages;
    VkImageView* pImage_views;
    VkFramebuffer* pFramebuffers;
    

    VkSurfaceFormatKHR surface_format;
    VkPresentModeKHR present_mode;
    VkExtent2D extent;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    //semaphores and stuff for syncronization/Vsync
    VkSemaphore* pImage_available_semaphores;
    uint32_t pImage_available_semaphore_count;

    VkSemaphore* pRender_finished_semaphores;
    uint32_t Render_finished_semaphore_count;

    VkFence* pIn_flight_fences;
    uint32_t pIn_flight_fence_count;

    uint32_t currentFrame;
};

typedef struct {
    vec3f Pos, Color;
} WLVertex;
#define WL_VERTEX_ATTRIBUTE_COUNT 2

#define WL_DEBUG

typedef struct WLPipeline {
    VkPipeline pipeline;

    // vertex data attibute description
    VkVertexInputAttributeDescription* attribute_descs;
    uint32_t attribute_desc_count;

    // shaders
    const char* vertex_shader;
    const char* fragment_shader;
    
    // settings
    VkPrimitiveTopology topology;
    VkCullModeFlags cull_mode;
    VkPolygonMode polygon_mode;
} WLPipeline;
typedef struct WLRenderPipelineLayout{
    // describes the how and what data is passed to the shaders in the pipeline
    VkPipelineLayout layout;

    // the render pass is the order and info about how drawing a frame happens
    VkRenderPass render_pass;

    // the pipelines are the code basically and they are tied to subpasses
    WLPipeline* pPipelines;
    uint32_t pipeline_count;

    VkDescriptorSetLayout* pDescriptor_set_layouts;
    uint32_t descriptor_set_layout_count;
    VkDescriptorPool descriptor_pool;

    //uniform buffers
    VkBuffer* pUniform_buffers;
    VkDeviceMemory* pUniform_buffers_memory; 
    uint32_t uniform_buffer_count;

    void** ppUniformBuffersMapped;
} WLRenderPipelineLayout;
typedef struct {
    VkVertexInputBindingDescription bind_description;
    VkVertexInputAttributeDescription pAttribute_descriptions[WL_VERTEX_ATTRIBUTE_COUNT];
} WLVertexInfo;
static const WLVertexInfo WL_VERTEX_INFO = {
    .bind_description = {
        .binding = 0,
        .stride = sizeof(WLVertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    },
    .pAttribute_descriptions = {
        /*position*/{
        0,  // location
        0,  // binding
        VK_FORMAT_R32G32B32_SFLOAT,
        0,  // offset inside vertex
        },
        /*Color*/{
        1,  // location
        0,  // binding
        VK_FORMAT_R32G32B32_SFLOAT,
        sizeof(vec3f),  // offset inside vertex
        }
    }   
};
typedef struct WLVertexBuffer {
    
    VkBuffer buffer;
    VkDeviceMemory memory;
} WLVertexBuffer;
typedef struct WLIndexBuffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
} WLIndexBuffer;
typedef struct WLRenderObject {
    WLPipeline* pPipeline;
    WLVertexBuffer vertex_buffer;
    WLIndexBuffer index_buffer;
} WLRenderObject;
struct WLRenderer{

    VkInstance vulkan_instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkDebugUtilsMessengerEXT debug_messenger;
    // SwapChain
    VkSurfaceKHR surface;
    WLSwapChain swap_chain;
    // queueueueueueueueueueueueueueues
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue compute_queue;
    VkQueue transfer_queue;

    VkCommandPool graphicsCommandPool;
    VkCommandPool transferCommandPool;
    VkCommandBuffer* pCommandBuffers;

    // pipelines
    WLRenderPipelineLayout basic_pipeline_layout;
    bool pipeline_layout_exists;
    WLPipeline simple_graphics_pipeline;
};

static WLRenderer renderer = {};

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
    VkSurfaceFormatKHR* pFormats;
    uint32_t format_count;
    VkPresentModeKHR* pPresent_modes;
    uint32_t present_mode_count;
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

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.format_count, NULL);
    details.pFormats = (VkSurfaceFormatKHR*)wlAlloc(details.format_count*sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.format_count, details.pFormats);

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.present_mode_count, NULL);
    details.pPresent_modes = (VkPresentModeKHR*)wlAlloc(details.present_mode_count*sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.present_mode_count, details.pPresent_modes);
    

    #ifdef WL_DEBUG 
    printf("avaialable format count: %u     available present count:%u\n",details.format_count ,details.present_mode_count);
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
    if(swapchain_details.pFormats==NULL || swapchain_details.pPresent_modes==NULL){
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

void wlCreateRenderer(void* window_handle){

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
    create_result = vkCreateInstance(&vulkan_instance_info, NULL, &renderer.vulkan_instance);
    if(create_result!= VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create VkInstance");
        return;
    }

/////////////////////////////////////////////////////
    //      debug messenger     //

    #ifdef WL_DEBUG
    WL_LOG(WL_LOG_TRACE,"creating debug messenger...");
    LoadDebugUtilsMessengerEXTFunctions(renderer.vulkan_instance);

    VkResult debugger_result;
    debugger_result = fpCreateDebugUtilsMessengerEXT(renderer.vulkan_instance,&debugger_info, NULL, &renderer.debug_messenger);

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
        return;
    }
    surface_result = glfwCreateWindowSurface(renderer.vulkan_instance, (GLFWwindow*)window_handle, NULL, &renderer.surface);
    if(surface_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create surfaceKHR");
        return;
    }
    WL_LOG(WL_LOG_TRACE, "surfaceKHR created successfully!");



/////////////////////////////////////////////////////  
    //      choosing the physicsal device   //

    WL_LOG(WL_LOG_TRACE,"gettings valid GPUs...");
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(renderer.vulkan_instance, &device_count, NULL);

    if(device_count==0){
        WL_LOG(WL_LOG_FATAL, "no GPUs with vulkan support");
        return;
    }
    printf("found %u GPUs with vulkan support", device_count);


    // gettings the list of GPUs
    VkPhysicalDevice physical_devices[device_count];
    vkEnumeratePhysicalDevices(renderer.vulkan_instance, &device_count, physical_devices);

    WL_LOG(WL_LOG_TRACE,"choosing GPU...");
    renderer.physical_device = VK_NULL_HANDLE;
    for(uint32_t i=0; i<device_count; i++){
        if(is_device_suitable(physical_devices[i], renderer.surface)){
            renderer.physical_device = physical_devices[i];
            break;
        }
    }
    if(renderer.physical_device==VK_NULL_HANDLE){
        WL_LOG(WL_LOG_FATAL, "no suitable gpu for vulkan");
        return;
    }
    WL_LOG(WL_LOG_TRACE,"successfully! found a suitable GPU!");

/////////////////////////////////////////////////////////////
        //      creating logical device         //

    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // getting the queue indices
    WLQueueFamilyIndices family_indices;
    family_indices = find_queue_families(renderer.physical_device, renderer.surface);

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
    device_result = vkCreateDevice(renderer.physical_device, &device_info, NULL, &renderer.device);
    if(device_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create logical device");
        return;
    }
    WL_LOG(WL_LOG_TRACE,"logical device created successfully!");

    vkGetDeviceQueue(renderer.device, family_indices.graphics_family, 0, &renderer.graphics_queue);
    vkGetDeviceQueue(renderer.device, family_indices.present_family, 0, &renderer.present_queue);
    vkGetDeviceQueue(renderer.device, family_indices.compute_family, 0, &renderer.compute_queue);
    vkGetDeviceQueue(renderer.device, family_indices.transfer_family, 0, &renderer.transfer_queue);

    wlCreateSwapChain(window_handle);
    wlCreateRasterizedRenderPipelineLayout();
    wlCreateBasicPipeLine();

    return;
}

void wlCreateSwapChain(void* window_handle){

    WLSwapChain swapchain;
    
 ///////////////////////////////////////////////////
            //      swapchain       //

    WLSwapChainSupportDetails swapchain_support = query_swap_chain_support_details(renderer.physical_device, renderer.surface);

    WL_LOG(WL_LOG_TRACE, "choosing format");
    // choosing surface format
    if(swapchain_support.format_count == 0){
        WL_LOG(WL_LOG_FATAL, "no available surface formats");
    }
    VkSurfaceFormatKHR surface_format = swapchain_support.pFormats[0];
    printf("Desired Format %d, ColorSpace %d\n", VK_FORMAT_B8G8R8A8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR);
    for (size_t i = 0; i < swapchain_support.format_count; i++)
    {   
        printf("checking the surface with: Format %d, ColorSpace %d\n", swapchain_support.pFormats[i].format, swapchain_support.pFormats[i].colorSpace);
        if(
            swapchain_support.pFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            swapchain_support.pFormats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR
        ){
            surface_format = swapchain_support.pFormats[i];
            break;
        }
    }
    swapchain.surface_format = surface_format;
    
    #ifdef WL_DEBUG
    printf("Format %d, ColorSpace %d\n", swapchain.surface_format, swapchain.surface_format.colorSpace);
    #endif

    // choosing present mode
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (size_t i = 0; i < swapchain_support.present_mode_count; i++){
        if(
            swapchain_support.pPresent_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR
        ){
            present_mode = swapchain_support.pPresent_modes[i];
        }
    }
    swapchain.present_mode = present_mode;

    // getting image extent
    VkExtent2D extent;
    if(swapchain_support.capabilities.currentExtent.width != UINT32_MAX){
        extent = swapchain_support.capabilities.currentExtent;
    } else{
        int extent_width, extent_height;
        glfwGetFramebufferSize((GLFWwindow*)window_handle, &extent_width, &extent_height);

        VkExtent2D new_extent = {
            (uint32_t)extent_width,
            (uint32_t)extent_height
        };

        //TODO: clamp the width and height to the capablity max and min

        extent = new_extent;
    }
    swapchain.extent = extent;
    
    swapchain.image_count = swapchain_support.capabilities.minImageCount + 1;
    // check if we exceeded the max image limit
    if (swapchain_support.capabilities.maxImageCount > 0 && swapchain.image_count > swapchain_support.capabilities.maxImageCount) {
	    swapchain.image_count = swapchain_support.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR swapchain_info = {};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = renderer.surface;
    swapchain_info.minImageCount = swapchain.image_count;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageFormat = swapchain.surface_format.format;
    swapchain_info.imageColorSpace = swapchain.surface_format.colorSpace;
    swapchain_info.imageExtent = swapchain.extent;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    WLQueueFamilyIndices family_indices = find_queue_families(renderer.physical_device, renderer.surface);
    uint32_t queue_family_indecies[] = { family_indices.graphics_family, family_indices.present_family };

    if (family_indices.graphics_family != family_indices.present_family) {
	swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	swapchain_info.queueFamilyIndexCount = 2;
	swapchain_info.pQueueFamilyIndices = queue_family_indecies;
    } else{
	swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchain_info.queueFamilyIndexCount = 0;
	swapchain_info.pQueueFamilyIndices = NULL;
    }

    swapchain_info.preTransform = swapchain_support.capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swapchain_info.presentMode = swapchain.present_mode;
    swapchain_info.clipped = VK_TRUE;

    swapchain_info.oldSwapchain = VK_NULL_HANDLE;

    WL_LOG(WL_LOG_TRACE, "creating swapchain ...");
    VkResult swapchain_result;
    swapchain_result = vkCreateSwapchainKHR(renderer.device, &swapchain_info, NULL, &swapchain.swapchain);
    if(swapchain_result != VK_SUCCESS){
        printf("error code: %d\n", swapchain_result);
        WL_LOG(WL_LOG_FATAL, "failed to create swapchain");
        return;
    }
    WL_LOG(WL_LOG_TRACE, "swapchain created successfully!");

    // getting the image handles.
    vkGetSwapchainImagesKHR(renderer.device, swapchain.swapchain, &swapchain.image_count, NULL);
    swapchain.pImages = (VkImage*)wlAlloc(sizeof(VkImage) * swapchain.image_count);
    vkGetSwapchainImagesKHR(renderer.device, swapchain.swapchain, &swapchain.image_count, swapchain.pImages);

 //////////////////////////////////////////////////////////
            //      swapchain image views      //

    WL_LOG(WL_LOG_TRACE, "creating image views ...");

    swapchain.pImage_views = (VkImageView*)wlAlloc(swapchain.image_count*sizeof(VkImageView));
    for (size_t i = 0; i < swapchain.image_count; i++){
        VkImageViewCreateInfo image_view_info = {};
        image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_info.image = swapchain.pImages[i];

        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format = swapchain.surface_format.format;

        image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.baseMipLevel = 0;
        image_view_info.subresourceRange.layerCount = 1;
        image_view_info.subresourceRange.levelCount = 1;

        
        
        //WL_LOG(WL_LOG_TRACE, "creating image view ...");  
        VkResult image_view_result;
        image_view_result = vkCreateImageView(renderer.device, &image_view_info, nullptr, &swapchain.pImage_views[i]);
        if(image_view_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create image view");
        return;
        }
        //WL_LOG(WL_LOG_TRACE, "image view created successfully!");
    }
    
 //////////////////////////////////////////////////////////
            //      swapchain buffers      //

    if(!renderer.pipeline_layout_exists){
        WL_LOG(WL_LOG_WARNING, "render layout not created, no frame buffers for you");
        renderer.swap_chain = swapchain;
        return;
    }

    WL_LOG(WL_LOG_TRACE, "creating swap chain buffers ...");

    swapchain.pFramebuffers = (VkFramebuffer*)wlAlloc(swapchain.image_count*sizeof(VkFramebuffer));
    for (size_t i = 0; i < swapchain.image_count; i++){
        VkImageView attachments[] = {
	        swapchain.pImage_views[i]
	        // other attachments
        };

        VkFramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = renderer.basic_pipeline_layout.render_pass;
        framebuffer_info.height = swapchain.extent.height;
        framebuffer_info.width = swapchain.extent.width;
        framebuffer_info.attachmentCount = SIZE_OF_ARRAY(attachments);
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.layers = 1;

        VkResult frame_buffer_result;
        frame_buffer_result = vkCreateFramebuffer(renderer.device, &framebuffer_info, nullptr, &swapchain.pFramebuffers[i]);
        if(frame_buffer_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create frame buffer");
        return;
        }
    }
}
void wlReCreateSwapChain(void* window_handle);
void wlDestroySwapChain();

void wlCreateRasterizedRenderPipelineLayout(){
    WLRenderPipelineLayout pipeline_layout = {};

 //////////////////////////////////////////////////////////////
             //       render pass        //

    #ifdef WL_DEBUG
    printf("suface format: %d, surface colour space:%d\n", renderer.swap_chain.surface_format.format, renderer.swap_chain.surface_format.colorSpace);
    #endif
          
    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    // colour attachments
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = renderer.swap_chain.surface_format.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0; // index of the attachment in the attachment array
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;

    // sub passes
    VkSubpassDescription sub_pass_decription = {};
    sub_pass_decription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    sub_pass_decription.colorAttachmentCount = 1;
    sub_pass_decription.pColorAttachments = &color_attachment_ref;
    
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &sub_pass_decription;
 
    // dependancies
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    // creating the render pass
   
    WL_LOG(WL_LOG_TRACE, "creating renderpass..");
    VkResult render_pass_result;
    render_pass_result = vkCreateRenderPass(renderer.device, &render_pass_info, NULL, &pipeline_layout.render_pass);
    if(render_pass_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create renderpass");
        return;
    }
    WL_LOG(WL_LOG_TRACE, "render pass created successfully!");

 ////////////////////////////////////////////////////////////
            //        pipeline layout          //

    VkPipelineLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    // getting the descriptor set data
    pipeline_layout.pDescriptor_set_layouts = VK_NULL_HANDLE;
    pipeline_layout.descriptor_set_layout_count = 0;
    // descriptor sets
    layout_info.pSetLayouts = pipeline_layout.pDescriptor_set_layouts;
    layout_info.setLayoutCount = pipeline_layout.descriptor_set_layout_count;
    // push constants
    layout_info.pPushConstantRanges = VK_NULL_HANDLE;
    layout_info.pushConstantRangeCount = 0;

    WL_LOG(WL_LOG_TRACE, "creating pipeline layout...");
    VkResult layout_result;
    layout_result = vkCreatePipelineLayout(renderer.device, &layout_info, NULL, &pipeline_layout.layout);
    if(layout_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create pipeline layout");
        return;
    }

    renderer.basic_pipeline_layout = pipeline_layout;

    WL_LOG(WL_LOG_TRACE, "pipeline layout created successfully!");

}
void wlDestroyRasterizedRenderPipelineLayout(){
    vkDestroyPipelineLayout(renderer.device ,renderer.basic_pipeline_layout.layout, NULL);
}

void wlCreateBasicPipeLine(){
    WLPipeline pipeline;

 ///////////////////////////////////////////////////
            //      shaders      //         

    uint32_t vertex_code_size;
    uint8_t* vertex_shader_code = wlReadFile("shaders/basic_vert.spv", &vertex_code_size);
    uint32_t fragment_code_size;
    uint8_t* fragment_shader_code = wlReadFile("shaders/basic_frag.spv", &fragment_code_size);

    VkShaderModule vertex_shader_module;
    VkShaderModule fragment_shader_module;

    //vertex shader
    VkShaderModuleCreateInfo vertex_shader_create_info = {};
    vertex_shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vertex_shader_create_info.codeSize = vertex_code_size;
    vertex_shader_create_info.pCode = (const uint32_t*)(vertex_shader_code);

    WL_LOG(WL_LOG_TRACE, "creating vertex shader ...");
    VkResult vertex_result;
    vertex_result = vkCreateShaderModule(renderer.device, &vertex_shader_create_info, NULL, &vertex_shader_module);
    if(vertex_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create vertex shader");
        return;
    }
    WL_LOG(WL_LOG_TRACE, "vertex shader created successfully!");

    //fragment shader
    VkShaderModuleCreateInfo fragment_shader_create_info = {};
    fragment_shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fragment_shader_create_info.codeSize = fragment_code_size;
    fragment_shader_create_info.pCode = (const uint32_t*)(fragment_shader_code);

    WL_LOG(WL_LOG_TRACE, "creating fragment shader ...");
    VkResult fragment_result;
    fragment_result = vkCreateShaderModule(renderer.device, &fragment_shader_create_info, NULL, &fragment_shader_module);
    if(fragment_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create fragment shader");
        return;
    }
    WL_LOG(WL_LOG_TRACE, "fragment shader created successfully!");

    VkPipelineShaderStageCreateInfo vertex_shader_stage_info = {};
    vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_stage_info.module = vertex_shader_module;
    vertex_shader_stage_info.pName = "main";
    VkPipelineShaderStageCreateInfo fragment_shader_stage_info = {};
    fragment_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_stage_info.module = fragment_shader_module;
    fragment_shader_stage_info.pName = "main";
    VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_stage_info, fragment_shader_stage_info };

 /////////////////////////////////////////////////////////////////////
            //      fixed piplestage settings       //

    
    VkDynamicState pDynamic_states[] = {
	    VK_DYNAMIC_STATE_VIEWPORT,
	    VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamic_state_info = {};
    dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = SIZE_OF_ARRAY(pDynamic_states);
    dynamic_state_info.pDynamicStates = pDynamic_states;

    VkPipelineViewportStateCreateInfo view_port_info = {};
    view_port_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    view_port_info.viewportCount = 1;
    view_port_info.scissorCount = 1;


    // Anti Aliassing
    VkPipelineMultisampleStateCreateInfo multi_sampling_info = {};
    multi_sampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multi_sampling_info.sampleShadingEnable = VK_FALSE;
    multi_sampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multi_sampling_info.minSampleShading = 1.0f; // Optional
    multi_sampling_info.pSampleMask = NULL; // Optional
    multi_sampling_info.alphaToCoverageEnable = VK_FALSE; // Optional
    multi_sampling_info.alphaToOneEnable = VK_FALSE; // Optional

    // Rasterization Stage Stuff
    VkPipelineRasterizationStateCreateInfo rasterization_state_info = {};
    rasterization_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state_info.depthClampEnable = VK_FALSE;
    rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state_info.lineWidth = 1.0f;
    rasterization_state_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_state_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterization_state_info.depthBiasEnable = VK_FALSE;
    rasterization_state_info.depthBiasConstantFactor = 0.0f; // Optional
    rasterization_state_info.depthBiasClamp = 0.0f; // Optional
    rasterization_state_info.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
    
    // fragement color blending
    VkPipelineColorBlendStateCreateInfo color_blending_info = {};
    color_blending_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending_info.logicOpEnable = VK_FALSE;
    color_blending_info.logicOp = VK_LOGIC_OP_COPY; // Optional
    color_blending_info.attachmentCount = 1;
    color_blending_info.pAttachments = &color_blend_attachment;
    color_blending_info.blendConstants[0] = 0.0f; // Optional
    color_blending_info.blendConstants[1] = 0.0f; // Optional
    color_blending_info.blendConstants[2] = 0.0f; // Optional
    color_blending_info.blendConstants[3] = 0.0f; // Optional

 /////////////////////////////////////////////////////////////////////
             //       creating the pipeline        //

    // input structure

    VkPipelineVertexInputStateCreateInfo vertex_input_state_info = {};
    vertex_input_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertex_input_state_info.vertexAttributeDescriptionCount = WL_VERTEX_ATTRIBUTE_COUNT;
    vertex_input_state_info.pVertexAttributeDescriptions = WL_VERTEX_INFO.pAttribute_descriptions;

    vertex_input_state_info.vertexBindingDescriptionCount = 1;
    vertex_input_state_info.pVertexBindingDescriptions = &WL_VERTEX_INFO.bind_description;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
    input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;	// 0,1,2 is a trianlge, 3,4,5 is another triangle, 6,7,8 is another triangle
    input_assembly_info.primitiveRestartEnable = VK_FALSE;	
    
    // PIPELINE CREATION

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = SIZE_OF_ARRAY(shader_stages);
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_state_info;
    pipeline_info.pInputAssemblyState = &input_assembly_info;
    pipeline_info.pViewportState = &view_port_info;
    pipeline_info.pRasterizationState = &rasterization_state_info;
    pipeline_info.pMultisampleState = &multi_sampling_info;
    pipeline_info.pDepthStencilState = NULL;
    pipeline_info.pColorBlendState = &color_blending_info;
    pipeline_info.pDynamicState = &dynamic_state_info;
    pipeline_info.layout = renderer.basic_pipeline_layout.layout;
    pipeline_info.renderPass = renderer.basic_pipeline_layout.render_pass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipeline_info.basePipelineIndex = -1; // Optional


    WL_LOG(WL_LOG_TRACE, "creating simple graphics pipeline ...");
    VkResult layout_result;
    layout_result = vkCreateGraphicsPipelines(renderer.device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &pipeline.pipeline);;
    if(layout_result != VK_SUCCESS){
        WL_LOG(WL_LOG_FATAL, "failed to create simple graphics pipeline");
        return;
    }
    WL_LOG(WL_LOG_TRACE, "simple graphics pipeline created successfully!");

    renderer.simple_graphics_pipeline = pipeline;
}
void wlDestroyBasicPipeline(){
    
}

void wlDestroyRenderer(){
    wlDestroyRasterizedRenderPipelineLayout();
    vkDestroyDevice(renderer.device, NULL);
    vkDestroySurfaceKHR(renderer.vulkan_instance, renderer.surface, NULL);
    fpDestroyDebugUtilsMessengerEXT(renderer.vulkan_instance, renderer.debug_messenger, NULL);
    vkDestroyInstance(renderer.vulkan_instance, NULL);
}