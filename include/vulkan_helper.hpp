#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <expected>
#include <string_view>
#include <tuple>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "error.hpp"

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 800;

auto initialize_window(const uint32_t width, const uint32_t height,
                       std::string_view title)
    -> std::expected<GLFWwindow*, Error> {
  // tell glfw not to create an OpenGL context
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // we'll come back to resizing windows later
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  auto window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

  if (nullptr == window) {
    return std::unexpected(Error::failed_to_create_window());
  }

  return window;
}

constexpr auto get_application_information() -> VkApplicationInfo {
  auto application_information = VkApplicationInfo{};
  application_information.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  application_information.pApplicationName = "hello vulkan";
  application_information.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  application_information.pEngineName = "No Engine";
  application_information.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  application_information.apiVersion = VK_API_VERSION_1_0;
  return application_information;
}

auto get_glfw_extensions() -> std::tuple<const char* const*, uint32_t> {
  uint32_t glfw_extensions_count = 0;
  auto glfw_extensions =
      glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

  return std::tuple(glfw_extensions, glfw_extensions_count);
}

constexpr auto get_vulkan_create_instance_information(
    VkApplicationInfo* application_information,
    const char* const* glfw_extensions, uint32_t glfw_extensions_count)
    -> VkInstanceCreateInfo {
  auto create_instance_information = VkInstanceCreateInfo{};
  create_instance_information.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_instance_information.pApplicationInfo = application_information;
  create_instance_information.ppEnabledExtensionNames = glfw_extensions;
  create_instance_information.enabledExtensionCount = glfw_extensions_count;
  create_instance_information.enabledLayerCount = 0;
}

auto initialize_vulkan() -> std::expected<VkInstance, Error> {
  // setup application metadata for vulkan
  auto application_information = get_application_information();

  // get glfw extensions metadata
  auto [glfw_extensions, glfw_extensions_count] = get_glfw_extensions();

  // setup metadata for the vulkan instance.
  // Passing along the application and glfw extension metadata
  auto create_instance_information = get_vulkan_create_instance_information(
      &application_information, glfw_extensions, glfw_extensions_count);

  // dont use allocation call backs for now
  const VkAllocationCallbacks* allocator = nullptr;

  // create the vulkan instance
  VkInstance instance;
  auto result =
      vkCreateInstance(&create_instance_information, allocator, &instance);

  // check for errors
  if (is_create_instance_error(result)) {
    return std::unexpected(Error::failed_to_initialize_vulkan(result));
  }

  return instance;
}

auto initialize() -> std::expected<std::tuple<GLFWwindow*, VkInstance>, Error> {
  GLFWwindow* window = nullptr;
  if (auto result =
          initialize_window(WINDOW_WIDTH, WINDOW_HEIGHT, "hello_vulkan");
      result.has_value()) {
    window = *result;
  } else {
    return std::unexpected(result.error());
  }

  VkInstance vulkan_instance;
  if (auto result = initialize_vulkan(); result.has_value()) {
    vulkan_instance = *result;
  } else {
    return std::unexpected(result.error());
  }

  return std::tuple(window, vulkan_instance);
}

auto deinitialize(GLFWwindow* window) {
  glfwDestroyWindow(window);
  glfwTerminate();
}