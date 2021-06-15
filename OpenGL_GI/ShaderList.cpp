#include "ShaderList.h"

ShaderList::ShaderList()
{
}

bool ShaderList::ShadersAreValid()
{
	bool result = true;
	for (Shader shader : shaders)
	{
		result &= shader.isValid;
	}
	return result;
}
