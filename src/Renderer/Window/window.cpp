#include "Window.hpp"

namespace Renderer
{
	int Window::s_monitorCount = 0;
	Renderer::Monitor* Window::s_monitors = nullptr;
	Renderer::Window* Window::s_currentContext = nullptr;
	void Window::GLFWInit()
	{
		// initialize GLFW
		if(!glfwInit())
			throw Renderer::WindowCreationFailed("GLFW Initialization Failed!");

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// get the monitor information
		GLFWmonitor** get_monitors = glfwGetMonitors(&s_monitorCount);

		// create an array of Monitors
		s_monitors = new Monitor[s_monitorCount];

		// store all the monitor information in the array
		for(int i=0;i<s_monitorCount;++i)
		{
			const GLFWvidmode* monitor_vidmode = glfwGetVideoMode(get_monitors[i]);

			Monitor monitor_info = {
				get_monitors[i],
				monitor_vidmode->width,
				monitor_vidmode->height,
				monitor_vidmode->refreshRate,
				{
					monitor_vidmode->redBits,
					monitor_vidmode->greenBits,
					monitor_vidmode->blueBits
				}
			};

			s_monitors[i] = monitor_info;
		}
	}

	void Window::GLFWFinished()
	{
		delete[] s_monitors;

		glfwTerminate();
	}

	Window::Window(bool _autoMakeCurrent)
		: m_window(nullptr), m_width(0), m_height(0), m_currentMonitor(0), m_fullscreen(false),
		m_cursor(nullptr), m_cursorImage(nullptr), m_windowTitle(""), m_iconImage(nullptr),
		m_autoMakeCurrent(_autoMakeCurrent)
	{
		m_icon.width = 0;
		m_icon.height = 0;
		m_icon.pixels = nullptr;

		m_cursorGLFWImage.width = 0;
		m_cursorGLFWImage.height = 0;
		m_cursorGLFWImage.pixels = nullptr;
	}

	void Window::init(unsigned int _width, unsigned int _height, const char* _title)
	{
		// create the window
		m_window = glfwCreateWindow(_width, _height, _title, NULL, NULL);
		if(!m_window)
			throw Renderer::WindowCreationFailed("Failed to create window!");

		// set the variables
		m_width = _width;
		m_height = _height;
		m_windowTitle = _title;

		// make current
		makeCurrent();

		// add the event listeners
		glfwSetKeyCallback(m_window, GLFWKeyEvent);
		glfwSetMouseButtonCallback(m_window, GLFWMouseButtonEvent);
		glfwSetCursorPosCallback(m_window, GLFWMouseMoveEvent);
		glfwSetFramebufferSizeCallback(m_window, GLFWWindowResizeEvent);
		glfwSetScrollCallback(m_window, GLFWMouseScrollEvent);

		// attach window to this class
		glfwSetWindowUserPointer(m_window, this);

		// load opengl
		if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			throw Renderer::WindowCreationFailed("Failed to load OpenGL Context!");
	}

	void Window::resize(unsigned int _width, unsigned int _height)
	{
		assertContextCurrent("resize");

		glfwSetWindowSize(m_window, _width, _height);
		m_width = _width;
		m_height = _height;
	}

	void Window::setFullScreen(int _monitor)
	{
		assertContextCurrent("setFullScreen");

		if(_monitor < 0 || _monitor >= s_monitorCount)
			throw Renderer::OutOfRangeException("Monitor index out of range!");

		Monitor get_monitor = s_monitors[_monitor];
		resize(get_monitor.width, get_monitor.height);
		glfwSetWindowMonitor(m_window, get_monitor.monitor,
				0, 0, get_monitor.width, get_monitor.height,
				get_monitor.refreshRate);

		m_fullscreen = true;
	}

	void Window::exitFullScreen()
	{
		assertContextCurrent("exitFullScreen");

		glfwSetWindowMonitor(m_window, nullptr, 0, 0, m_width, m_height, GLFW_DONT_CARE);

		m_fullscreen = false;
	}

	void Window::setVSync(bool _enable)
	{
		assertContextCurrent("setVSync");

		int set_value = _enable ? 1 : 0;
		glfwSwapInterval(set_value);
	}

	void Window::setCursorMode(int _cursorMode)
	{
		assertContextCurrent("setCursorMode");

		glfwSetInputMode(m_window, GLFW_CURSOR, _cursorMode);
	}

	void Window::setCursor(int _cursor)
	{
		assertContextCurrent("setCursor");

		if(m_cursor != nullptr)
			glfwDestroyCursor(m_cursor);

		m_cursor = glfwCreateStandardCursor(_cursor);
		glfwSetCursor(m_window, m_cursor);
	}

	void Window::setCursor(const char* _image, int _hotspotX, int _hotspotY)
	{
		assertContextCurrent("setCursor");

		// load the image
		if(m_cursorImage != nullptr)
			stbi_image_free(m_cursorImage);

		int image_width, image_height, image_channels;
		m_cursorImage = stbi_load(_image, &image_width, &image_height, &image_channels, 4);

		if(m_cursorImage == nullptr)
			throw Renderer::FileNotFoundException("Image file cannot be opened: " + std::string(_image) + "!");

		// create the cursor
		if(m_cursor != nullptr)
			glfwDestroyCursor(m_cursor);

		m_cursorGLFWImage.width = image_width;
		m_cursorGLFWImage.height = image_height;
		m_cursorGLFWImage.pixels = m_cursorImage;
		m_cursor = glfwCreateCursor(&m_cursorGLFWImage, _hotspotX, _hotspotY);
		glfwSetCursor(m_window, m_cursor);
	}

	void Window::setIcon(const char* _image)
	{
		assertContextCurrent("setIcon");

		// load the image
		if(m_iconImage != nullptr)
			stbi_image_free(m_iconImage);

		int image_width, image_height, image_channels;
		m_iconImage = stbi_load(_image, &image_width, &image_height, &image_channels, 4);

		if(m_iconImage == nullptr)
			throw Renderer::FileNotFoundException("Image file cannot be opened: " + std::string(_image) + "!");

		// create the glfw image
		m_icon.width = image_width;
		m_icon.height = image_height;
		m_icon.pixels = m_iconImage;

		glfwSetWindowIcon(m_window, 1, &m_icon);
	}

	void Window::setPosition(int _x, int _y)
	{
		assertContextCurrent("setPosition");

		glfwSetWindowPos(m_window, _x, _y);
	}

	void Window::makeCurrent()
	{
		glfwMakeContextCurrent(m_window);
		s_currentContext = this;
	}

	void Window::swapBuffers()
	{
		assertContextCurrent("swapBuffers");

		glfwSwapBuffers(m_window);
	}

	bool Window::isOpened()
	{
		assertContextCurrent("isOpened");

		return !glfwWindowShouldClose(m_window);
	}

	int Window::getX()
	{
		assertContextCurrent("getX");

		int x_pos, y_pos;
		glfwGetWindowPos(m_window, &x_pos, &y_pos);

		return x_pos;
	}

	int Window::getY()
	{
		assertContextCurrent("getY)");

		int x_pos, y_pos;
		glfwGetWindowPos(m_window, &x_pos, &y_pos);

		return y_pos;
	}

	bool Window::isCurrentContext() const
	{
		return s_currentContext == this;
	}

	void Window::assertContextCurrent(std::string _func)
	{
		if(isCurrentContext())
			return;

		if(m_autoMakeCurrent)
		{
			makeCurrent();
			return;
		}

		throw Renderer::InvalidWindowContext("Invalid window context! Call .makeCurrent() before using ." + _func + "()!");
	}

	// glfw window events
	void Window::GLFWKeyEvent(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
	{
		Window* window_instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
		if(_action == GLFW_PRESS)
		{
			for(WindowEvents* each_event : window_instance->m_windowEvents)
				each_event->KeyPressed(_key, _scancode, _mods);

		} else if(_action == GLFW_RELEASE)
		{
			for(WindowEvents* each_event : window_instance->m_windowEvents)
				each_event->KeyReleased(_key, _scancode, _mods);
		}
	}

	void Window::GLFWMouseButtonEvent(GLFWwindow* _window, int _button, int _action, int _mods)
	{
		Window* window_instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
		if(_action == GLFW_PRESS)
		{
			for(WindowEvents* each_event : window_instance->m_windowEvents)
				each_event->MousePressed(_button, _mods);
		} else if(_action == GLFW_RELEASE)
		{
			for(WindowEvents* each_event : window_instance->m_windowEvents)
				each_event->MouseReleased(_button, _mods);
		}
	}

	void Window::GLFWMouseMoveEvent(GLFWwindow* _window, double _x, double _y)
	{
		Window* window_instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
		for(WindowEvents* each_event : window_instance->m_windowEvents)
			each_event->MouseMove(_x, _y);
	}

	void Window::GLFWWindowResizeEvent(GLFWwindow* _window, int _width, int _height)
	{
		Window* window_instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
		for(WindowEvents* each_event : window_instance->m_windowEvents)
			each_event->WindowResize(_width, _height);
	}

	void Window::GLFWMouseScrollEvent(GLFWwindow* _window, double _dx, double _dy)
	{
		Window* window_instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
		for(WindowEvents* each_event : window_instance->m_windowEvents)
			each_event->MouseScroll(_dx, _dy);
	}

	Window::~Window()
	{
		if(m_cursor != nullptr)
			glfwDestroyCursor(m_cursor);

		if(m_cursorImage != nullptr)
			stbi_image_free(m_cursorImage);

		if(m_iconImage != nullptr)
			stbi_image_free(m_iconImage);

		for(WindowEvents* each_event : m_windowEvents)
			delete each_event;

		glfwDestroyWindow(m_window);
	}
}
