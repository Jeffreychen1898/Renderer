#pragma once

#include <iostream>

#include <glad/glad.h>

#include "Window/Window.hpp"

namespace Renderer
{
	class Render
	{
		private:
			Renderer::Window* m_window;

		public:
			Render();
			~Render();

			void attach(Renderer::Window* _window);

			void render();

			Renderer::Window* getWindow() { return m_window; };
	};
}
