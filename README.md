# Vulcan 3D Engine from scratch


# Setup
You can download the newest Vulkan SDK [here](https://vulkan.lunarg.com/sdk/home) 

I'm using version 1.3.290.0 (23-Jul-2024) which has the following SHA256 checksum:
```
a14f3026290c2ef0a9fc96af3f6b75135018d145748543a644107a36f1d65a71
```

Add the following to your RC file (I'm using the ZSH shell so it's at `~/.zshrc` for me).
```
# Vulkan
export VULKAN_SDK=/Users/danielsinkin/VulkanSDK/1.3.290.0/macOS
export PATH=$VULKAN_SDK/bin:$PATH
export DYLD_LIBRARY_PATH=$VULKAN_SDK/lib
export VK_LAYER_PATH=/Users/danielsinkin/VulkanSDK/1.3.290.0/macOS/etc/vulkan/explicit_layer.d
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