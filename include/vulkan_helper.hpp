#pragma once

#include <array>
#include <cstdint>
#include <expected>
#include <print>
#include <string_view>
#include <tuple>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "error.hpp"

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 800;
constexpr auto title = "hello_vulkan";

#ifndef NDEBUG
constexpr auto validation_layers_enabled = false;
#else
constexpr auto validation_layers_enabled = true;
#endif

constexpr std::array<const char*, 1> validation_layers = {
    "VK_LAYER_KHRONOS_validation"};

auto initialize_window(const uint32_t width, const uint32_t height,
                       std::string_view title) noexcept
    -> std::expected<GLFWwindow*, Error> {
  // initialize glfw
  if (glfwInit() == GLFW_FALSE) {
    return std::unexpected(Error::failed_to_initialize_glfw());
  }

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

auto get_glfw_extensions() noexcept
    -> std::tuple<const char* const*, uint32_t> {
  uint32_t glfw_extensions_count = 0;
  auto glfw_extensions =
      glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

  return std::tuple(glfw_extensions, glfw_extensions_count);
}

/// If a validation layer is missing the name of the layer is returned otherwise `std::nullopt`
auto check_validation_layer_support() -> std::optional<std::string_view> {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  std::vector<VkLayerProperties> available_validation_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count,
                                     available_validation_layers.data());

  for (auto ptr_layer_name : validation_layers) {
    const auto layer_name = std::string_view(ptr_layer_name);

    for (const auto& available_layer : available_validation_layers) {
      if (layer_name != available_layer.layerName) {
        return layer_name;
      }
    }
  }

  return std::nullopt;
}

constexpr auto get_vulkan_application_information() noexcept
    -> VkApplicationInfo {
  auto application_information = VkApplicationInfo{};
  application_information.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  application_information.pApplicationName = "hello vulkan";
  application_information.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  application_information.pEngineName = "No Engine";
  application_information.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  application_information.apiVersion = VK_API_VERSION_1_0;
  return application_information;
}

constexpr auto get_vulkan_create_instance_information(
    VkApplicationInfo* application_information,
    const char* const* glfw_extensions, uint32_t glfw_extensions_count) noexcept
    -> std::expected<VkInstanceCreateInfo, Error> {
  auto create_instance_information = VkInstanceCreateInfo{};
  create_instance_information.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_instance_information.pApplicationInfo = application_information;
  create_instance_information.ppEnabledExtensionNames = glfw_extensions;
  create_instance_information.enabledExtensionCount = glfw_extensions_count;

  if (validation_layers_enabled) {
    if (auto missing_validation_layer = check_validation_layer_support();
        missing_validation_layer.has_value()) {
      return std::unexpected(
          Error::validation_layer_not_found(*missing_validation_layer));
    }

    create_instance_information.enabledLayerCount =
        static_cast<uint32_t>(validation_layers.size());
    create_instance_information.ppEnabledLayerNames = validation_layers.data();
  }

  return create_instance_information;
}

auto get_vulkan_extensions() noexcept -> std::vector<VkExtensionProperties> {
  uint32_t extension_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

  std::vector<VkExtensionProperties> extensions(extension_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count,
                                         extensions.data());

  std::println("available vulkan extensions:");
  for (const auto& extension : extensions) {
    std::println("\t{}", extension.extensionName);
  }

  return extensions;
}

auto create_vulkan_instance() noexcept -> std::expected<VkInstance, Error> {
  // setup application metadata for vulkan
  auto application_information = get_vulkan_application_information();

  // get glfw extensions metadata
  auto [glfw_extensions, glfw_extensions_count] = get_glfw_extensions();

  // setup metadata for the vulkan instance.
  // Passing along the application and glfw extension metadata
  VkInstanceCreateInfo create_instance_information;
  if (auto result = get_vulkan_create_instance_information(
          &application_information, glfw_extensions, glfw_extensions_count);
      result.has_value()) {
    create_instance_information = *result;
  } else {
    return std::unexpected(result.error());
  }

  // create the vulkan instance
  VkInstance instance;
  auto result =
      vkCreateInstance(&create_instance_information, nullptr, &instance);

  // check for errors
  if (is_create_instance_error(result)) {
    return std::unexpected(Error::failed_to_initialize_vulkan(result));
  }

  // std::println("GLFW extensions");
  // for (uint32_t i = 0; i < glfw_extensions_count; ++i) {
  //   std::println("\t{}", glfw_extensions[i]);
  // }
  // std::println();
  // get_vulkan_extensions();

  return instance;
}

auto initialize_vulkan() noexcept -> std::expected<std::tuple<VkInstance>, Error> {
  VkInstance instance;
  if (auto result = create_vulkan_instance(); result.has_value()) {
    instance = *result;
  } else {
    return std::unexpected(result.error());
  }

  return std::tuple(instance);
}

auto initialize() noexcept
    -> std::expected<std::tuple<GLFWwindow*, VkInstance>, Error> {
  GLFWwindow* window = nullptr;
  if (auto result = initialize_window(WINDOW_WIDTH, WINDOW_HEIGHT, title);
      result.has_value()) {
    window = *result;
  } else {
    return std::unexpected(result.error());
  }

  VkInstance vulkan_instance;
  if (auto result = initialize_vulkan(); result.has_value()) {
    std::tie(vulkan_instance) = *result;
  } else {
    return std::unexpected(result.error());
  }

  check_validation_layer_support();

  return std::tuple(window, vulkan_instance);
}

auto deinitialize(GLFWwindow* window, VkInstance vulkan_instance) noexcept {
  vkDestroyInstance(vulkan_instance, nullptr);
  glfwDestroyWindow(window);
  glfwTerminate();
}
