#ifndef UTIL_H
#define UTIL_H

#include <string_view>
#include <vulkan/vulkan.h>

namespace util {
// Function to convert VkResult to string
std::string_view to_string(VkResult result);
} // namespace util

#endif // UTIL_H