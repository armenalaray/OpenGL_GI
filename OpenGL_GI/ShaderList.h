#pragma once
#include "Shader.h"

#include <vector>
class ShaderList
{
public:
	std::vector<Shader> shaders;
	ShaderList();
	bool ShadersAreValid();
};