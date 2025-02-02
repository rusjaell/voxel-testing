#include "Shader.h"

#include "OpenGL.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertex_file_path, const char* fragment_file_path)
	: _compiled(false), _handle(0)
{
	// Create the shaders
	GLuint vertexShaderId;
	GL_CALL(vertexShaderId = glCreateShader(GL_VERTEX_SHADER));

	GLuint fragmentShaderId;
	GL_CALL(fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER));

	// Read the Vertex Shader code from the file
	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(vertex_file_path, std::ios::in);
	if (!vertexShaderStream.is_open()) {
		return;
	}

	std::stringstream sstr;
	sstr << vertexShaderStream.rdbuf();
	vertexShaderCode = sstr.str();
	vertexShaderStream.close();

	// Read the Fragment Shader code from the file
	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(fragment_file_path, std::ios::in);
	if (fragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << fragmentShaderStream.rdbuf();
		fragmentShaderCode = sstr.str();
		fragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	char const* vertexSourcePointer = vertexShaderCode.c_str();
	GL_CALL(glShaderSource(vertexShaderId, 1, &vertexSourcePointer, NULL));
	GL_CALL(glCompileShader(vertexShaderId));

	GL_CALL(glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &Result));
	GL_CALL(glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength));
	if (InfoLogLength > 0) {
		std::vector<char> vertexShaderErrorMessage(InfoLogLength + 1);
		GL_CALL(glGetShaderInfoLog(vertexShaderId, InfoLogLength, NULL, &vertexShaderErrorMessage[0]));
		printf("%s\n", &vertexShaderErrorMessage[0]);
	}

	char const* fragmentSourcePointer = fragmentShaderCode.c_str();
	GL_CALL(glShaderSource(fragmentShaderId, 1, &fragmentSourcePointer, NULL));
	GL_CALL(glCompileShader(fragmentShaderId));

	GL_CALL(glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &Result));
	GL_CALL(glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength));
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		GL_CALL(glGetShaderInfoLog(fragmentShaderId, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]));
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	GL_CALL(_handle = glCreateProgram());

	GL_CALL(glAttachShader(_handle, vertexShaderId));
	GL_CALL(glAttachShader(_handle, fragmentShaderId));
	GL_CALL(glLinkProgram(_handle));

	GL_CALL(glGetProgramiv(_handle, GL_LINK_STATUS, &Result));
	GL_CALL(glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &InfoLogLength));
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		GL_CALL(glGetProgramInfoLog(_handle, InfoLogLength, NULL, &ProgramErrorMessage[0]));
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	GL_CALL(glDetachShader(_handle, vertexShaderId));
	GL_CALL(glDetachShader(_handle, fragmentShaderId));

	GL_CALL(glDeleteShader(vertexShaderId));
	GL_CALL(glDeleteShader(fragmentShaderId));

	_compiled = true;
}

bool Shader::IsCompiled() const {
	return _compiled;
}

void Shader::Use() const
{
	GL_CALL(glUseProgram(_handle));
}

bool Shader::HasUniform(const std::string& name)
{
	int location = GetUniformLocation(name); // glGetUniformLocation(handle_, name.c_str());
	return location != -1;
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLuint location = GetUniformLocation(name);
	GL_CALL(glUniform1i(location, value));
}

void Shader::SetUniform1iv(const std::string& name, int* values, int count)
{
	GLuint location = GetUniformLocation(name);
	GL_CALL(glUniform1iv(location, count, values));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	GLuint location = GetUniformLocation(name);
	GL_CALL(glUniform1f(location, value));
}

void Shader::SetUniform3f(const std::string& name, const glm::vec3& value)
{
	GLuint location = GetUniformLocation(name);
	GL_CALL(glUniform3f(location, value.x, value.y, value.z));
}

void Shader::SetUniform4f(const std::string& name, const glm::vec4& value)
{
	GLuint location = GetUniformLocation(name);
	GL_CALL(glUniform4f(location, value.x, value.y, value.z, value.w));
}

void Shader::SetUniform4f(const std::string& name, float x, float y, float z, float w)
{
	GLuint location = GetUniformLocation(name);
	GL_CALL(glUniform4f(location, x, y, z, w));
}

void Shader::SetUniformMatrix4f(const std::string& name, const glm::mat4& matrix)
{
	GLuint location = GetUniformLocation(name);
	GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

int Shader::GetUniformLocation(const std::string& name) {
	if (_uniformCache.find(name) != _uniformCache.end()) {
		return _uniformCache[name];
	}

	int location;
	GL_CALL(location = glGetUniformLocation(_handle, name.c_str()));
	_uniformCache[name] = location;
	return location;
}
