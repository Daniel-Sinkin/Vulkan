# Vulcan 3D Engine from scratch

# Setup Tutorial
Add the following to your RC file (I'm using the ZSH shell so it's at `~/.zshrc` for me).
```
# Vulkan
export VULKAN_SDK=/Users/danielsinkin/VulkanSDK/1.3.290.0/macOS
export PATH=$VULKAN_SDK/bin:$PATH
export DYLD_LIBRARY_PATH=$VULKAN_SDK/lib
export VK_LAYER_PATH=$VULKAN_SDK/etc/vulkan/explicit_layer.d
```


# References
* Apple is Incompatbile with OpenGL and Vulcan
    * [1] https://developer.apple.com/documentation/apple-silicon/porting-your-macos-apps-to-apple-silicon
    * List of versions supported by device version.
        [2] https://support.apple.com/en-ca/101525