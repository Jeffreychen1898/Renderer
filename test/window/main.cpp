#include <iostream>

#include <Renderer.hpp>

/*
 * Two Windows
 *
 * number pad to swap cursors, 0 to set cursor image
 * arrow up to increase size
 * arrow down to decrease size
 * space to fullscreen or exit fullscreen
 *
 * scroll up to make window go up
 * scroll down to make window go down
 *
 * window on focus, print to console
 *
 * all window have their own icon
 *
 * any q press to terminate program
*/


int cursor_types[] = {
	GLFW_ARROW_CURSOR,
	GLFW_IBEAM_CURSOR,
	GLFW_CROSSHAIR_CURSOR,
	GLFW_HAND_CURSOR,
	GLFW_HRESIZE_CURSOR
};
int cursor_counts = 5;

class WinEventOne : public Renderer::WindowEvents
{
	public:
		Renderer::Window* window;
		int window_x;
		int window_y;
		int window_size;
		int selection = 0;

		WinEventOne(Renderer::Window& _window)
			: window_x(0), window_y(0), window_size(200), window(&_window), selection(0)
		{ }

		void KeyPressed(int _key, int _scancode, int _mods) override
		{
			window->makeCurrent();

			if(_key - 49 < cursor_counts && _key >= 49)
			{
				selection = _key - 49;

				window->setCursor(cursor_types[selection]);
			}
			if(_key == GLFW_KEY_UP)
				window_size += 5;
			else if(_key == GLFW_KEY_DOWN)
				window_size -= 5;

			if(_key == GLFW_KEY_SPACE)
			{
				if(window->isFullScreen())
					window->exitFullScreen();
				else
					window->setFullScreen();
			}

			if(_key == GLFW_KEY_Q)
				exit(0); // not ideal, but eh. whatever. It's just a test
		}
		void KeyReleased(int _key, int _scancode, int _mods) override
		{
			std::cout << "Released: " << _key << " : " << _scancode << std::endl;
		}

		void MouseScroll(double _dx, double _dy) override
		{
			window_x += _dx * 5;
			window_y += _dy * 5;
		}
};

int main()
{
	Renderer::Window window_one;
	Renderer::Window window_two;

	Renderer::Window::GLFWInit();

	WinEventOne* event_one = new WinEventOne(window_one);
	WinEventOne* event_two = new WinEventOne(window_two);
	event_two->window_x = 300;

	window_one.init(event_one->window_size, event_one->window_size, "Window 1");
	window_one.setPosition(event_one->window_x, event_one->window_y);
	window_one.setIcon("./logo1.png");
	window_one.setCursor("./logo1.png", 0, 0);

	window_two.init(event_two->window_size, event_two->window_size, "Window 2");
	window_two.setPosition(event_two->window_x, event_two->window_y);
	window_two.setIcon("./logo2.png");
	window_two.setCursor("./logo2.png", 16, 16);

	window_one.addEvents(event_one);
	window_two.addEvents(event_two);

	Renderer::Monitor prim_monitor = Renderer::Window::getMonitorInfo(0);
	std::cout << prim_monitor.width << " : " << prim_monitor.height << std::endl;

	while(true)
	{
		window_one.makeCurrent();

		window_one.setPosition(event_one->window_x, event_one->window_y);

		if(!window_one.isFullScreen())
			window_one.resize(event_one->window_size, event_one->window_size);

		window_one.swapBuffers();

		window_two.makeCurrent();

		window_two.setPosition(event_two->window_x, event_two->window_y);

		if(!window_two.isFullScreen())
			window_two.resize(event_two->window_size, event_two->window_size);

		window_two.swapBuffers();
		
		Renderer::Window::pollEvents();
	}
	return 0;
}
