#ifndef ENGINE_H
#define ENGINE_H

#include "Constants.h"

DEF CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) -> VkResult;
DEF DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) -> void;

class Engine {
public:
    Engine();

    DEF run() -> void;

private:
    DEF initWindow() -> void;
    DEF initVulkan() -> void;
    DEF mainLoop() -> void;
    DEF cleanup() -> void;

    DEF createInstance() -> void;
    DEF setupDebugMessenger() -> void;
    DEF createSurface() -> void;
    DEF pickPhysicalDevice() -> void;
    DEF isDeviceSuitable(VkPhysicalDevice device) -> bool;
    DEF findQueueFamilies(VkPhysicalDevice device) -> QueueFamilyIndices;
    DEF querySwapChainSupport(VkPhysicalDevice device) -> SwapChainSupportDetails;
    DEF createLogicalDevice() -> void;
    DEF createSwapChain() -> void;
    DEF createRenderPass() -> void;
    DEF createDescriptorSetLayout() -> void;
    DEF createGraphicsPipeline() -> void;
    DEF createFramebuffers() -> void;
    DEF createCommandPool() -> void;
    DEF createSyncObjects() -> void;
    DEF createVertexBuffer() -> void;
    DEF createIndexBuffer() -> void;
    DEF createUniformBuffers() -> void;
    DEF createDescriptorPool() -> void;
    DEF createDescriptorSets() -> void;
    DEF generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) -> void;
    DEF createTextureImage() -> void;
    DEF createTextureImageView() -> void;
    DEF createTextureSampler() -> void;
    DEF createColorResources() -> void;
    DEF getMaxUsableSampleCount() -> VkSampleCountFlagBits;
    DEF createCommandBuffers() -> void;
    DEF createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) -> void;
    DEF copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) -> void;
    DEF copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) -> void;
    DEF transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) -> void;
    DEF recreateSwapChain() -> void;
    DEF cleanupSwapChain() -> void;
    DEF drawFrame() -> void;
    DEF updateUniformBuffer(uint32_t currentImage) -> void;
    DEF beginSingleTimeCommands() -> VkCommandBuffer;
    DEF endSingleTimeCommands(VkCommandBuffer commandBuffer) -> void;
    DEF createImageViews() -> void;
    DEF createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) -> VkImageView;
    DEF createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory) -> void;
    DEF createShaderModule(const vector<char> &code) -> VkShaderModule;
    DEF findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) -> uint32_t;
    DEF chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) -> VkExtent2D;
    DEF recordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex) -> void;
    DEF findSupportedFormat(const vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) -> VkFormat;
    DEF createDepthResources() -> void;
    DEF findDepthFormat() -> VkFormat;
    DEF loadModel() -> void;

    static DEF getRequiredExtensions() -> vector<const char *>;
    static DEF checkValidationLayerSupport() -> bool;
    static DEF debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) -> VkBool32;
    static DEF populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) -> void;
    static DEF chooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR> &availableFormats) -> VkSurfaceFormatKHR;
    static DEF chooseSwapPresentMode(const vector<VkPresentModeKHR> &availablePresentModes) -> VkPresentModeKHR;
    static DEF checkDeviceExtensionSupport(VkPhysicalDevice device) -> bool;
    static DEF framebufferResizeCallback(GLFWwindow *window, int width, int height) -> void;
    static DEF validateExtensions(const vector<VkExtensionProperties> &supported_extensions, vector<const char *> required_extensions) -> bool;
    static DEF hasStencilComponent(VkFormat format) -> bool;

    GLFWwindow *m_Window;

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_Device;
    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;
    VkSurfaceKHR m_Surface;

    VkSwapchainKHR m_SwapChain;
    vector<VkImage> m_SwapChainImages;
    vector<VkImageView> m_SwapChainImageViews;
    VkFormat m_SwapChainImageFormat;
    VkExtent2D m_SwapChainExtent;
    vector<VkFramebuffer> m_SwapChainFramebuffers;

    VkRenderPass m_RenderPass;
    VkDescriptorSetLayout m_DescriptorSetLayout;
    VkPipelineLayout m_PipelineLayout;
    VkPipeline m_GraphicsPipeline;

    VkCommandPool m_CommandPool;
    vector<VkCommandBuffer> m_CommandBuffers;

    vector<VkSemaphore> m_ImageAvailableSemaphores;
    vector<VkSemaphore> m_RenderFinishedSemaphores;
    vector<VkFence> m_InFlightFences;

    uint32_t m_CurrentFrameIdx;
    bool m_FramebufferResized;

    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_VertexIndices;
    VkBuffer m_VertexBuffer;
    VkDeviceMemory m_VertexBufferMemory;
    VkBuffer m_IndexBuffer;
    VkDeviceMemory m_IndexBufferMemory;

    vector<VkBuffer> m_UniformBuffers;
    vector<VkDeviceMemory> m_UniformBuffersMemory;
    vector<void *> m_UniformBuffersMapped;

    VkDescriptorPool m_DescriptorPool;
    vector<VkDescriptorSet> m_DescriptorSets;

    uint32_t m_MipLevels;
    VkImage m_TextureImage;
    VkDeviceMemory m_TextureImageMemory;
    VkImageView m_TextureImageView;
    VkSampler m_TextureSampler;

    VkSampleCountFlagBits m_MSAASamples;

    VkImage m_ColorImage;
    VkDeviceMemory m_ColorImageMemory;
    VkImageView m_ColorImageView;

    VkImage m_DepthImage;
    VkDeviceMemory m_DepthImageMemory;
    VkImageView m_DepthImageView;
};

#endif // ENGINE_H