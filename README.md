# Vulcan 3D Engine from scratch

# Notes
* As this is supposed to be a portfolio project it is vital to put in effort into presentation
    * Could look into CodeSnap and its alternatives
* I'm at Swap Chains right now

# Vulkan Tutorial Roadmap
* [x] Development Environment
* [x] Drawing a Triangle
    * [x] Setup
        * [x] Base Code
        * [x] Instance
        * [x] Validation Layers
        * [x] Physical Devices and queue families
        * [x] Logical Devices and queues
    * [x] Presentation
        * [x] Window Surface
        * [x] Swap Chain
        * [x] Image Views
    * [ ] Graphics pipeline basics
        * [ ] Shader modules
        * [ ] Fixed functions
        * [ ] Render passes 
    * [ ] Drawing
    * [ ] Swap chain recreation
* [ ] Vertex Buffers
* [ ] Texture Mapping
* [ ] Depth Buffering
* [ ] Loading Models
* [ ] Generating Mipmaps
* [ ] Multisampling
* [ ] Compute Shader



# Reminders and things to look out for
* (Currently not implemented) Be careful whether you are compiling with DEBUG flag set or not
* (Currently always on) The runtime sanitizers in compilation settings are VERY slow, active during debug, if things are slow should try to disable those first.
* Regarding MacOS Specific Workarounds
    * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_portability_enumeration.html
    * https://github.com/ocornut/imgui/issues/6101#issuecomment-1398223233
    * https://stackoverflow.com/questions/72374316/validation-error-on-device-extension-on-m1-mac
    * https://stackoverflow.com/questions/66659907/vulkan-validation-warning-catch-22-about-vk-khr-portability-subset-on-moltenvk/66660106#66660106
    * https://www.youtube.com/watch?v=7Sp7waWUXOc

# Setup
I'm using AppleClang 15.0.0.15000309 as my compiler and CMake to generate my make files. The project can be rebuild by using the `clean_and_recompile.sh` script (Warning: This wipes all build information and so can be quite slow. Although I don't build my dependencies from scratch so it shouldn't be too bad. Might build vulcan from source later on, but currently I just use the SDK directly.)

If you want to compile the project you can either use the `compile_and_run.sh` (just comment out the last line if you don't want to run after building)
```

You can download the newest Vulkan SDK [here](https://vulkan.lunarg.com/sdk/home) 

I'm using version 1.3.290.0 (23-Jul-2024) which has the following SHA256 checksum:
```
a14f3026290c2ef0a9fc96af3f6b75135018d145748543a644107a36f1d65a71
```

Add the following to your RC file (I'm using the ZSH shell so it's at `~/.zshrc` for me).
```
# Vulkan
export VULKAN_SDK=/Users/danielsinkin/VulkanSDK/1.3.290.0/macOS
export GLSC=$VULKAN_SDK/bin/glsc
export PATH=$VULKAN_SDK/bin:$PATH
export DYLD_LIBRARY_PATH=$VULKAN_SDK/lib:/usr/local/lib:$DYLD_LIBRARY_PATH
export VK_LAYER_PATH=$VULKAN_SDK/share/vulkan/explicit_layer.d
# export VK_INSTANCE_LAYERS="VK_LAYER_LUNARG_api_dump:VK_LAYER_KHRONOS_validation"
export VK_INSTANCE_LAYERS="VK_LAYER_KHRONOS_validation"
export VK_ICD_FILENAMES=/Users/danielsinkin/VulkanSDK/1.3.290.0/macOS/share/vulkan/icd.d/MoltenVK_icd.json
```

# My Dev Specs
```
> system_profiler
    Software
        System Version: macOS 14.6.1 (23G93)
        Kernel Version: Darwin 23.6.0

    Hardware
        Model Name: MacBook Air
        Model Identifier: Mac14,2
        Model Number: MN703D/A
        Chip: Apple M2
        Total Number of Cores: 8 (4 performance and 4 efficiency)
        Memory: 16 GB
        System Firmware Version: 10151.140.19
        OS Loader Version: 10151.140.19
        Serial Number (system): *
        Hardware UUID: *
        Provisioning UDID: *
        Activation Lock Status: *

    Graphics/Displays:
        Chipset Model: Apple M2
        Type: GPU
        Bus: Built-In
        Total Number of Cores: 10
        Vendor: Apple (0x106b)
        Metal Support: Metal 3
        Displays:
            Studio Display:
                Display Type: Retina LCD
                Resolution: 5120 x 2880 Retina
                Display Serial Number: *
                Display Firmware Version: Version 17.0 (Build 21A329)
                Main Display: Yes
                Mirror: Off
                Online: Yes
                Automatically Adjust Brightness: No

```


# References
* Apple is Incompatbile with OpenGL and Vulcan
    * [1] https://developer.apple.com/documentation/apple-silicon/porting-your-macos-apps-to-apple-silicon
    * List of versions supported by device version.
        * [2] https://support.apple.com/en-ca/101525
* Documentation
    * [3] https://www.glfw.org/docs/latest/vulkan_guide.html
* Educational Resources
    * [4] [Vulkan Tutorial](https://vulkan-tutorial.com)
    * [5] [Learning Modern 3D Graphics Programming](https://paroj.github.io/gltut/)