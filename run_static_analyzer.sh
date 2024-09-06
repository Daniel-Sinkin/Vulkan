#!/bin/bash

echo "Running cppcheck"
cppcheck_output=$(cppcheck --inconclusive \
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
         src/*.cpp 2>&1)

# Filter out the "Checking" and "Active checkers" lines
filtered_output=$(echo "$cppcheck_output" | grep -vE "^Checking|^nofile:0:0: information: Active checkers:")

if [ -n "$filtered_output" ]; then
    echo "$cppcheck_output"
else
    echo -e "\tNo issues, good job!"
fi

echo "Running clang-tidy"
clang_tidy_output=$(clang-tidy -p build src/*.cpp \
           -header-filter='^/Users/danielsinkin/GitHub_private/Vulcan/.*' \
           --extra-arg=-Iinclude \
           --extra-arg=-I/opt/homebrew/opt/glfw/include \
           --extra-arg=-I$VULKAN_SDK/include 2>&1)

if [ -z "$clang_tidy_output" ]; then
    echo -e "\tNo issues, good job!"
else
    echo "$clang_tidy_output"
fi