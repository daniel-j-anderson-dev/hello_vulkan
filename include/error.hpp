#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>
#include <string_view>

enum class ErrorKind {
  FailedToCreateWindow,
  FailedToInitializeVulkan,
};

struct Error {
 private:
  ErrorKind _kind;
  std::optional<VkResult> failed_to_initialize_vulkan_data;

  Error() = delete;
  Error(ErrorKind kind,
        std::optional<VkResult> failed_to_initialize_vulkan_data)
      : _kind(kind),
        failed_to_initialize_vulkan_data(failed_to_initialize_vulkan_data) {};

 public:
  constexpr static auto failed_to_create_window() -> Error {
    return Error(ErrorKind::FailedToCreateWindow, std::nullopt);
  }

  constexpr static auto failed_to_initialize_vulkan(
      VkResult initialization_error) -> Error {
    return Error(ErrorKind::FailedToInitializeVulkan, initialization_error);
  }

  constexpr auto kind(this const Error& self) noexcept -> const ErrorKind& {
    return self._kind;
  }

  auto to_string(this const Error& self) noexcept -> std::string_view {
    switch (self._kind) {
      case ErrorKind::FailedToCreateWindow:
        return "Failed to initialize GLFW Window";
      case ErrorKind::FailedToInitializeVulkan:
        return "Failed to initialize Vulkan";
      default:
        return "UNRECOGNIZED ERROR KIND THIS IS A BUG";
    }
  }
};

constexpr auto is_create_instance_error(VkResult create_instance_result)
    -> bool {
  return create_instance_result == VK_ERROR_OUT_OF_HOST_MEMORY ||
         create_instance_result == VK_ERROR_OUT_OF_DEVICE_MEMORY ||
         create_instance_result == VK_ERROR_INITIALIZATION_FAILED ||
         create_instance_result == VK_ERROR_LAYER_NOT_PRESENT ||
         create_instance_result == VK_ERROR_EXTENSION_NOT_PRESENT ||
         create_instance_result == VK_ERROR_INCOMPATIBLE_DRIVER;
}
