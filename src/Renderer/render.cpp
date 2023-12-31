#include "Render.hpp"

namespace Renderer
{

	Render::Render()
		: m_window(nullptr)
	{
	}

	Render::~Render()
	{
	}

	void Render::attach(Renderer::Window* _window)
	{
		if(m_window)
			throw Renderer::InvalidWindowContext("Renderer can only be attached to a Renderer::Window once!");

		m_window = _window;
	}

	void Render::render()
	{
		int indices_size = 3;
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
	}
}
