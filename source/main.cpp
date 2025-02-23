#include <print>
#include <tuple>

#include <GLFW/glfw3.h>

#include "vulkan_helper.hpp"

auto main() -> int {
  GLFWwindow* window = nullptr;
  VkInstance vulkan_instance;
  if (auto result = initialize(); result.has_value()) {
    std::tie(window, vulkan_instance) = *result;
  } else {
    std::println("An error occurred during initialization: {}",
                 result.error().to_string());
    return -1;
  }

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  deinitialize(window);

  return 0;
}
