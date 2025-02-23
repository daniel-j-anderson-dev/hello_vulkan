#include <print>
#include <tuple>

#include <GLFW/glfw3.h>

#include "error.hpp"
#include "vulkan_helper.hpp"

auto main() -> int {
  GLFWwindow* window = nullptr;
  if (auto result = initialize(); result.has_value()) {
    std::tie(window) = *result;
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
