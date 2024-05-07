#include <Renderer.hpp>

int main()
{
	Renderer::Window::GLFWInit();
	Renderer::Window window;
	window.init(800, 600, "My Window");

	Renderer::Render renderer;
	renderer.attach(&window);
	renderer.init();

	while(window.isOpened())
	{
		glClear(GL_COLOR_BUFFER_BIT);

		renderer.setColor(Renderer::Color(255, 0, 255, 255));
		renderer.drawRect(100, 100, 100, 100);
		renderer.drawRect(200, 200, 100, 100);

		renderer.render();
		window.swapBuffers();
		Renderer::Window::pollEvents();
	}
	return 0;
}
