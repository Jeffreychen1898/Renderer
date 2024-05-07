#include "Render.hpp"

static const char* default_vertex_shader = R"(
#version 410 core
layout (location = 0) in vec2 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_texCoord;

uniform mat4 u_projection;

out vec4 v_color;
out vec2 v_texCoord;

void main()
{
	gl_Position = u_projection * vec4(a_position, 0.0, 1.0);
	v_color = a_color;
	v_texCoord = a_texCoord;
}
)";

static const char* default_fragment_shader = R"(
#version 410 core
in vec4 v_color;
in vec2 v_texCoord;

uniform sampler2D u_texture;

out vec4 FragColor;

void main()
{
	vec4 texel = texture(u_texture, v_texCoord);
	FragColor = v_color * texel;
}
)";

namespace Renderer
{
	Render::Render(unsigned int _vertexBatchSize, unsigned int _indexBatchSize)
		: m_window(nullptr), m_verticesTracker(0), m_indicesTracker(0), m_defaultShader(nullptr),
		m_currentDrawType(DrawType::NONE), m_whiteTexture(nullptr), m_shapeVertexTracker(0), m_shapeVertexBytesLeft(0),
		m_shapeIndexCount(0), m_startOfShapeVertexTracker(0), m_vertexBatchSize(_vertexBatchSize),
		m_indexBatchSize(_indexBatchSize)
	{
		m_verticesBatch = new unsigned char[_vertexBatchSize];
		m_indicesBatch = new unsigned int[_indexBatchSize];
		m_defaultRectStyle = {
			Renderer::Color(255),
			VerticalAlign::DEFAULT,
			HorizontalAlign::DEFAULT,
			0, 0,
			0.f
		};
	}

	void Render::attach(Renderer::Window* _window)
	{
		if(m_window)
			throw Renderer::InvalidWindowContext("Renderer can only be attached to a Renderer::Window once!");

		m_window = _window;

		RendererWindowEvent* window_event = new RendererWindowEvent(this);
		m_window->addEvents(window_event);
	}

	void Render::init()
	{
		glEnable(GL_BLEND);
		setBlendMode(BlendMode::BLEND);

		m_defaultShader = new Renderer::Shader;
		m_defaultShader->attach(m_window);
		m_defaultShader->create(default_vertex_shader, default_fragment_shader);
		// shader attributes
		m_defaultShader->vertexAttribAdd(0, Renderer::AttribType::VEC2);
		m_defaultShader->vertexAttribAdd(1, Renderer::AttribType::VEC4);
		m_defaultShader->vertexAttribAdd(2, Renderer::AttribType::VEC2);
		m_defaultShader->vertexAttribsEnable();
		// shader uniforms
		m_defaultShader->uniformAdd("u_projection", Renderer::UniformType::MAT4);
		m_defaultShader->uniformAdd("u_texture", Renderer::UniformType::INT);
		m_defaultShader->setUniformInt("u_texture", 0);

		Renderer::Mat4<float> projection = Renderer::Math::projection2D(
				0.f, static_cast<float>(m_window->getWidth()),
				0.f, static_cast<float>(m_window->getHeight()),
				1.f, -1.f);
		m_defaultShader->setUniformMatrix("u_projection", *projection);

		// default texture
		unsigned char default_texture_data[] = {255, 255, 255};
		m_whiteTexture = new Renderer::Texture(8);
		m_whiteTexture->create(m_window, 1, 1, 3, default_texture_data);

		bindTexture(m_whiteTexture, 0);
	}

	void Render::setBlendMode(BlendMode blendMode)
	{
		switch(blendMode)
		{
			case BlendMode::BLEND:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBlendEquation(GL_FUNC_ADD);
				break;
			case BlendMode::MULTIPLY:
				glBlendFunc(GL_DST_COLOR, GL_ZERO);
				glBlendEquation(GL_FUNC_ADD);
				break;
			case BlendMode::ADD:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glBlendEquation(GL_FUNC_ADD);
				break;
			case BlendMode::SUBTRACT:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBlendEquation(GL_FUNC_SUBTRACT);
				break;
			case BlendMode::REPLACE:
				glBlendFunc(GL_ONE, GL_ZERO);
				glBlendEquation(GL_FUNC_ADD);
				break;
			case BlendMode::IFEMPTY:
				glBlendFunc(GL_ZERO, GL_ONE);
				glBlendEquation(GL_FUNC_ADD);
				break;
			case BlendMode::MIN:
				glBlendFunc(GL_ONE, GL_ONE);
				glBlendEquation(GL_MIN);
				break;
			case BlendMode::MAX:
				glBlendFunc(GL_ONE, GL_ONE);
				glBlendEquation(GL_MAX);
				break;
		}
	}

	void Render::bindShader(Renderer::Shader* _shader)
	{
		Renderer::Shader* bind_shader = _shader ? _shader : m_defaultShader;
		if(bind_shader->isBound())
			return;

		render();
		bind_shader->bind();
	}

	void Render::bindTexture(Renderer::Texture* _texture, unsigned int _slot)
	{
		Renderer::Texture* bind_texture = _texture ? _texture : m_whiteTexture;

		if(bind_texture->isBound(_slot))
			return;

		render();
		bind_texture->bind(_slot);
	}

	void Render::setColor(const Renderer::Color& _color)
	{
		m_defaultRectStyle.color = _color;
	}

	void Render::setAlign(HorizontalAlign _horizontal, VerticalAlign _vertical)
	{
		m_defaultRectStyle.horizontalAlign = _horizontal;
		m_defaultRectStyle.verticalAlign = _vertical;
	}

	void Render::setAlign(int _x, int _y)
	{
		m_defaultRectStyle.horizontalAlignAmount = _x;
		m_defaultRectStyle.verticalAlignAmount = _y;
	}

	void Render::setAngle(float _angle)
	{
		m_defaultRectStyle.angle = _angle;
	}

	void Render::drawRect(int _x, int _y, int _width, int _height)
	{
		drawRect(_x, _y, _width, _height, m_defaultRectStyle);
	}

	void Render::drawRect(int _x, int _y, int _width, int _height, const RectStyle& _style)
	{
		drawImage(*m_whiteTexture, _x, _y, _width, _height, _style);
	}

	void Render::drawImage(Renderer::Texture& _texture, int _x, int _y, int _width, int _height)
	{
		drawImage(_texture, _x, _y, _width, _height, m_defaultRectStyle);
	}

	void Render::drawImage(Renderer::Texture& _texture, int _x, int _y, int _width, int _height, const RectStyle& _style)
	{
		// calculate the alignment
		float vertical_align = _style.verticalAlignAmount;
		float horizontal_align = _style.horizontalAlignAmount;
		switch(_style.verticalAlign)
		{
			case VerticalAlign::TOP:
				vertical_align = 0;
				break;
			case VerticalAlign::CENTER:
				vertical_align = _height / 2;
				break;
			case VerticalAlign::BOTTOM:
				vertical_align = _height;
				break;
			default:
				break;
		}

		switch(_style.horizontalAlign)
		{
			case HorizontalAlign::LEFT:
				horizontal_align = 0;
				break;
			case HorizontalAlign::CENTER:
				horizontal_align = _width / 2;
				break;
			case HorizontalAlign::RIGHT:
				horizontal_align = _width;
				break;
			default:
				break;
		}

		// create the vertices
		float vertices[] = {
			-horizontal_align, -vertical_align, // top left
			-horizontal_align, -vertical_align + _height, // bottom left
			-horizontal_align + _width, -vertical_align + _height, // bottom right
			-horizontal_align + _width, -vertical_align // top right
		};

		// rotate the vertices, then add the _x and _y
		float cos_ang = std::cos(_style.angle);
		float sin_ang = std::sin(_style.angle);

		for(int i=0;i<4;++i)
		{
			float x = vertices[i * 2];
			float y = vertices[i * 2 + 1];
			vertices[i * 2] = cos_ang * x - sin_ang * y + _x;
			vertices[i * 2 + 1] = sin_ang * x + cos_ang * y + _y;
		}

		// draw the shape
		bindTexture(&_texture, 0);
		bindShader(m_defaultShader);

		float col_r = _style.color.red / 255.f;
		float col_g = _style.color.green / 255.f;
		float col_b = _style.color.blue / 255.f;
		float col_a = _style.color.alpha / 255.f;

		beginShape(Renderer::DrawType::TRIANGLE, 4, 0);
		vertex2f(vertices[0], vertices[1]);
		vertex4f(col_r, col_g, col_b, col_a);
		vertex2f(0.f, 1.f);
		nextVertex();
		vertex2f(vertices[2], vertices[3]);
		vertex4f(col_r, col_g, col_b, col_a);
		vertex2f(0.f, 0.f);
		nextVertex();
		vertex2f(vertices[4], vertices[5]);
		vertex4f(col_r, col_g, col_b, col_a);
		vertex2f(1.f, 0.f);
		nextVertex();
		vertex2f(vertices[6], vertices[7]);
		vertex4f(col_r, col_g, col_b, col_a);
		vertex2f(1.f, 1.f);
		endShape();
	}

	void Render::beginShape(DrawType _type, unsigned int _vertexCount, unsigned int _indicesCount)
	{
		if(Renderer::Shader::getCurrentShader()->getWindow() != m_window)
			throw Renderer::RenderingException("The currently bound shader is not for this window context!");

		if(_type == DrawType::NONE)
			return;

		if(Renderer::Shader::getCurrentShader()->getVertexBitSize() * _vertexCount >= m_vertexBatchSize)
			throw Renderer::RenderingException("The shape is too large. Consider splitting it up or changing the vertex batch size!");

		if(m_currentDrawType != _type)
			render(); // flush all the other shapes first

		unsigned int shape_bytes = Renderer::Shader::getCurrentShader()->getVertexBitSize() * _vertexCount;
		if(m_verticesTracker + shape_bytes >= m_vertexBatchSize)
			render(); // flush all the other shapes first

		// calculate default number of indices
		if(_indicesCount == 0)
		{
			switch(_type)
			{
				case DrawType::TRIANGLE:
					_indicesCount = 3 * (_vertexCount - 2);
					break;
				default:
					_indicesCount = _vertexCount;
					break;
			}
		}

		if(m_indicesTracker + _indicesCount >= m_indexBatchSize)
			render(); // flush all the other shapes first

		// setup variables for the upcoming shape
		m_currentDrawType = _type;

		m_shapeIndexCount = _indicesCount;
		m_shapeVertexTracker = _vertexCount;
		m_shapeVertexBytesLeft = Renderer::Shader::getCurrentShader()->getVertexBitSize();
		m_startOfShapeVertexTracker = m_verticesTracker / m_shapeVertexBytesLeft;
	}

	void Render::nextVertex()
	{
		if(m_shapeVertexTracker == 0)
			throw Renderer::RenderingException("Too many vertices are passed in! Please call beginShape() with the correct number of vertices!");

		if(m_shapeVertexBytesLeft > 0)
			throw Renderer::RenderingException("Not enough bytes are passed in for the previous vertex!");

		-- m_shapeVertexTracker;
		m_shapeVertexBytesLeft = Renderer::Shader::getCurrentShader()->getVertexBitSize();
	}

	void Render::endShape(const unsigned int* _indices)
	{
		if(m_shapeVertexTracker > 1)
			throw Renderer::RenderingException("Too few vertices are passed in! Please call beginShape() with the correct number of vertices!");
		if(m_shapeVertexTracker < 1)
			throw Renderer::RenderingException("Too many vertices are passed in! Please call beginShape() with the correct number of vertices!");

		if(m_shapeVertexBytesLeft > 0)
			throw Renderer::RenderingException("Not enough bytes are passed in for the last vertex!");

		memcpy(m_indicesBatch + m_indicesTracker, _indices, sizeof(unsigned int) * m_shapeIndexCount);
		for(int i=0;i<m_shapeIndexCount;++i)
			m_indicesBatch[m_indicesTracker + i] += m_startOfShapeVertexTracker;

		m_indicesTracker += m_shapeIndexCount;
	}

	void Render::endShape()
	{
		unsigned int* calculate_indices = new unsigned int[m_shapeIndexCount];

		// for triangles (which are used for drawing polygons)
		if(m_currentDrawType == DrawType::TRIANGLE)
		{
			for(unsigned int i=0;i<m_shapeIndexCount / 3;++i)
			{
				calculate_indices[3 * i    ] = 0;
				calculate_indices[3 * i + 1] = 1 + i;
				calculate_indices[3 * i + 2] = 2 + i;
			}
			endShape(calculate_indices);

			delete[] calculate_indices;
			return;
		}

		// for every other draw types
		for(unsigned int i=0;i<m_shapeIndexCount;++i)
			calculate_indices[i] = i;

		endShape(calculate_indices);
		delete[] calculate_indices;
		return;
	}

	void Render::vertex1f(float _v)
	{
		assertShapeVertexSafeToStore(sizeof(float));
		memcpy(m_verticesBatch + m_verticesTracker, &_v, sizeof(float));
		m_verticesTracker += sizeof(float);
	}

	void Render::vertex2f(float _v0, float _v1)
	{
		assertShapeVertexSafeToStore(sizeof(float) * 2);
		memcpy(m_verticesBatch + m_verticesTracker, &_v0, sizeof(float));
		memcpy(m_verticesBatch + m_verticesTracker + sizeof(float), &_v1, sizeof(float));

		m_verticesTracker += sizeof(float) * 2;
	}

	void Render::vertex3f(float _v0, float _v1, float _v2)
	{
		assertShapeVertexSafeToStore(sizeof(float) * 3);
		memcpy(m_verticesBatch + m_verticesTracker, &_v0, sizeof(float));
		memcpy(m_verticesBatch + m_verticesTracker + sizeof(float), &_v1, sizeof(float));
		memcpy(m_verticesBatch + m_verticesTracker + sizeof(float) * 2, &_v2, sizeof(float));

		m_verticesTracker += sizeof(float) * 3;
	}

	void Render::vertex4f(float _v0, float _v1, float _v2, float _v3)
	{
		assertShapeVertexSafeToStore(sizeof(float) * 4);
		memcpy(m_verticesBatch + m_verticesTracker, &_v0, sizeof(float));
		memcpy(m_verticesBatch + m_verticesTracker + sizeof(float), &_v1, sizeof(float));
		memcpy(m_verticesBatch + m_verticesTracker + sizeof(float) * 2, &_v2, sizeof(float));
		memcpy(m_verticesBatch + m_verticesTracker + sizeof(float) * 3, &_v3, sizeof(float));
		m_verticesTracker += sizeof(float) * 4;
	}

	void Render::vertex1i(int _v)
	{
		assertShapeVertexSafeToStore(sizeof(int));
		memcpy(m_verticesBatch + m_verticesTracker, &_v, sizeof(int));
		m_verticesTracker += sizeof(int);
	}

	void Render::vertex2i(int _v0, int _v1)
	{
		assertShapeVertexSafeToStore(sizeof(int) * 2);
		memcpy(m_verticesBatch + m_verticesTracker, &_v0, sizeof(int));
		memcpy(m_verticesBatch + m_verticesTracker + sizeof(int), &_v1, sizeof(int));
		m_verticesTracker += sizeof(int) * 2;
	}

	void Render::vertex3i(int _v0, int _v1, int _v2)
	{
		assertShapeVertexSafeToStore(sizeof(int) * 3);
		memcpy(m_verticesBatch + m_verticesTracker, &_v0, sizeof(int));
		memcpy(m_verticesBatch + m_verticesTracker + sizeof(int), &_v1, sizeof(int));
		memcpy(m_verticesBatch + m_verticesTracker + sizeof(int) * 2, &_v2, sizeof(int));
		m_verticesTracker += sizeof(int) * 3;
	}

	void Render::vertex4i(int _v0, int _v1, int _v2, int _v3)
	{
		assertShapeVertexSafeToStore(sizeof(int) * 4);
		memcpy(m_verticesBatch + m_verticesTracker, &_v0, sizeof(int));
		memcpy(m_verticesBatch + m_verticesTracker + sizeof(int), &_v1, sizeof(int));
		memcpy(m_verticesBatch + m_verticesTracker + sizeof(int) * 2, &_v2, sizeof(int));
		memcpy(m_verticesBatch + m_verticesTracker + sizeof(int) * 3, &_v3, sizeof(int));
		m_verticesTracker += sizeof(int) * 4;
	}

	void Render::render()
	{
		Renderer::Shader* current_shader = Renderer::Shader::getCurrentShader();
		if(current_shader->getWindow() != m_window)
			return;

		GLenum gl_draw_type;
		switch(m_currentDrawType)
		{
			case DrawType::POINTS:
				gl_draw_type = GL_POINTS;
				break;
			case DrawType::TRIANGLE:
				gl_draw_type = GL_TRIANGLES;
				break;
			case DrawType::TRIANGLE_STRIP:
				gl_draw_type = GL_TRIANGLE_STRIP;
				break;
			case DrawType::TRIANGLE_FAN:
				gl_draw_type = GL_TRIANGLE_FAN;
				break;
			case DrawType::LINE:
				gl_draw_type = GL_LINES;
				break;
			case DrawType::LINE_STRIP:
				gl_draw_type = GL_LINE_STRIP;
				break;
			case DrawType::LINE_LOOP:
				gl_draw_type = GL_LINE_LOOP;
				break;
			default:
				return;
				break;
		}
		if(m_indicesTracker < 3)
			return;

		current_shader->verticesData(m_verticesBatch, sizeof(unsigned char) * m_verticesTracker);
		current_shader->indicesData(m_indicesBatch, m_indicesTracker);
		glDrawElements(gl_draw_type, m_indicesTracker, GL_UNSIGNED_INT, nullptr);

		m_verticesTracker = 0;
		m_indicesTracker = 0;
	}

	void Render::assertShapeVertexSafeToStore(unsigned int _bytesRequired)
	{
		if(m_shapeVertexBytesLeft < _bytesRequired)
			throw Renderer::RenderingException("Too many bytes are passed in for the vertex!");

		m_shapeVertexBytesLeft -= _bytesRequired;
	}

	Render::~Render()
	{
		delete[] m_verticesBatch;
		delete[] m_indicesBatch;
		delete m_defaultShader;
		delete m_whiteTexture;
	}

	void RendererWindowEvent::WindowResize(int _width, int _height)
	{
		m_renderer->getWindow()->makeCurrent();

		Renderer::Mat4<float> projection = Renderer::Math::projection2D(
				0.f, static_cast<float>(_width),
				0.f, static_cast<float>(_height),
				1.f, -1.f);
		m_renderer->m_defaultShader->setUniformMatrix("u_projection", *projection);
	}
}
