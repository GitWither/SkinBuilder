#include <iostream>
#include <GLFW/glfw3.h>
#include "glm/vec2.hpp"
#include "Core/Core.hpp"
#include <vulkan/vulkan.h>

#include "Core/SkinBuilder.hpp"

int main() {

	SkinBuilder::SkinBuilder skinBuilder(1280, 720, "SkinBuilder");
	skinBuilder.Run();
}