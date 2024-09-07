#include "Constants.h"

// Initial Window size, currently we explicitly do NOT allow resizing (see initWindow)
const uint32_t WINDOW_WIDTH = 800;
const uint32_t WINDOW_HEIGHT = 600;

const vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
// VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME is a MacOS specific workaround
const vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    vector<VkSurfaceFormatKHR> formats;
    vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentationFamily;

    DEF isComplete() const -> bool {
        return graphicsFamily.has_value() && presentationFamily.has_value();
    }
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

DEF CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) -> VkResult {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

DEF DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) -> void {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

class HelloTriangleApplication {
public:
    HelloTriangleApplication()
        : m_Window(nullptr),
          m_Instance(VK_NULL_HANDLE),
          m_DebugMessenger(VK_NULL_HANDLE),
          m_PhysicalDevice(VK_NULL_HANDLE),
          m_Device(VK_NULL_HANDLE),
          m_GraphicsQueue(VK_NULL_HANDLE),
          m_PresentationQueue(VK_NULL_HANDLE),
          m_Surface(VK_NULL_HANDLE) {}

    DEF run() -> auto {
        initWindow();
        initVulkan();
        // Force a buffer flush
        std::cout << std::endl;

        mainLoop();
        cleanup();
    }

private:
    GLFWwindow *m_Window;

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;

    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device;

    VkQueue m_GraphicsQueue;
    VkQueue m_PresentationQueue;

    VkSurfaceKHR m_Surface;

    VkSwapchainKHR m_SwapChain;
    vector<VkImage> m_SwapChainImages;
    vector<VkImageView> m_SwapChainImageViews;
    VkFormat m_SwapChainImageFormat;
    VkExtent2D m_SwapChainExtent;

    VkPipelineLayout pipelineLayout;

    DEF initWindow() -> void {
        fprintf(stdout, "\nTrying to initialize window.\n");
        if (glfwInit() == GLFW_FALSE) {
            throw std::runtime_error("Failed to instantiate GLFW window!");
        }

        // GLFW defaults to creating OpenGL context if we don't pass GLFW_NO_APP explicitly
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Disables window resizing
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_Window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", nullptr, nullptr);
        fprintf(stdout, "Successfully initialized window.\n");
    }

    static DEF validateExtensions(const vector<VkExtensionProperties> &supported_extensions, const char **required_extensions, uint32_t required_extensions_count) -> bool {
        const char **ext_first = required_extensions;
        const char **ext_last = required_extensions + required_extensions_count;
        return std::all_of(ext_first, ext_last, [&](const char *required_extension) {
            std::string_view required_extension_view(required_extension);
            return std::any_of(supported_extensions.begin(), supported_extensions.end(), [&](const auto &supported_extension) {
                return std::string_view(supported_extension.extensionName) == required_extension_view;
            });
        });
    }

    DEF createInstance() -> void {
        fprintf(stdout, "\nTrying to Create Instance.\n");
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("Validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo = {};

        vector<const char *> requiredExtensions = getRequiredExtensions();
#if defined(__APPLE__) && defined(__arm64__) // MacOS specific workarounds
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        requiredExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

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

        if (!validateExtensions(extensions, requiredExtensions.data(), static_cast<uint32_t>(requiredExtensions.size()))) {
            throw std::runtime_error("Required extensions are not supported!");
        }
        fprintf(stdout, "\nSuccessfully created instance.\n");
    }

    DEF initVulkan() -> void {
        fprintf(stdout, "\n\nTrying to setup Vulkan.\n");
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createGraphicsPipeline();
        fprintf(stdout, "\n\nFinished setting up Vulkan.\n");
    }

    DEF createGraphicsPipeline() -> void {
        fprintf(stdout, "\nTrying to setup Graphics Pipeline.\n");

        fprintf(stdout, "\tTrying to createa Shader modules.\n");
        // This is a ugly workaround, for some reason c++ couldn't see the frag.spv (but the vert.spv despite being in the same directory)
        // TODO: Move this back to relative paths
        fprintf(stdout, "\tTrying to read .spv files.\n");
        auto vertShaderCode = Util::readFile("/Users/danielsinkin/GitHub_private/Vulcan/shaders/compiled/vert.spv");
        auto fragShaderCode = Util::readFile("/Users/danielsinkin/GitHub_private/Vulcan/shaders/compiled/frag.spv");

        fprintf(stdout, "\t\tTrying to create Vertex Shader.\n");
        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        fprintf(stdout, "\t\tTrying to create Fragment Shader.\n");
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
        fprintf(stdout, "Successfully created the shader modules.\n");

        fprintf(stdout, "\tTrying to Initialize Fixed Functions.\n");
        fprintf(stdout, "\t\tInitializing Vertex Input.\n");
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        fprintf(stdout, "\t\tInitializing Input Assembly.\n");
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        fprintf(stdout, "\t\tInitializing Viewport and scissors.\n");
        VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(m_SwapChainExtent.width),
            .height = static_cast<float>(m_SwapChainExtent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_SwapChainExtent;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        fprintf(stdout, "\t\tInitializing Rasterizer.\n");
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f;          // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

        fprintf(stdout, "\t\tInitializing Multisampling.\n");
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        fprintf(stdout, "\t\tInitializing Color Blending.\n");
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
        fprintf(stdout, "\tSuccessfully setup fixed function state.\n");

        fprintf(stdout, "\tCleaning up shader modules.\n");
        vkDestroyShaderModule(m_Device, fragShaderModule, nullptr);
        vkDestroyShaderModule(m_Device, vertShaderModule, nullptr);
        fprintf(stdout, "Successfully set up Graphics Pipeline.\n");
    }

    DEF createShaderModule(const std::vector<char> &code) -> VkShaderModule {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    DEF createImageViews() -> void {
        fprintf(stdout, "\nTrying to create ImageViews.\n");

        m_SwapChainImageViews.resize(m_SwapChainImages.size());
        for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
            fprintf(stdout, "\t%zu. SwapChainImageView.\n", i + 1);
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_SwapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SwapChainImageFormat;
            createInfo.components = {
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY};
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }

        fprintf(stdout, "Successfully created ImageViews.\n");
    }

    DEF createSwapChain() -> void {
        fprintf(stdout, "\nTrying to create Swapchain.\n");

        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_PhysicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        // 0 is sentinal value for no max
        if (swapChainSupport.capabilities.maxImageCount != 0) {
            imageCount = std::min(imageCount, swapChainSupport.capabilities.maxImageCount);
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_Surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);
        uint32_t QueueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentationFamily.value()};

        if (indices.graphicsFamily != indices.presentationFamily) {
            fprintf(stdout, "\tSetting imageSharingMode to Concurrent.\n");
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = QueueFamilyIndices;
        } else {
            fprintf(stdout, "\tSetting imageSharingMode to Exclusive.\n");
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        // if the swapchain becomes invalid we need to recreate with reference to previous one, this
        // for example the case when we rescale the window.
        createInfo.oldSwapchain = VK_NULL_HANDLE;

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

    DEF createSurface() -> void {
        fprintf(stdout, "\nTrying to create GLFW window surface.\n");
        if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
        fprintf(stdout, "Successfully to created GLFW window surface.\n");
    }

    DEF createLogicalDevice() -> void {
        fprintf(stdout, "\nTrying to create Logical Device.\n");

        QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);

        vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        set<uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamily.value(),
            indices.presentationFamily.value(),
        };

        // Between 0.0 and 1.0
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // For now every feature is disabled
        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        vector<const char *> requiredDeviceExtensions = getRequiredDeviceExtensions(m_PhysicalDevice);
        // requiredDeviceExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

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
        vkGetDeviceQueue(m_Device, indices.presentationFamily.value(), 0, &m_PresentationQueue);
        fprintf(stdout, "Successfully created Logical Device.\n");
    }

    DEF pickPhysicalDevice() -> void {
        fprintf(stdout, "\nPicking Physical Device.\n");

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        size_t i = 0;
        auto it = std::find_if(devices.begin(), devices.end(), [&](const auto &device) {
            fprintf(stdout, "\tChecking %zu. device", ++i);
            return isDeviceSuitable(device);
        });

        if (it != devices.end()) {
            m_PhysicalDevice = *it;
        } else {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
        fprintf(stdout, "\nSuccessfully picked physical device.\n");
    }

    DEF isDeviceSuitable(VkPhysicalDevice device) -> bool {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        fprintf(stdout, "\nChecking the device '%s' for suitability.\n", deviceProperties.deviceName);

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

        fprintf(stdout, "Device is suitable.\n");
        return true;
    }

    static DEF checkDeviceExtensionSupport(VkPhysicalDevice device) -> bool {
        uint32_t extensionCount;
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
        return indices;
    }

    DEF mainLoop() -> void {
        fprintf(stdout, "\nStarting the main loop.\n");
        while (!glfwWindowShouldClose(m_Window)) {
            glfwPollEvents();
        }
        fprintf(stdout, "Finished the main loop.\n");
    }

    DEF cleanup() -> void {
        fprintf(stdout, "\nStarting the cleanup.\n");
        vkDestroyPipelineLayout(m_Device, pipelineLayout, nullptr);
        for (auto imageView : m_SwapChainImageViews) {
            vkDestroyImageView(m_Device, imageView, nullptr);
        }

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
        }
        vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
        vkDestroyDevice(m_Device, nullptr);
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyInstance(m_Instance, nullptr);

        glfwDestroyWindow(m_Window);
        glfwTerminate();
        fprintf(stdout, "\nFinshed the cleanup.\n");
    }

    static DEF populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) -> void {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    DEF setupDebugMessenger() -> void {
        fprintf(stdout, "\nTrying to setup DebugMessanger.\n");
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

    static DEF getRequiredExtensions() -> vector<const char *> {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    static DEF getRequiredDeviceExtensions(VkPhysicalDevice device) -> vector<const char *> {
        uint32_t deviceExtensionPropertyCount = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionPropertyCount, nullptr);

        vector<VkExtensionProperties> deviceExtensionProperties(deviceExtensionPropertyCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionPropertyCount, deviceExtensionProperties.data());

        vector<const char *> extensions;
        for (const auto &property : deviceExtensionProperties) {
            if (strcmp(property.extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) == 0) {
                extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
            }
        }
        return extensions;
    }

    static DEF checkValidationLayerSupport() -> bool {
        uint32_t layerCount;
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

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    DEF querySwapChainSupport(VkPhysicalDevice device) -> SwapChainSupportDetails {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    static DEF chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) -> VkSurfaceFormatKHR {
        if (availableFormats.empty()) {
            fprintf(stderr, "No surface formats availiable!");
        }
        const auto begin_ = availableFormats.begin();
        const auto end_ = availableFormats.end();
        auto it = std::find_if(begin_, end_, [](const VkSurfaceFormatKHR &availableFormat) {
            return availableFormat == Settings::PREFERRED_SURFACE_FORMAT;
        });
        return (it != end_) ? *it : availableFormats[0];
    }

    static DEF chooseSwapPresentMode(const vector<VkPresentModeKHR> &availablePresentModes) -> VkPresentModeKHR {
        if (availablePresentModes.empty()) {
            fprintf(stderr, "No presentation modes availiable!");
        }
        const auto begin_ = availablePresentModes.begin();
        const auto end_ = availablePresentModes.end();
        auto it = std::find_if(begin_, end_, [](const VkPresentModeKHR &availablePresentMode) {
            return availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR;
        });
        return (it != end_) ? *it : availablePresentModes[0];
    }

    DEF chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) -> VkExtent2D {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_Window, &width, &height);

        VkExtent2D actualExtent = {
            std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
        };

        return actualExtent;
    }
};

DEF main() -> int {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
