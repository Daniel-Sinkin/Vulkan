#include "Constants.h"

const vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
// VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME is a MacOS specific workaround
const vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

DEF CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) -> VkResult {

    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

DEF DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator) -> void {

    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

class Engine {
public:
    Engine() = default;

    DEF run() -> void {
        fprintf(stdout, "Initializing Vulkan application.\n");
        initWindow();
        initVulkan();

        fprintf(stdout, "\nFinished Initializing Vulkan application.\n");
        std::cout << std::flush;

        mainLoop();
        cleanup();

        fprintf(stdout, "Reached the end of the Vulkan application.\n");
    }

private:
    GLFWwindow *m_Window = nullptr;

    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

    VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;
    VkFormat m_SwapChainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D m_SwapChainExtent = {};
    vector<VkFramebuffer> m_SwapChainFramebuffers;

    VkRenderPass m_RenderPass = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;

    VkCommandPool m_CommandPool;
    vector<VkCommandBuffer> m_CommandBuffers;

    vector<VkSemaphore> m_ImageAvailableSemaphores;
    vector<VkSemaphore> m_RenderFinishedSemaphores;
    vector<VkFence> m_InFlightFences;

    uint32_t m_CurrentFrameIdx = 0; // Not the total frames, 0 <= m_CurrentFrameIdx < Settings::MAX_FRAMES_IN_FLIGHT

    bool m_FramebufferResized = false;

    VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_VertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer m_IndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_IndexBufferMemory = VK_NULL_HANDLE;

    vector<VkBuffer> m_UniformBuffers;
    vector<VkDeviceMemory> m_UniformBuffersMemory;
    vector<void *> m_UniformBuffersMapped;

    VkDescriptorPool m_DescriptorPool;
    vector<VkDescriptorSet> m_DescriptorSets;

    VkImage m_TextureImage;
    VkDeviceMemory m_TextureImageMemory;

    VkImageView m_TextureImageView;
    VkSampler m_TextureSampler;

    DEF initWindow() -> void {
        fprintf(stdout, "Trying to initialize window.\n");
        if (glfwInit() == GLFW_FALSE) {
            throw std::runtime_error("Failed to instantiate GLFW window!");
        }

        // GLFW defaults to creating OpenGL context if we don't pass GLFW_NO_APP explicitly
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Disables window resizing
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_Window = glfwCreateWindow(
            Settings::DEFAULT_WINDOW_WIDTH,
            Settings::DEFAULT_WINDOW_HEIGHT,
            Settings::WINDOW_NAME,
            nullptr,
            nullptr);
        glfwSetWindowUserPointer(m_Window, this);
        glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
        fprintf(stdout, "Successfully initialized window.\n");
    }

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto app = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));
        app->m_FramebufferResized = true;
    }

    static DEF validateExtensions(const vector<VkExtensionProperties> &supported_extensions, vector<const char *> required_extensions) -> bool {
        return std::all_of(
            required_extensions.begin(),
            required_extensions.end(),
            [&](const char *required_extension) {
                std::string_view required_extension_view(required_extension);
                return std::any_of(
                    supported_extensions.begin(),
                    supported_extensions.end(),
                    [&](const auto &supported_extension) {
                        return std::string_view(supported_extension.extensionName) == required_extension_view;
                    });
            });
    }

    DEF createInstance() -> void {
        fprintf(stdout, "Trying to Create Instance.\n");
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("Validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "Hello Triangle",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "No Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_3};

        VkInstanceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr};

        vector<const char *> requiredExtensions = getRequiredExtensions();
#if defined(__APPLE__) && defined(__arm64__) // MacOS specific workarounds
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        requiredExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            // Note that pNext is in general a `const void*` ptr so we can't static_cast it
            createInfo.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT *>(&debugCreateInfo);
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        if (!validateExtensions(extensions, requiredExtensions)) {
            throw std::runtime_error("Required extensions are not supported!");
        }
        fprintf(stdout, "Successfully created instance.\n");
    }

    DEF initVulkan() -> void {
        size_t initVulkanIteration = 0;
        PRINT_BOLD_GREEN("* * * * * * * * * * * * * *");
        PRINT_BOLD_GREEN("*    Setting up Vulkan    *");
        PRINT_BOLD_GREEN("* * * * * * * * * * * * * *");

        PRINT_BOLD_GREEN("Instance and Debug Setup");
        VULKAN_SETUP(createInstance);
        VULKAN_SETUP(setupDebugMessenger);

        PRINT_BOLD_GREEN("Window Surface Setup");
        VULKAN_SETUP(createSurface);

        PRINT_BOLD_GREEN("Physical and Logical Device Setup");
        VULKAN_SETUP(pickPhysicalDevice);
        VULKAN_SETUP(createLogicalDevice);

        PRINT_BOLD_GREEN("Swap Chain Setup");
        VULKAN_SETUP(createSwapChain);
        VULKAN_SETUP(createImageViews);

        PRINT_BOLD_GREEN("Render Pass and Pipeline Setup");
        VULKAN_SETUP(createRenderPass);
        VULKAN_SETUP(createDescriptorSetLayout);
        VULKAN_SETUP(createGraphicsPipeline);

        PRINT_BOLD_GREEN("Framebuffers and Command Pool Setup");
        VULKAN_SETUP(createFramebuffers);
        VULKAN_SETUP(createCommandPool);

        PRINT_BOLD_GREEN("Buffers Setup");
        VULKAN_SETUP(createTextureImage);
        VULKAN_SETUP(createTextureImageView);
        VULKAN_SETUP(createTextureSampler);
        VULKAN_SETUP(createVertexBuffer);
        VULKAN_SETUP(createIndexBuffer);
        VULKAN_SETUP(createUniformBuffers);

        PRINT_BOLD_GREEN("Descriptor Pool and Sets Setup");
        VULKAN_SETUP(createDescriptorPool);
        VULKAN_SETUP(createDescriptorSets);

        PRINT_BOLD_GREEN("Command Buffers and Sync Objects Setup");
        VULKAN_SETUP(createCommandBuffers);
        VULKAN_SETUP(createSyncObjects);

        PRINT_BOLD_GREEN("* * * * * * * * * * * * * * * * * *");
        PRINT_BOLD_GREEN("*    Finished setting up Vulkan   *");
        PRINT_BOLD_GREEN("* * * * * * * * * * * * * * * * * *");
    }

    DEF createTextureSampler() -> void {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

        VkSamplerCreateInfo samplerInfo{
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
            .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .mipLodBias = 0.0f,
            .minLod = 0.0f,
            .maxLod = 0.0f};

        if (vkCreateSampler(m_Device, &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    DEF createImage(
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage &image,
        VkDeviceMemory &imageMemory) -> void {

        VkImageCreateInfo imageInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent = {.width = width, .height = height, .depth = 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = tiling,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

        if (vkCreateImage(m_Device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_Device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(m_Device, image, imageMemory, 0);
    }

    DEF createImageView(VkImage image, VkFormat format) -> VkImageView {
        VkImageViewCreateInfo viewInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1},
        };

        VkImageView imageView;
        if (vkCreateImageView(m_Device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    DEF createTextureImageView() -> void {
        m_TextureImageView = createImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB);
    }

    DEF createTextureImage() -> void {
        int texWidth = 0;
        int texHeight = 0;
        int texChannels = 0;
        stbi_uc *pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = static_cast<VkDeviceSize>(texWidth) * texHeight * 4;

        if (!pixels) throw std::runtime_error("failed to load texture image!");

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
        createBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void *data = nullptr;
        vkMapMemory(m_Device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(m_Device, stagingBufferMemory);

        stbi_image_free(pixels);

        createImage(
            texWidth,
            texHeight,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_TextureImage,
            m_TextureImageMemory);

        transitionImageLayout(
            m_TextureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(
            stagingBuffer,
            m_TextureImage,
            static_cast<uint32_t>(texWidth),
            static_cast<uint32_t>(texHeight));
        transitionImageLayout(
            m_TextureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
        vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
    }

    DEF beginSingleTimeCommands() -> VkCommandBuffer {
        VkCommandBufferAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = m_CommandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        vkAllocateCommandBuffers(m_Device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    DEF endSingleTimeCommands(VkCommandBuffer commandBuffer) -> void {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_GraphicsQueue);

        vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &commandBuffer);
    }

    DEF createDescriptorSets() -> void {
        fprintf(stdout, "Trying to create Descriptor Sets.\n");
        std::vector<VkDescriptorSetLayout> layouts(Settings::MAX_FRAMES_IN_FLIGHT, m_DescriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_DescriptorPool,
            .descriptorSetCount = static_cast<uint32_t>(Settings::MAX_FRAMES_IN_FLIGHT),
            .pSetLayouts = layouts.data()};
        m_DescriptorSets.resize(Settings::MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(m_Device, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < Settings::MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_UniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_DescriptorSets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .pBufferInfo = &bufferInfo};

            vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);
        }
        fprintf(stdout, "Successfully created Descriptor Sets.\n");
    }

    DEF createDescriptorPool() -> void {
        fprintf(stdout, "Trying to create Descriptor Pool.\n");
        VkDescriptorPoolSize poolSize{
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = static_cast<uint32_t>(Settings::MAX_FRAMES_IN_FLIGHT)};

        VkDescriptorPoolCreateInfo poolInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize};
        poolInfo.maxSets = static_cast<uint32_t>(Settings::MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
        fprintf(stdout, "Successfully created Descriptor Pool.\n");
    }

    DEF createUniformBuffers() -> void {
        fprintf(stdout, "Trying to create uniform buffers.\n");

        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        m_UniformBuffers.resize(Settings::MAX_FRAMES_IN_FLIGHT);
        m_UniformBuffersMemory.resize(Settings::MAX_FRAMES_IN_FLIGHT);
        m_UniformBuffersMapped.resize(Settings::MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < Settings::MAX_FRAMES_IN_FLIGHT; i++) {
            createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                m_UniformBuffers[i],
                m_UniformBuffersMemory[i]);
            VkResult result = vkMapMemory(
                m_Device,
                m_UniformBuffersMemory[i],
                0,
                bufferSize,
                0,
                &m_UniformBuffersMapped[i]);
            if (result != VK_SUCCESS) throw std::runtime_error("failed to map memory for the uniform memory buffer.");
        }
        fprintf(stdout, "Successfully created uniform buffers.\n");
    }

    DEF createDescriptorSetLayout() -> void {
        fprintf(stdout, "Trying to create descriptor set layout.\n");
        VkDescriptorSetLayoutBinding uboLayoutBinding{
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT};

        VkDescriptorSetLayoutCreateInfo layoutInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &uboLayoutBinding};

        if (vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
        fprintf(stdout, "Trying to create descriptor set layout.\n");
    }
    DEF findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) -> uint32_t {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        throw std::runtime_error("Couldn't determine the memory type.");
    }

    DEF createIndexBuffer() -> void {
        fprintf(stdout, "Trying to create Index buffer.\n");
        VkDeviceSize bufferSize = sizeof(vertexIndices[0]) * vertexIndices.size();

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
        createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void *data = nullptr;
        vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertexIndices.data(), (size_t)bufferSize);
        vkUnmapMemory(m_Device, stagingBufferMemory);

        createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_IndexBuffer,
            m_IndexBufferMemory);

        copyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

        vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
        vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
        fprintf(stdout, "Successfully created Index buffer.\n");
    }

    DEF createVertexBuffer() -> void {
        fprintf(stdout, "Trying to create Vertex buffer.\n");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
        createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void *data = nullptr;
        vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m_Device, stagingBufferMemory);

        createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_VertexBuffer,
            m_VertexBufferMemory);

        copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

        vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
        vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
        fprintf(stdout, "Successfully created Vertex buffer.\n");
    }

    DEF createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer &buffer,
        VkDeviceMemory &bufferMemory) -> void {

        VkBufferCreateInfo bufferInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

        if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_Device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties)};

        if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(m_Device, buffer, bufferMemory, 0);
    }

    DEF copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) -> void {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    DEF copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) -> void {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1},
            .imageOffset = {.x = 0, .y = 0, .z = 0},
            .imageExtent = {.width = width, .height = height, .depth = 1}};

        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);

        endSingleTimeCommands(commandBuffer);
    }

    DEF transitionImageLayout(
        VkImage image,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout) -> void {

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();
        VkImageMemoryBarrier barrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = 0,
            .dstAccessMask = 0,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        VkPipelineStageFlags sourceStage = 0;
        VkPipelineStageFlags destinationStage = 0;
        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        endSingleTimeCommands(commandBuffer);
    }

    DEF createSyncObjects() -> void {
        fprintf(stdout, "Trying to create Sync Objects.\n");
        m_ImageAvailableSemaphores.resize(Settings::MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(Settings::MAX_FRAMES_IN_FLIGHT);
        m_InFlightFences.resize(Settings::MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        VkFenceCreateInfo fenceInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT};

        for (size_t i = 0; i < Settings::MAX_FRAMES_IN_FLIGHT; i++) {
            fprintf(stdout, "\t%zu. frame\n", i + 1);
            VkResult result_1 = vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]);
            if (result_1 != VK_SUCCESS) throw std::runtime_error("failed to create ImageAvailable semaphore!");
            VkResult result_2 = vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]);
            if (result_2 != VK_SUCCESS) throw std::runtime_error("failed to create RenderFinished semaphore!");
            VkResult result_3 = vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFences[i]);
            if (result_3 != VK_SUCCESS) throw std::runtime_error("failed to create InFlight fence!");
        }
        fprintf(stdout, "Finished creating Sync Objects.\n");
    }

    DEF createCommandBuffers() -> void {
        fprintf(stdout, "Trying to setup Command Buffers.\n");
        m_CommandBuffers.resize(Settings::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = m_CommandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size())};

        if (vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
        fprintf(stdout, "Successfully set up Command Buffers.\n");
    }

    DEF recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) -> void {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkOffset2D offset = {0, 0};
        VkExtent2D extent = m_SwapChainExtent;
        VkRect2D renderArea = {offset, extent};
        VkRenderPassBeginInfo renderPassInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = m_RenderPass,
            .framebuffer = m_SwapChainFramebuffers[imageIndex],
            .renderArea = renderArea};

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

        VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(m_SwapChainExtent.width),
            .height = static_cast<float>(m_SwapChainExtent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{.offset = {0, 0}, .extent = m_SwapChainExtent};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        array<VkBuffer, 1> vertexBuffers = {m_VertexBuffer};
        array<VkDeviceSize, 1> offsets = {0};

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers.data(), offsets.data());
        vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[m_CurrentFrameIdx], 0, nullptr);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(vertexIndices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    DEF createCommandPool() -> void {
        fprintf(stdout, "Trying to setup Command Pool.\n");
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_PhysicalDevice);
        if (!queueFamilyIndices.graphicsFamily.has_value()) {
            throw std::runtime_error("QueueFamilyIndices GraphicsFamily has no value!");
        }
        VkCommandPoolCreateInfo poolInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()};

        if (vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
        fprintf(stdout, "Successfully set up Command Pool.\n");
    }

    DEF createFramebuffers() -> void {
        fprintf(stdout, "Trying to setup Framebuffers.\n");
        m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());
        for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
            fprintf(stdout, "\t%zu. Framebuffers.\n", i + 1);
            std::array<VkImageView, 1> attachments = {m_SwapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass = m_RenderPass,
                .attachmentCount = 1,
                .pAttachments = attachments.data(),
                .width = m_SwapChainExtent.width,
                .height = m_SwapChainExtent.height,
                .layers = 1,
            };

            if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }

        fprintf(stdout, "Successfully set up Framebuffers.\n");
    }

    DEF createRenderPass() -> void {
        fprintf(stdout, "Trying to setup Render Pass.\n");
        VkAttachmentDescription colorAttachment{
            .format = m_SwapChainImageFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};

        VkAttachmentReference colorAttachmentRef{
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

        VkSubpassDescription subpass{
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentRef};

        VkSubpassDependency dependency{
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT};

        VkRenderPassCreateInfo renderPassInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &colorAttachment,
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 1,
            .pDependencies = &dependency};

        if (vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }

        fprintf(stdout, "Successfully set upped Render Pass.\n");
    }

    DEF createGraphicsPipeline() -> void {
        fprintf(stdout, "Trying to setup Graphics Pipeline.\n");

        fprintf(stdout, "Trying to create Shader modules.\n");
        fprintf(stdout, "Trying to read .spv files.\n");
        vector<char> vertShaderCode = Util::readFile(FilePaths::SHADER_VERT);
        vector<char> fragShaderCode = Util::readFile(FilePaths::SHADER_FRAG);

        fprintf(stdout, "\tTrying to create Vertex Shader.\n");
        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertShaderModule,
            .pName = "main"};

        fprintf(stdout, "\tTrying to create Fragment Shader.\n");
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragShaderModule,
            .pName = "main"};

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {vertShaderStageInfo, fragShaderStageInfo};
        fprintf(stdout, "Successfully created the shader modules.\n");

        fprintf(stdout, "Trying to Initialize Fixed Functions.\n");
        fprintf(stdout, "\tInitializing Vertex Input.\n");
        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &bindingDescription,
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
            .pVertexAttributeDescriptions = attributeDescriptions.data()};

        fprintf(stdout, "\tInitializing Input Assembly.\n");
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE};

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo dynamicState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data()};

        VkPipelineViewportStateCreateInfo viewportState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .scissorCount = 1};

        fprintf(stdout, "\tInitializing Rasterizer.\n");
        VkPipelineRasterizationStateCreateInfo rasterizer{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f};

        fprintf(stdout, "\tInitializing Multisampling.\n");
        VkPipelineMultisampleStateCreateInfo multisampling{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE};

        fprintf(stdout, "\tInitializing Color Blending.\n");
        VkPipelineColorBlendAttachmentState colorBlendAttachment{
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };

        VkPipelineColorBlendStateCreateInfo colorBlending{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment,
            .blendConstants = {0.0F, 0.0F, 0.0F, 0.0F}};

        fprintf(stdout, "\tInitializing Render Pipeline.\n");
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 1,
            .pSetLayouts = &m_DescriptorSetLayout};

        if (vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = 2,
            .pStages = shaderStages.data(),
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = m_PipelineLayout,
            .renderPass = m_RenderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE};

        if (vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        fprintf(stdout, "Cleaning up shader modules.\n");
        vkDestroyShaderModule(m_Device, fragShaderModule, nullptr);
        vkDestroyShaderModule(m_Device, vertShaderModule, nullptr);
        fprintf(stdout, "Successfully set up Graphics Pipeline.\n");
    }

    DEF createShaderModule(const std::vector<char> &code) -> VkShaderModule {
        VkShaderModuleCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = code.size(),
            .pCode = reinterpret_cast<const uint32_t *>(code.data()),
        };

        VkShaderModule shaderModule = VK_NULL_HANDLE;
        if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    DEF createImageViews() -> void {
        fprintf(stdout, "Trying to create ImageViews.\n");

        m_SwapChainImageViews.resize(m_SwapChainImages.size());

        for (uint32_t i = 0; i < m_SwapChainImages.size(); i++) {
            m_SwapChainImageViews[i] = createImageView(m_SwapChainImages[i], m_SwapChainImageFormat);
        }

        fprintf(stdout, "Successfully created ImageViews.\n");
    }

    DEF createSwapChain() -> void {
        fprintf(stdout, "Trying to create Swapchain.\n");

        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_PhysicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        // 0 is sentinal value for no max
        if (swapChainSupport.capabilities.maxImageCount != 0) {
            imageCount = std::min(imageCount, swapChainSupport.capabilities.maxImageCount);
        }

        VkSwapchainCreateInfoKHR createInfo{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = m_Surface,
            .minImageCount = imageCount,
            .imageFormat = surfaceFormat.format,
            .imageColorSpace = surfaceFormat.colorSpace,
            .imageExtent = extent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform = swapChainSupport.capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = presentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE,
        };

        QueueFamilyIndices queueIndices = findQueueFamilies(m_PhysicalDevice);
        array<uint32_t, 2> queueFamilyIndices;
        if (queueIndices.graphicsFamily.has_value() && queueIndices.presentationFamily.has_value()) {
            queueFamilyIndices = {queueIndices.graphicsFamily.value(), queueIndices.presentationFamily.value()};
        } else {
            throw std::runtime_error("QueueFamilyIndices not complete!");
        }

        if (queueIndices.graphicsFamily != queueIndices.presentationFamily) {
            fprintf(stdout, "Setting imageSharingMode to Concurrent.\n");
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
        } else {
            fprintf(stdout, "Setting imageSharingMode to Exclusive.\n");
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

        m_SwapChainImageFormat = surfaceFormat.format;
        m_SwapChainExtent = extent;

        fprintf(stdout, "Finished setting up the swapchain.\n");
    }

    DEF cleanupSwapChain() -> void {
        for (auto frameBuffer : m_SwapChainFramebuffers) {
            vkDestroyFramebuffer(m_Device, frameBuffer, nullptr);
        }

        for (auto imageView : m_SwapChainImageViews) {
            vkDestroyImageView(m_Device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
    }

    DEF recreateSwapChain() -> void {
        int height = INVALID_FRAMEBUFFER_SIZE;
        int width = INVALID_FRAMEBUFFER_SIZE;
        glfwGetFramebufferSize(m_Window, &width, &height);
        while (width == INVALID_FRAMEBUFFER_SIZE || height == INVALID_FRAMEBUFFER_SIZE) {
            glfwGetFramebufferSize(m_Window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_Device);

        createSwapChain();
        createImageViews();
        createFramebuffers();
    }

    DEF createSurface() -> void {
        fprintf(stdout, "Trying to create GLFW window surface.\n");
        if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
        fprintf(stdout, "Successfully to created GLFW window surface.\n");
    }

    DEF createLogicalDevice() -> void {
        fprintf(stdout, "Trying to create Logical Device.\n");

        QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);

        vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        set<uint32_t> uniqueQueueFamilies;
        if (indices.graphicsFamily.has_value() && indices.presentationFamily.has_value()) {
            uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentationFamily.value()};
        } else {
            throw std::runtime_error("QueueFamilyIndices are not fully defined.");
        }

        // Between 0.0 and 1.0
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = queueFamily,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority};
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{.samplerAnisotropy = VK_TRUE};

        VkDeviceCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data(),
            .pEnabledFeatures = &deviceFeatures};

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, indices.presentationFamily.value(), 0, &m_PresentQueue);
        fprintf(stdout, "Successfully created Logical Device.\n");
    }

    DEF pickPhysicalDevice() -> void {
        fprintf(stdout, "Picking Physical Device.\n");

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        size_t i = 0;
        auto found = std::find_if(
            devices.begin(),
            devices.end(),
            [&](const auto &device) {
                fprintf(stdout, "Checking %zu. device", ++i);
                return isDeviceSuitable(device);
            });
        if (found == devices.end()) throw std::runtime_error("failed to find a suitable GPU!");

        m_PhysicalDevice = *found;
        fprintf(stdout, "Successfully picked physical device.\n");
    }

    DEF isDeviceSuitable(VkPhysicalDevice device) -> bool {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        fprintf(stdout, "Checking the device '%s' for suitability.\n", deviceProperties.deviceName);

        if (!Settings::ALLOW_DEVICE_WITHOUT_INTEGRATED_GPU) {
            if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                fprintf(stdout, "Device is unsuitable because it's not a discrete GPU!.");
            } else {
                fprintf(stdout, "Device is a discrete GPU!.");
            }
        } else {
            fprintf(stdout, "'%s' flag is set so we don't check if it's a discrete GPU.\n", "Settings::ALLOW_DEVICE_WITHOUT_INTEGRATED_GPU");
        }

        if (!Settings::ALLOW_DEVICE_WITHOUT_GEOMETRY_SHADER) {
            if (!deviceFeatures.geometryShader) {
                fprintf(stdout, "Device is unsuitable because it does not support Geometry Shaders!.\n");
                return false;
            } else {
                fprintf(stdout, "Device supports Geometry Shaders.\n");
            }
        } else {
            fprintf(stdout, "'%s' flag is set so we don't check if it supports geometry shaders.\n", "Settings::ALLOW_DEVICE_WITHOUT_GEOMETRY_SHADER");
        }

        QueueFamilyIndices indices = findQueueFamilies(device);
        if (!indices.isComplete()) {
            fprintf(stdout, "Device is unsuitable because its QueueFamily is incomplete!\n");
            return false;
        } else {
            fprintf(stdout, "QueueFamily of the Device is complete.\n");
        }

        bool extensionsSupported = checkDeviceExtensionSupport(device);
        if (!extensionsSupported) {
            fprintf(stdout, "Device is unsuitable because it does not support the necessary extensions!\n");
            return false;
        } else {
            fprintf(stdout, "Device supports the necessary extensions.\n");
        }

        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
            fprintf(stdout, "Device is unsuitable because its swapChain is not adequate!\n");
            return false;
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
        if (!supportedFeatures.samplerAnisotropy) {
            fprintf(stdout, "Device is unsuitable because does not support sampler Anisotropy (that is very surprising)!\n");
            return false;
        }

        fprintf(stdout, "Device is suitable.\n");
        return true;
    }

    static DEF checkDeviceExtensionSupport(VkPhysicalDevice device) -> bool {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        vector<VkExtensionProperties> availiableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availiableExtensions.data());

        set<string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto &extension : availiableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    DEF findQueueFamilies(VkPhysicalDevice device) -> QueueFamilyIndices {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
            if (presentSupport) {
                indices.presentationFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }
            i++;
        }
        if (!indices.isComplete()) throw std::runtime_error("findQUeueFamilies couldn't find the necessary values!\n");
        return indices;
    }

    DEF mainLoop() -> void {
        fprintf(stdout, "Starting the main loop.\n");
        while (!glfwWindowShouldClose(m_Window)) {
            glfwPollEvents();
            drawFrame();
        }

        fprintf(stdout, "Finished mainLoop, waiting for the device to idle.\n");
        vkDeviceWaitIdle(m_Device);
        fprintf(stdout, "Finished waiting.\n");
    }

    DEF drawFrame() -> void {
        vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrameIdx], VK_TRUE, NO_TIMEOUT);
        uint32_t imageIndex = 0;
        VkResult resultNextImage = vkAcquireNextImageKHR(
            m_Device,
            m_SwapChain,
            NO_TIMEOUT,
            m_ImageAvailableSemaphores[m_CurrentFrameIdx],
            VK_NULL_HANDLE,
            &imageIndex);

        if (resultNextImage == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        } else if (resultNextImage != VK_SUCCESS && resultNextImage != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrameIdx]);

        vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrameIdx], 0);
        recordCommandBuffer(m_CommandBuffers[m_CurrentFrameIdx], imageIndex);

        array<VkSemaphore, 1> waitSemaphores = {m_ImageAvailableSemaphores[m_CurrentFrameIdx]};
        array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        array<VkSemaphore, 1> signalSemaphores = {m_RenderFinishedSemaphores[m_CurrentFrameIdx]};

        updateUniformBuffer(m_CurrentFrameIdx);

        VkSubmitInfo submitInfo{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores.data(),
            .pWaitDstStageMask = waitStages.data(),
            .commandBufferCount = 1,
            .pCommandBuffers = &m_CommandBuffers[m_CurrentFrameIdx],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signalSemaphores.data()};

        if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrameIdx]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        array<VkSwapchainKHR, 1> swapChains = {m_SwapChain};
        VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = signalSemaphores.data(),
            .swapchainCount = 1,
            .pSwapchains = swapChains.data(),
            .pImageIndices = &imageIndex,
            .pResults = nullptr};

        VkResult resultQueue = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

        if (resultQueue == VK_ERROR_OUT_OF_DATE_KHR || resultQueue == VK_SUBOPTIMAL_KHR || m_FramebufferResized) {
            m_FramebufferResized = false;
            recreateSwapChain();
        } else if (resultQueue != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        m_CurrentFrameIdx = (m_CurrentFrameIdx + 1) % Settings::MAX_FRAMES_IN_FLIGHT;
    }

    DEF updateUniformBuffer(uint32_t currentImage) -> void {
        // The static here makes it into a global static variable so the next function call will not overwrite it.
        static time_point startTime = std::chrono::high_resolution_clock::now();

        time_point currentTime = std::chrono::high_resolution_clock::now();
        float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{
            .model = glm::rotate(mat4(1.0f),
                delta_time * PI_HALF,
                vec3(0.0f, 0.0f, 1.0f)),
            .view = glm::lookAt(Settings::CAMERA_EYE, Settings::CAMERA_CENTER, Settings::CAMERA_UP),
            .proj = glm::perspective(PI_QUARTER,
                static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height),
                Settings::CLIPPING_PLANE_NEAR,
                Settings::CLIPPING_PLANE_FAR)};

        // GLM uses OpenGL convention, this fixes that
        ubo.proj[1][1] *= -1;

        memcpy(m_UniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    }

    DEF cleanup() -> void {
        fprintf(stdout, "Starting the cleanup.\n");
        fprintf(stdout, "Starting the Vulkan cleanup.\n");

        for (size_t i = 0; i < Settings::MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
        }
        cleanupSwapChain();

        vkDestroySampler(m_Device, m_TextureSampler, nullptr);
        vkDestroyImageView(m_Device, m_TextureImageView, nullptr);

        vkDestroyImage(m_Device, m_TextureImage, nullptr);
        vkFreeMemory(m_Device, m_TextureImageMemory, nullptr);

        vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);

        vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
        vkFreeMemory(m_Device, m_IndexBufferMemory, nullptr);
        vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
        vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);

        for (size_t i = 0; i < Settings::MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(m_Device, m_UniformBuffers[i], nullptr);
            vkFreeMemory(m_Device, m_UniformBuffersMemory[i], nullptr);
        }

        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);

        vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
        vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
        }
        vkDestroyDevice(m_Device, nullptr);
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
        fprintf(stdout, "Finished the Vulkan cleanup.\n");

        fprintf(stdout, "Started the the GLFW cleanup.\n");
        glfwDestroyWindow(m_Window);
        glfwTerminate();
        fprintf(stdout, "Finished the GLFW cleanup.\n");
        fprintf(stdout, "Finshed the cleanup.\n");
    }

    static DEF populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) -> void {
        createInfo = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback};
    }

    DEF setupDebugMessenger() -> void {
        fprintf(stdout, "Trying to setup DebugMessanger.\n");
        if (!enableValidationLayers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        VkResult result = CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
        fprintf(stdout, "Successfully setup DebugMessanger.\n");
    }

    static DEF getRequiredExtensions() -> std::vector<const char *> {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::span<const char *> glfwExtensionSpan(glfwExtensions, glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensionSpan.begin(), glfwExtensionSpan.end());
        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    static DEF checkValidationLayerSupport() -> bool {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        vector<VkLayerProperties> availableLayers(layerCount);
        (void)vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName : validationLayers) {
            std::string_view layerNameView(layerName);

            bool layerFound = std::any_of(
                availableLayers.begin(),
                availableLayers.end(),
                [&](const auto &layerProperties) {
                    return layerNameView == std::string_view(layerProperties.layerName);
                });

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    static VKAPI_ATTR DEF VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData) -> VkBool32 {

        std::cerr << "validation layer: " << pCallbackData->pMessage << "";
        return VK_FALSE;
    }

    DEF querySwapChainSupport(VkPhysicalDevice device) -> SwapChainSupportDetails {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            device,
            m_Surface,
            &details.capabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            device,
            m_Surface,
            &formatCount,
            details.formats.data());
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                device,
                m_Surface,
                &formatCount,
                details.formats.data());
        }

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device,
            m_Surface,
            &presentModeCount,
            nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                m_Surface,
                &presentModeCount,
                details.presentModes.data());
        }

        return details;
    }

    static DEF chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) -> VkSurfaceFormatKHR {
        if (availableFormats.empty()) fprintf(stderr, "No surface formats availiable!");

        auto found = std::find_if(
            availableFormats.begin(),
            availableFormats.end(),
            [](const VkSurfaceFormatKHR &availableFormat) {
                return availableFormat.format == Settings::PREFERRED_SURFACE_FORMAT.format &&
                       availableFormat.colorSpace == Settings::PREFERRED_SURFACE_FORMAT.colorSpace;
            });
        return (found != availableFormats.end()) ? *found : availableFormats[0];
    }

    static DEF chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) -> VkPresentModeKHR {
        if (availablePresentModes.empty()) fprintf(stderr, "No presentation modes availiable!");

        auto found = std::find_if(
            availablePresentModes.begin(),
            availablePresentModes.end(),
            [](const VkPresentModeKHR &availablePresentMode) {
                return availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR;
            });
        return (found != availablePresentModes.end()) ? *found : availablePresentModes[0];
    }

    DEF chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) -> VkExtent2D {
        bool isExtentUndefined = capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max();
        if (!isExtentUndefined) return capabilities.currentExtent;

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_Window, &width, &height);
        VkExtent2D actualExtent = {
            .width = std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            .height = std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
        };

        return actualExtent;
    }
};

DEF main() -> int {
    Engine app;

    try {
        fprintf(stdout, "Starting application run.\n");
        app.run();
        fprintf(stdout, "Finished application run.\n");
    } catch (const std::exception &e) {
        std::cerr << e.what() << "";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
