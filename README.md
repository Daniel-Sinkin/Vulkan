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
    * [x] Graphics pipeline basics
        * [x] Shader modules
        * [x] Fixed functions
        * [x] Render passes 
        * [x] Conclusion
    * [x] Drawing
        * [x] Framebuffers
        * [x] Command Buffers
        * [x] Rendering and presentation
        * [x] Frames in flight
    * [x] Swap chain recreation
* [x] Vertex Buffers
    * [x] Vertex Input description
    * [x] Vertex Buffer Creation
    * [x] Staging Buffer
    * [x] Index Buffer
* [x] Uniform Buffers
    * [x] Descriptor layout and buffer
    * [x] Descriptor pool and sets
* [x] Texture Mapping
    * [x] Images
    * [x] Image view and sampler
    * [x] Combined image sampler 
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
* Libraries Used
    * 
* Apple is Incompatbile with OpenGL and Vulcan
    * https://developer.apple.com/documentation/apple-silicon/porting-your-macos-apps-to-apple-silicon
    * List of versions supported by device version.
        * https://support.apple.com/en-ca/101525
* Documentation
    * https://www.glfw.org/docs/latest/vulkan_guide.html
* Educational Resources
    * Books
        * [Physically Based Rendering: From Theory to Implementation](https://www.pbr-book.org)
        * [Data-Oriented Design](https://www.dataorienteddesign.com/dodbook/)
        * [Ray Tracing in one Weekend](https://raytracing.github.io)
        * [Computer Graphics from Scratch](https://gabrielgambetta.com/computer-graphics-from-scratch/)
        * [The Art of writing Efficient Programs](https://github.com/ssloy/tinyraytracer/wiki)
    * Repositories
        * [tinyraytracer](https://github.com/ssloy/tinyraytracer/wiki)
    * Blogs
        * [The Essential Resources for Vulkan development](https://www.vulkan.org/learn)
        * [Nvidia's GPU Gems series](https://developer.nvidia.com/gpugems/gpugems/contributors)
        * [A trip through the Graphics Pipeline 2011: Index](https://fgiesen.wordpress.com/2011/07/09/a-trip-through-the-graphics-pipeline-2011-index/)
        * [Learning Modern 3D Graphics Programming](https://paroj.github.io/gltut/)
        * [Intel: API without secrets: Introduction to Vulkan](https://www.intel.com/content/www/us/en/developer/articles/training/api-without-secrets-introduction-to-vulkan-part-1.html)
    * Tutorials
        * [Vulkan Tutorial](https://vulkan-tutorial.com)
        * [Learn OpenGL](https://learnopengl.com)
        * [VulkanGuide](https://vkguide.dev)
    * YouTube
        * [MollyRocket]
            * Handmade Hero Series
                * [Rendering Playlist](https://www.youtube.com/watch?v=ofMJUSchXwo&list=PLEMXAbCVnmY40lfaaowTqIs_dKNgOXR5Q)
                * [Lighting Playlist](https://www.youtube.com/watch?v=owpVP0IQWXk&list=PLEMXAbCVnmY4ASbr-fMBSroE2JF-u20du)
                * [Optimization Playlist](https://www.youtube.com/watch?v=qin-Eps3U_E&list=PLEMXAbCVnmY5qGQB96s7Vysr1nJcX_BW_)
                * [Multithreading Playlist](https://www.youtube.com/watch?v=qkugPXGeX58&list=PLEMXAbCVnmY7me6j4VtpCYMuZX3QpcBBH)
            * [Performanc-Aware Programming Playlist](https://www.youtube.com/watch?v=pZ0MF1q_LUE&list=PLEMXAbCVnmY7t29i_rd3mnALWu-aZr_42)
        * [Acerola](https://www.youtube.com/@Acerola_t)
            * Post-Processind with Shaders
        * [GetIntoGameDev's Vulkan 2024 Series](https://www.youtube.com/watch?v=Est5AvResbE&list=PLn3eTxaOtL2Nr89hYzKPib7tvce-ZO4yB)
        * CppCon 2021
            * [Back to Basics Series](https://www.youtube.com/watch?v=Bt3zcJZIalk&list=PLHTh1InhhwT4TJaHBVWzvBOYhp27UO7mI)
        * C++Now 2024
            * [Unlocking Modern CPU Power - Next-Gen C++ Optimization Techniques - Fedor G Pikus - C++Now 2024
](https://www.youtube.com/watch?v=wGSSUSeaLgA)
        * [Interactive Computer Graphics](https://www.youtube.com/watch?v=UVCuWQV_-Es&list=PLplnkTzzqsZS3R5DjmCQsqupu43oS9CFN&index=1)
