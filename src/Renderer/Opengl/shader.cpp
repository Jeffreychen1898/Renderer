#include "Shader.hpp"

namespace Renderer
{
	Renderer::Shader* Shader::s_currentShader = nullptr;
	Shader::Shader(bool _autoBind)
		: m_program(0), m_vao(0), m_ibo(0), m_attribOffset(0), m_window(nullptr), m_initialized(false),
		m_autoBind(_autoBind)
	{
	}

	void Shader::attach(Renderer::Render* _renderer)
	{
		if(m_window)
			throw Renderer::ShaderOperationRejected("Shader can only be attached to a Renderer::Render class once!");

		m_window = _renderer->getWindow();
	}

	void Shader::create(const char* _vertexCode, const char* _fragmentCode, bool _checkErrs)
	{
		assertValidRenderer();
		assertCurrentContext();

		// create the shader program
		GLuint fragment_shader = createShader(_fragmentCode, GL_FRAGMENT_SHADER, _checkErrs);
		GLuint vertex_shader = createShader(_vertexCode, GL_VERTEX_SHADER, _checkErrs);

		m_program = glCreateProgram();
		glAttachShader(m_program, vertex_shader);
		glAttachShader(m_program, fragment_shader);
		glLinkProgram(m_program);

		// create the vao
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		
		glGenBuffers(1, &m_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

		bind();

		if(!_checkErrs)
		{
			glDeleteShader(vertex_shader);
			glDeleteShader(fragment_shader);

			m_initialized = true;
			return;
		}

		GLint success;
		GLchar message[512];

		glGetProgramiv(m_program, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(m_program, sizeof(message), nullptr, message);
			throw Renderer::ShaderCompilationException("Shader program linking failed: " + std::string(message));
		}

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		m_initialized = true;
	}

	void Shader::createFromFile(const char* _vertexPath, const char* _fragmentPath, bool _checkErrs)
	{
		std::string each_line;
		std::string vertex_code = "";
		std::string fragment_code = "";

		// load the vertex shader
		std::ifstream vertex_file;
		vertex_file.open(_vertexPath);
		if(!vertex_file.is_open())
			throw Renderer::FileNotFoundException("Shader file cannot be opened: " + std::string(_vertexPath) + "!");

		while(std::getline(vertex_file, each_line))
			vertex_code += each_line + "\n";

		// load the fragment shader
		std::ifstream fragment_file;
		fragment_file.open(_fragmentPath);
		if(!fragment_file.is_open())
			throw Renderer::FileNotFoundException("Shader file cannot be opened: " + std::string(_fragmentPath) + "!");

		while(std::getline(fragment_file, each_line))
			fragment_code += each_line + "\n";

		create(vertex_code.c_str(), fragment_code.c_str(), _checkErrs);
	}

	void Shader::bind()
	{
		assertValidRenderer();
		assertCurrentContext();

		if(isBound())
			return;

		glUseProgram(m_program);
		glBindVertexArray(m_vao);

		s_currentShader = this;
	}

	void Shader::verticesData(const void* _vertices, unsigned int _arrBitSize)
	{
		assertValidRenderer();
		assertCurrentContext();
		assertShaderBound("verticesData()");

		for(const VBO& each_vbo : m_vbos)
		{
			glBindBuffer(GL_ARRAY_BUFFER, each_vbo.vbo);
			glBufferData(GL_ARRAY_BUFFER, _arrBitSize, _vertices, GL_DYNAMIC_DRAW);
		}
	}

	void Shader::indicesData(unsigned int* _indices, unsigned int _indicesCount)
	{
		assertValidRenderer();
		assertCurrentContext();
		assertShaderBound("indicesData()");

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indicesCount, _indices, GL_DYNAMIC_DRAW);
	}

	GLuint Shader::createShader(const char* _sourcecode, GLenum _shaderType, bool _checkErrs)
	{
		GLuint shader = glCreateShader(_shaderType);
		glShaderSource(shader, 1, &_sourcecode, nullptr);
		glCompileShader(shader);

		if(!_checkErrs)
			return shader;

		GLint success;
		GLchar err_msg[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(shader, sizeof(err_msg), nullptr, err_msg);

			std::string shader_type;
			switch(_shaderType)
			{
				case GL_FRAGMENT_SHADER:
					shader_type = "Fragment Shader";
					break;
				case GL_VERTEX_SHADER:
					shader_type = "Vertex Shader";
					break;
				default:
					shader_type = "Unknown Shader";
					break;
			}

			throw Renderer::ShaderCompilationException(shader_type + " failed to compile: " + std::string(err_msg));
			return 0;
		}

		return shader;
	}

	void Shader::vertexAttribAdd(unsigned int _location, AttribType _attribType)
	{
		assertValidRenderer();
		assertCurrentContext();

		int attribute_size = getAttribSize(_attribType);
		AttribDataType attrib_datatype = getAttribDataType(_attribType);
		m_vbos.push_back({ 0, attribute_size, _location, attrib_datatype });
	}

	void Shader::vertexAttribsEnable()
	{
		assertValidRenderer();
		assertCurrentContext();
		assertShaderBound("vertexAttribsEnable()");

		unsigned int vertex_size = 0;
		for(VBO& vbo : m_vbos)
		{
			if(vbo.type == AttribDataType::FLOAT)
				vertex_size += vbo.size * sizeof(float);
			else if(vbo.type == AttribDataType::INT)
				vertex_size += vbo.size * sizeof(int);
		}

		for(VBO& vbo : m_vbos)
		{
			glGenBuffers(1, &vbo.vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo.vbo);

			if(vbo.type == AttribDataType::FLOAT)
			{
				glVertexAttribPointer(
					vbo.location,
					vbo.size,
					GL_FLOAT,
					GL_FALSE,
					vertex_size,
					(const void*)(m_attribOffset)
				);
				m_attribOffset += sizeof(float) * vbo.size;
			} else if(vbo.type == AttribDataType::INT)
			{
				glVertexAttribIPointer(
					vbo.location,
					vbo.size,
					GL_INT,
					vertex_size,
					(const void*)(m_attribOffset)
				);
				m_attribOffset += sizeof(int) * vbo.size;
			}

			glEnableVertexAttribArray(vbo.location);
		}
	}

	void Shader::uniformAdd(const char* _uniformName, UniformType _type)
	{
		assertValidRenderer();
		assertCurrentContext();
		assertShaderBound("uniformAdd()");

		if(m_uniformLocation.find(_uniformName) != m_uniformLocation.end())
			return;

		int location = glGetUniformLocation(m_program, _uniformName);

		UniformObject uniform_obj = { _uniformName, location, _type };

		m_uniformLocation.insert({ _uniformName, uniform_obj });
	}

	void Shader::setUniformInt(const char* _name, int _data)
	{
		assertValidRenderer();
		assertCurrentContext();
		assertShaderBound("setUniformInt()");

		std::unordered_map<const char*, UniformObject>::iterator it = m_uniformLocation.find(_name);
		if(it == m_uniformLocation.end())
			return;
		if(it->second.type != UniformType::INT)
		{
			std::string exception_message = "The uniform \"" + std::string(_name) + "\" is not an int! ";
			exception_message += "Please call the appropriate method!";
			throw Renderer::InvalidType(exception_message);
		}

		glUniform1i(it->second.location, _data);
	}

	void Shader::setUniformInt(const char* _name, const int* _data)
	{
		assertValidRenderer();
		assertCurrentContext();
		assertShaderBound("setUniformInt()");

		std::unordered_map<const char*, UniformObject>::iterator it = m_uniformLocation.find(_name);
		if(it == m_uniformLocation.end())
			return;
		if(it->second.type != UniformType::IVEC2 && it->second.type != UniformType::IVEC3 &&
				it->second.type != UniformType::IVEC4)
		{
			std::string exception_message = "The uniform \"" + std::string(_name) + "\" is not an int vector! ";
			exception_message += "Please call the appropriate method!";
			throw Renderer::InvalidType(exception_message);
		}

		switch(it->second.type)
		{
			case UniformType::IVEC2:
				glUniform2i(it->second.location, _data[0], _data[1]);
				break;
			case UniformType::IVEC3:
				glUniform3i(it->second.location, _data[0], _data[1], _data[2]);
				break;
			case UniformType::IVEC4:
				glUniform4i(it->second.location, _data[0], _data[1], _data[2], _data[3]);
				break;
			default:
				break;
		}
	}

	void Shader::setUniformInt(const char* _name, int _count, const int* _data)
	{
		assertValidRenderer();
		assertCurrentContext();
		assertShaderBound("setUniformInt()");

		std::unordered_map<const char*, UniformObject>::iterator it = m_uniformLocation.find(_name);
		if(it == m_uniformLocation.end())
			return;
		if(it->second.type != UniformType::INT_ARR)
		{
			std::string exception_message = "The uniform \"" + std::string(_name) + "\" is not an int array! ";
			exception_message += "Please call the appropriate method!";
			throw Renderer::InvalidType(exception_message);
		}

		glUniform1iv(it->second.location, _count, _data);
	}

	void Shader::setUniformFloat(const char* _name, float _data)
	{
		assertValidRenderer();
		assertCurrentContext();
		assertShaderBound("setUniformFloat()");

		std::unordered_map<const char*, UniformObject>::iterator it = m_uniformLocation.find(_name);
		if(it == m_uniformLocation.end())
			return;
		if(it->second.type != UniformType::FLOAT)
		{
			std::string exception_message = "The uniform \"" + std::string(_name) + "\" is not a float! ";
			exception_message += "Please call the appropriate method!";
			throw Renderer::InvalidType(exception_message);
		}

		glUniform1f(it->second.location, _data);
	}

	void Shader::setUniformFloat(const char* _name, const float* _data)
	{
		assertValidRenderer();
		assertCurrentContext();
		assertShaderBound("setUniformFloat()");

		std::unordered_map<const char*, UniformObject>::iterator it = m_uniformLocation.find(_name);
		if(it == m_uniformLocation.end())
			return;
		if(it->second.type != UniformType::VEC2 && it->second.type != UniformType::VEC3 &&
				it->second.type != UniformType::VEC4)
		{
			std::string exception_message = "The uniform \"" + std::string(_name) + "\" is not a float vector! ";
			exception_message += "Please call the appropriate method!";
			throw Renderer::InvalidType(exception_message);
		}

		switch(it->second.type)
		{
			case UniformType::VEC2:
				glUniform2f(it->second.location, _data[0], _data[1]);
				break;
			case UniformType::VEC3:
				glUniform3f(it->second.location, _data[0], _data[1], _data[2]);
				break;
			case UniformType::VEC4:
				glUniform4f(it->second.location, _data[0], _data[1], _data[2], _data[3]);
				break;
			default:
				break;
		}
	}

	void Shader::setUniformFloat(const char* _name, int _count, const float* _data)
	{
		assertValidRenderer();
		assertCurrentContext();
		assertShaderBound("setUniformFloat()");

		std::unordered_map<const char*, UniformObject>::iterator it = m_uniformLocation.find(_name);
		if(it == m_uniformLocation.end())
			return;
		if(it->second.type != UniformType::FLOAT_ARR)
		{
			std::string exception_message = "The uniform \"" + std::string(_name) + "\" is not a float array! ";
			exception_message += "Please call the appropriate method!";
			throw Renderer::InvalidType(exception_message);
		}

		glUniform1fv(it->second.location, _count, _data);
	}

	void Shader::setUniformMatrix(const char* _name, const float* _data)
	{
		assertValidRenderer();
		assertCurrentContext();
		assertShaderBound("setUniformMatrix()");

		std::unordered_map<const char*, UniformObject>::iterator it = m_uniformLocation.find(_name);
		if(it == m_uniformLocation.end())
			return;
		if(it->second.type != UniformType::MAT2 && it->second.type != UniformType::MAT3 &&
				it->second.type != UniformType::MAT4)
		{
			std::string exception_message = "The uniform \"" + std::string(_name) + "\" is not a matrix! ";
			exception_message += "Please call the appropriate method!";
			throw Renderer::InvalidType(exception_message);
		}

		switch(it->second.type)
		{
			case UniformType::MAT2:
				glUniformMatrix2fv(it->second.location, 1, GL_TRUE, _data);
				break;
			case UniformType::MAT3:
				glUniformMatrix3fv(it->second.location, 1, GL_TRUE, _data);
				break;
			case UniformType::MAT4:
				glUniformMatrix4fv(it->second.location, 1, GL_TRUE, _data);
				break;
			default:
				break;
		}
	}

	int Shader::getAttribSize(const AttribType& _type)
	{
		switch(_type)
		{
			case AttribType::VEC2:
				return 2;
				break;
			case AttribType::VEC3:
				return 3;
				break;
			case AttribType::VEC4:
				return 4;
				break;
			case AttribType::IVEC2:
				return 2;
				break;
			case AttribType::IVEC3:
				return 3;
				break;
			case AttribType::IVEC4:
				return 4;
				break;
			case AttribType::FLOAT:
				return 1;
				break;
			case AttribType::INT:
				return 1;
				break;
		}

		return 0;
	}

	AttribDataType Shader::getAttribDataType(const AttribType& _type)
	{
		switch(_type)
		{
			case AttribType::VEC2:
				return AttribDataType::FLOAT;
				break;
			case AttribType::VEC3:
				return AttribDataType::FLOAT;
				break;
			case AttribType::VEC4:
				return AttribDataType::FLOAT;
				break;
			case AttribType::IVEC2:
				return AttribDataType::INT;
				break;
			case AttribType::IVEC3:
				return AttribDataType::INT;
				break;
			case AttribType::IVEC4:
				return AttribDataType::INT;
				break;
			case AttribType::FLOAT:
				return AttribDataType::FLOAT;
				break;
			case AttribType::INT:
				return AttribDataType::INT;
				break;
		}

		return AttribDataType::INT;
	}

	void Shader::assertValidRenderer()
	{
		if(m_window)
			return;

		throw Renderer::ShaderOperationRejected("Shader class must be attached to a valid Renderer::Render class before using!");
	}

	void Shader::assertCurrentContext()
	{
		if(m_window->isCurrentContext())
			return;

		if(m_window->willAutoMakeCurrent())
		{
			m_window->makeCurrent();
			return;
		}

		throw Renderer::InvalidWindowContext("The corresponding window must be made current first!");
	}

	void Shader::assertShaderBound(const char* _func)
	{
		if(isBound())
			return;

		if(m_autoBind)
		{
			bind();
			return;
		}

		throw Renderer::ShaderOperationRejected("Shader must be bound before using ." + std::string(_func) + "!");
	}

	Shader::~Shader()
	{
	}
}
