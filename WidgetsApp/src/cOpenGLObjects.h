#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <unordered_map>

using sampler2D = int;

class cOpenGLDynamicVBO
{
public:
	cOpenGLDynamicVBO(size_t size);
	~cOpenGLDynamicVBO() { glDeleteBuffers(1, &m_BufferID); }
	inline void Bind() { glBindBuffer(GL_ARRAY_BUFFER, m_BufferID); }
	void InsertData(size_t size, const void* data);

	inline size_t GetSize() { return m_Size; }
	inline size_t GetMaxSize() { return m_MaxSize; }
private:
	GLuint m_BufferID = 0;
	size_t m_MaxSize = 0;
	size_t m_Size = 0;
};

class cOpenGLShader
{
public:
	cOpenGLShader(const std::string& filepath);
	~cOpenGLShader() { glDeleteProgram(m_ShaderID); }
	void Use() const { glUseProgram(m_ShaderID); }

	void SetUniformMat4(const char* u_name, const glm::mat4& mat) {
		glUseProgram(m_ShaderID);
		GLint uniformID = glGetUniformLocation(m_ShaderID, u_name);
		SE_ASSERT(uniformID != -1, "Uniform '" + std::string(u_name) + "' could not be found!");
		glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(mat));

		if (m_UniformCache.find(u_name) == m_UniformCache.end()) {
			m_UniformCache[u_name] = uniformID;
		}
	}
	void SetUniformSampler2D(const char* u_name, sampler2D val) {
		glUseProgram(m_ShaderID);
		GLint uniformID = glGetUniformLocation(m_ShaderID, u_name);
		SE_ASSERT(uniformID != -1, "Uniform '" + std::string(u_name) + "' could not be found!");
		glUniform1i(uniformID, val);

		if (m_UniformCache.find(u_name) == m_UniformCache.end()) {
			m_UniformCache[u_name] = uniformID;
		}
	}
	void SetUniformVec2(const char* u_name, float v0, float v1) {
		glUseProgram(m_ShaderID);
		GLint uniformID = glGetUniformLocation(m_ShaderID, u_name);
		SE_ASSERT(uniformID != -1, "Uniform '" + std::string(u_name) + "' could not be found!");
		glUniform2f(uniformID, v0, v1);

		if (m_UniformCache.find(u_name) == m_UniformCache.end()) {
			m_UniformCache[u_name] = uniformID;
		}
	}
private:
	GLuint m_ShaderID = 0;
	std::unordered_map<std::string, GLuint> m_UniformCache;
};

class cOpenGLStaticIBO
{
public:
	cOpenGLStaticIBO(size_t size, const void* data, GLenum type);
	~cOpenGLStaticIBO() { glDeleteBuffers(1, &m_BufferID); }
	inline void Bind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}
	inline GLenum GetType() { return m_Type; }
	inline size_t GetCount() { return m_Count; }
private:
	GLuint m_BufferID = 0;
	size_t m_Count = 0;
	GLenum m_Type = 0;
};

class cOpenGLVAO
{
private:
	struct cOpenGLVertexAttribute
	{
		GLint size;
		GLenum type;
	};
public:
	cOpenGLVAO();
	~cOpenGLVAO();
	void SetLayout(std::initializer_list<cOpenGLVertexAttribute> layout);
	inline void Bind() { glBindVertexArray(m_ArrayID); }
	void BindVertexBuffer(std::shared_ptr<cOpenGLDynamicVBO>& vbo);
	void DrawElements(std::shared_ptr<cOpenGLStaticIBO>& ibo, GLenum mode, size_t count);
	void DrawArrays(GLenum mode);
private:
	GLuint m_ArrayID = 0;
	size_t m_Stride = 0;
	std::shared_ptr<cOpenGLDynamicVBO> m_vboRef;
};

class cOpenGLTexture {
public:
	cOpenGLTexture(const char* filepath);
	void Bind(sampler2D slot = 0) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}
private:
	GLuint m_TextureID = 0;
	int m_Width = 0, m_Height = 0, m_Channels = 0;
};