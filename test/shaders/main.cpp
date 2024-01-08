#include <iostream>
#include <Renderer.hpp>

int main() {
	Renderer::Window::GLFWInit();

	// initialize everything for window 1
	Renderer::Window window;
	window.init(800, 600, "Test Shaders");

	Renderer::Shader shader;
	shader.attach(&window);
	shader.createFromFile("./vertex.glsl", "./fragment.glsl", true);
	shader.vertexAttribAdd(0, Renderer::AttribType::VEC3);
	shader.vertexAttribAdd(1, Renderer::AttribType::VEC3);
	shader.uniformAdd("u_projection", Renderer::UniformType::MAT4);
	shader.uniformAdd("u_sumVector", Renderer::UniformType::IVEC3);
	shader.vertexAttribsEnable();

	float vertices[] = {
		-1.f, -0.5f, 0.f,
		1.f, 0.f, 0.f,

		0.f, -0.5f, 0.f,
		0.f, 1.f, 0.f,

		-0.5f, 0.5f, 0.f,
		0.f, 0.f, 1.f
	};
	unsigned int indices[] = {0, 1, 2};

	shader.verticesData(vertices, 18 * sizeof(float));
	shader.indicesData(indices, 3);

	Renderer::Mat4<float> projection(
		0.5f, 0.f, 0.f, 0.f,
		0.f, 0.5f, 0.f, -0.5f,
		0.f, 0.f, 0.5f, 0.f,
		0.f, 0.f, 0.f, 1.f
	);
	shader.setUniformMatrix("u_projection", *projection);

	Renderer::Vec3<int> sum_vector(1, 0, 0);
	shader.setUniformInt("u_sumVector", *sum_vector);

	// initialize everything for window 2
	Renderer::Window window2;
	window2.init(800, 600, "Test Shaders 2");

	Renderer::Shader shader2;
	shader2.attach(&window2);
	shader2.createFromFile("./vertex.glsl", "./fragment.glsl", true);
	shader2.vertexAttribAdd(0, Renderer::AttribType::VEC3);
	shader2.vertexAttribAdd(1, Renderer::AttribType::VEC3);
	shader2.uniformAdd("u_projection", Renderer::UniformType::MAT4);
	shader2.uniformAdd("u_sumVector", Renderer::UniformType::IVEC3);
	shader2.vertexAttribsEnable();

	shader2.verticesData(vertices, 18 * sizeof(float));
	shader2.indicesData(indices, 3);

	Renderer::Mat4<float> projection2;
	shader2.setUniformMatrix("u_projection", *projection2);

	Renderer::Vec3<int> sum_vector2(0, 0, 0);
	shader2.setUniformInt("u_sumVector", *sum_vector2);

	while(window2.isOpened())
	{
		window.makeCurrent();
		//shader.bind();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.bind();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
		
		window.swapBuffers();

		window2.makeCurrent();

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		shader2.bind();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

		window2.swapBuffers();
		Renderer::Window::pollEvents();
	}
    return 0;
}
