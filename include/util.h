#ifndef UTIL_H
#define UTIL_H

#include <format>
#include <string_view>
#include <vulkan/vulkan.h>

// Declaration of the std::formatter<VkResult> specialization
template <>
struct std::formatter<VkResult> : std::formatter<std::string_view> {
    // Declare the function that returns a string representation of VkResult
    std::string_view to_string(VkResult result) const;

    // Declare the formatting function for VkResult
    auto format(VkResult result, std::format_context &ctx);
};

#endif // UTIL_H