#include "cPCH.h"
#include "cOpenGLObjects.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

cOpenGLDynamicVBO::cOpenGLDynamicVBO(size_t size) : m_MaxSize(size) {
	glGenBuffers(1, &m_BufferID);
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
}
void cOpenGLDynamicVBO::InsertData(size_t size, const void* data) {
	SE_ASSERT(size <= m_MaxSize, "Vertex Buffer Overflow!");
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	m_Size = size;
}

static GLenum ShaderTypeFromString(const std::string& type) {
	if (type == "vertex") return GL_VERTEX_SHADER;
	if (type == "geometry") return GL_GEOMETRY_SHADER;
	if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

	SE_ASSERT(false, "Unknown shader type!");
		return 0;
}
cOpenGLShader::cOpenGLShader(const std::string& filepath)
{
	//Reads shader code
	std::string srccode;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	SE_ASSERT(in, "'%s' could not be read!", filepath);
	in.seekg(0, std::ios::end);
	srccode.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&srccode[0], srccode.size());
	in.close();

	//Seperates code for different shaders
	std::unordered_map<GLenum, std::string> shaderSources;

	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	size_t pos = srccode.find(typeToken, 0);
	while (pos != std::string::npos)
	{
		size_t eol = srccode.find_first_of("\r\n", pos);
		SE_ASSERT(eol != std::string::npos, "Syntax error");
		size_t begin = pos + typeTokenLength + 1;
		std::string type = srccode.substr(begin, eol - begin);
		SE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

		size_t nextLinePos = srccode.find_first_not_of("\r\n", eol);
		pos = srccode.find(typeToken, nextLinePos);
		shaderSources[ShaderTypeFromString(type)] =
			srccode.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? srccode.size() - 1 : nextLinePos));
	}

	//Creates Shader
	GLuint program = glCreateProgram();
	SE_ASSERT(shaderSources.size() <= 3, "Does not support more than three shaders");
	GLuint glShaderIDs[3];
	size_t glShaderIDCount = 0;
	for (auto& kv : shaderSources) {
		GLenum type = kv.first;
		const GLchar* source = kv.second.c_str();

		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, 0);
		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shader);
			for (size_t sh = 0; sh < glShaderIDCount; sh++) {
				glDeleteShader(glShaderIDs[sh]);
			}

			char* infoLogCh = new char[36 + maxLength];
			switch (type) {
			case GL_VERTEX_SHADER:
				sprintf(infoLogCh, "%s: %s", "Vertex shader failed to compile", infoLog.data());
				break;
			case GL_GEOMETRY_SHADER:
				sprintf(infoLogCh, "%s: %s", "Geometry shader failed to compile", infoLog.data());
				break;
			case GL_FRAGMENT_SHADER:
				sprintf(infoLogCh, "%s: %s", "Fragment shader failed to compile", infoLog.data());
				break;
			}
			SE_ASSERT(false, infoLogCh);
			delete[] infoLogCh;
			break;
		}

		glAttachShader(program, shader);
		glShaderIDs[glShaderIDCount++] = shader;
	}
	glLinkProgram(program);
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(program);
		for (size_t sh = 0; sh < glShaderIDCount; sh++) {
			glDeleteShader(glShaderIDs[sh]);
		}

		char* infoLogCh = new char[25 + maxLength];
		sprintf(infoLogCh, "Failed to link shaders: %s", infoLog.data());
		SE_ASSERT(false, infoLogCh);
		delete[] infoLogCh;
		return;
	}

	for (size_t sh = 0; sh < glShaderIDCount; sh++) {
		glDetachShader(program, glShaderIDs[sh]);
	}
	m_ShaderID = program;
}

static int glSizeof(GLenum type) {
	switch (type) {
	case GL_BYTE: return 1;
	case GL_UNSIGNED_BYTE: return 1;
	case GL_SHORT: return 2;
	case GL_UNSIGNED_SHORT: return 2;
	case GL_INT: return 4;
	case GL_UNSIGNED_INT: return 4;
	case GL_FIXED: return 4;
	case GL_HALF_FLOAT: return 2;
	case GL_FLOAT: return 4;
	case GL_DOUBLE: return 8;
	}
}

cOpenGLStaticIBO::cOpenGLStaticIBO(size_t size, const void* data, GLenum type) : m_Type(type) {
	glGenBuffers(1, &m_BufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	switch (type) {
	case GL_UNSIGNED_INT:
		m_Count = size / sizeof(GLuint);
		break;
	case GL_UNSIGNED_SHORT:
		m_Count = size / sizeof(GLushort);
		break;
	case GL_UNSIGNED_BYTE:
		m_Count = size / sizeof(GLubyte);
		break;
	default:
		SE_ASSERT(false, "Invalid Element Buffer Type!");
	}
}

cOpenGLVAO::cOpenGLVAO()
{
	glGenVertexArrays(1, &m_ArrayID);
}
cOpenGLVAO::~cOpenGLVAO()
{
	glDeleteVertexArrays(1, &m_ArrayID);
}
void cOpenGLVAO::SetLayout(std::initializer_list<cOpenGLVertexAttribute> layout)
{
	int stride = 0, offset = 0, index = 0;
	for (auto& att : layout) {
		stride += att.size*glSizeof(att.type);
	}
	m_Stride = stride;
	
	glBindVertexArray(m_ArrayID);
	for (auto& att : layout) {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index++, att.size, att.type, false, stride, (const void*)offset);
		offset += att.size * glSizeof(att.type);
	}
}

void cOpenGLVAO::BindVertexBuffer(std::shared_ptr<cOpenGLDynamicVBO>& vbo)
{
	glBindVertexArray(m_ArrayID);
	vbo->Bind();
	m_vboRef = vbo;
}
void cOpenGLVAO::DrawElements(std::shared_ptr<cOpenGLStaticIBO>& ibo, GLenum mode, size_t count)
{
	glBindVertexArray(m_ArrayID);
	SE_ASSERT(m_vboRef, "Vertex Array Object needs a Vertex Buffer Object!");
	SE_ASSERT(count <= ibo->GetCount(), "Index Buffer Overflow!");
	ibo->Bind();
	glDrawElements(mode, count, ibo->GetType(), NULL);
}
void cOpenGLVAO::DrawArrays(GLenum mode) {
	glBindVertexArray(m_ArrayID);
	SE_ASSERT(m_vboRef, "Vertex Array Object needs a Vertex Buffer Object!");
	size_t DrawCount = m_vboRef->GetSize() / m_Stride;
	glDrawArrays(mode, 0, DrawCount);
}

cOpenGLTexture::cOpenGLTexture(const char* filepath)
{
	unsigned char* data = stbi_load(filepath, &m_Width, &m_Height, &m_Channels, 0);

	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	GLint internalformat = 0;
	GLint format = 0;
	switch (m_Channels) {
	case 4:
		internalformat = GL_RGBA8;
		format = GL_RGBA;
		break;
	case 3:
		internalformat = GL_RGB8;
		format = GL_RGB;
		break;
	case 2:
		internalformat = GL_RG8;
		format = GL_RG;
		break;
	case 1:
		internalformat = GL_R8;
		format = GL_RED;
		break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
