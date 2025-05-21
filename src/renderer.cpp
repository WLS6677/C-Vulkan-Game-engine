#include <renderer.h>

struct WLRenderer {

    GLFWwindow* pWindow;
    VkInstance vulkan_instance;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
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

WLRenderer* wlCreateRenderer(){
    WLRenderer* renderer = (WLRenderer*)malloc(sizeof(WLRenderer));
    //renderer->vulkan_instance

    //creating the VK instance//
    VkApplicationInfo app_info;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "hello triangle (:";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;
    VkInstanceCreateInfo vulkan_instance_info;
    vulkan_instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vulkan_instance_info.pApplicationInfo;
    vulkan_instance_info.enabledExtensionCount;
    vulkan_instance_info.ppEnabledExtensionNames;
    vulkan_instance_info.flags;
    vulkan_instance_info.enabledLayerCount;
    vulkan_instance_info.ppEnabledLayerNames;
    //vkCreateInstance();

    return NULL;
}

void wlDestroyRenderer(WLRenderer* renderer){
    free(renderer);
}