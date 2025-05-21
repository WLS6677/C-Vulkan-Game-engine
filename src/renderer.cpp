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
