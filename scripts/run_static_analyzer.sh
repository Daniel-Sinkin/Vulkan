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
                           --suppress=missingInclude \
                           --suppress=normalCheckLevelMaxBranches \
                           --suppress=checkersReport \
                           --suppress=initializerList \
                           --suppress=unusedFunction \
                           src/*.cpp 2>&1)

# Check if cppcheck output contains any issues (warnings/errors)
if echo "$cppcheck_output" | grep -q "error\|warning"; then
    echo "$cppcheck_output"
else
    echo "    No issues, good job!"
fi

echo "Running clang-tidy"
clang_tidy_output=$(clang-tidy -p build src/*.cpp \
                               -header-filter='^/Users/danielsinkin/GitHub_private/Vulcan/.*' \
                               --extra-arg=-Iinclude \
                               --extra-arg=-I/opt/homebrew/opt/glfw/include \
                               --extra-arg=-I$VULKAN_SDK/include 2>&1)

# Filter out irrelevant warnings or information messages including "Processing file"
clang_tidy_filtered=$(echo "$clang_tidy_output" | grep -v "warnings generated\|Suppressed\|Use -header-filter\|Processing file")

# Check if any relevant warnings or errors remain after filtering
if [ -n "$clang_tidy_filtered" ]; then
    echo "$clang_tidy_filtered"
else
    echo "    No issues, good job!"
fi