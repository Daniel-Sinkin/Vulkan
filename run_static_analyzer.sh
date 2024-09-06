#!/bin/bash

echo "Running cppcheck"
cppcheck --inconclusive \
         --enable=all \
         --enable=performance \
         --enable=unusedFunction \
         --enable=style \
         --enable=portability \
         --force \
         -I include \
         --suppress=missingIncludeSystem \
         --suppress=\*:Users/danielsinkin/VulkanSDK \
         --suppress=\*:opt/homebrew/opt/glfw \
         src/*.cpp

echo "Running clang-tidy"
clang-tidy -p build src/*.cpp \
           -header-filter='^/Users/danielsinkin/GitHub_private/Vulcan/.*' \
           --extra-arg=-Iinclude \
           --extra-arg=-I/opt/homebrew/opt/glfw/include \
           --extra-arg=-I$VULKAN_SDK/include