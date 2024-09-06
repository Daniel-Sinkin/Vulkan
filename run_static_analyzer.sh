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