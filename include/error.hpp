#pragma once

#include <vulkan/vulkan_core.h>

#include <format>
#include <optional>
#include <string>
#include <string_view>

constexpr auto vkresult_to_string(VkResult result) -> std::string_view;

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

  auto to_string(this const Error& self) noexcept -> std::string {
    switch (self._kind) {
      case ErrorKind::FailedToCreateWindow:
        return "Failed to initialize GLFW Window";
      case ErrorKind::FailedToInitializeVulkan:
        return std::format(
            "Failed to initialize Vulkan: error number: {} ({})",
            static_cast<uint64_t>(*self.failed_to_initialize_vulkan_data),
            vkresult_to_string(*self.failed_to_initialize_vulkan_data));
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

constexpr auto vkresult_to_string(VkResult result) -> std::string_view {
  switch (result) {
    case (VK_SUCCESS):
      return "VK_SUCCESS";
    case (VK_NOT_READY):
      return "VK_NOT_READY";
    case (VK_TIMEOUT):
      return "VK_TIMEOUT";
    case (VK_EVENT_SET):
      return "VK_EVENT_SET";
    case (VK_EVENT_RESET):
      return "VK_EVENT_RESET";
    case (VK_INCOMPLETE):
      return "VK_INCOMPLETE";
    case (VK_ERROR_OUT_OF_HOST_MEMORY):
      return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case (VK_ERROR_OUT_OF_DEVICE_MEMORY):
      return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case (VK_ERROR_INITIALIZATION_FAILED):
      return "VK_ERROR_INITIALIZATION_FAILED";
    case (VK_ERROR_DEVICE_LOST):
      return "VK_ERROR_DEVICE_LOST";
    case (VK_ERROR_MEMORY_MAP_FAILED):
      return "VK_ERROR_MEMORY_MAP_FAILED";
    case (VK_ERROR_LAYER_NOT_PRESENT):
      return "VK_ERROR_LAYER_NOT_PRESENT";
    case (VK_ERROR_EXTENSION_NOT_PRESENT):
      return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case (VK_ERROR_FEATURE_NOT_PRESENT):
      return "VK_ERROR_FEATURE_NOT_PRESENT";
    case (VK_ERROR_INCOMPATIBLE_DRIVER):
      return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case (VK_ERROR_TOO_MANY_OBJECTS):
      return "VK_ERROR_TOO_MANY_OBJECTS";
    case (VK_ERROR_FORMAT_NOT_SUPPORTED):
      return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case (VK_ERROR_FRAGMENTED_POOL):
      return "VK_ERROR_FRAGMENTED_POOL";
    case (VK_ERROR_UNKNOWN):
      return "VK_ERROR_UNKNOWN";
    case (VK_ERROR_OUT_OF_POOL_MEMORY):
      return "VK_ERROR_OUT_OF_POOL_MEMORY";
    case (VK_ERROR_INVALID_EXTERNAL_HANDLE):
      return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
    case (VK_ERROR_FRAGMENTATION):
      return "VK_ERROR_FRAGMENTATION";
    case (VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS):
      return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
    case (VK_PIPELINE_COMPILE_REQUIRED):
      return "VK_PIPELINE_COMPILE_REQUIRED";
    case (VK_ERROR_NOT_PERMITTED):
      return "VK_ERROR_NOT_PERMITTED";
    case (VK_ERROR_INVALID_SHADER_NV):
      return "VK_ERROR_INVALID_SHADER_NV";
    case (VK_RESULT_MAX_ENUM):
      return "VK_RESULT_MAX_ENUM";
    default:
      return "UNRECOGNIZED VkResult VALUE THIS IS A BUG";
  }
}
