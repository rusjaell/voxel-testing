#ifndef SHADER_H
#define SHADER_H

class Shader
{
public:
	Shader(const char* vertex_file_path, const char* fragment_file_path);
	virtual ~Shader() = default;

	bool IsCompiled() const;

	void Use() const;

	bool HasUniform(const std::string& name);

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1iv(const std::string& name, int* values, int count);

	void SetUniform1f(const std::string& name, float value);
	void SetUniform3f(const std::string& name, const glm::vec3& value);
	void SetUniform4f(const std::string& name, const glm::vec4& value);
	void SetUniform4f(const std::string& name, float x, float y, float z, float w);
	void SetUniformMatrix4f(const std::string& name, const glm::mat4& matrix);

private:
	int GetUniformLocation(const std::string& name);

private:
	unsigned int _handle;
	bool _compiled;

	std::unordered_map<std::string, int> _uniformCache;
};

#endif // !SHADER_H