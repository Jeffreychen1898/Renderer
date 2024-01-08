#include <iostream>

#include <Renderer.hpp>

#undef VERTEX_BATCH_SIZE
#define VERTEX_BATCH_SIZE 10000

const char* vertex_shader = R"(
#version 410 core
layout (location = 0) in vec2 a_position;

uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * vec4(a_position, 0.0, 1.0);
}
)";
const char* fragment_shader = R"(
#version 410 core

out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}
)";

int main()
{
	Renderer::Window::GLFWInit();

	Renderer::Window window;
	window.init(800, 600, "My Window");

	Renderer::Render renderer;
	renderer.attach(&window);
	renderer.init();

	Renderer::Shader test_shader;
	test_shader.attach(&window);
	test_shader.create(vertex_shader, fragment_shader);
	test_shader.vertexAttribAdd(0, Renderer::AttribType::VEC2);
	test_shader.vertexAttribsEnable();
	test_shader.uniformAdd("u_projection", Renderer::UniformType::MAT4);
	Renderer::Mat4<float> projection = Renderer::Math::projection2D(0.f, 800.f, 0.f, 600.f, 1.f, -1.f);
	test_shader.setUniformMatrix("u_projection", *projection);

	Renderer::Window window2;
	window2.init(800, 600, "Second Window");

	Renderer::Render renderer2;
	renderer2.attach(&window2);
	renderer2.init();

	Renderer::Texture test_texture;
	test_texture.load(&window2, "largeTexture.png");

	float rotation_angle = 0.f;

	window.makeCurrent();
	while(window.isOpened())
	{
		window.makeCurrent();
		glClear(GL_COLOR_BUFFER_BIT);

		renderer.bindShader(&test_shader);

		renderer.beginShape(Renderer::DrawType::TRIANGLE, 4, 0);
		renderer.vertex2f(100.f, 100.f);
		renderer.nextVertex();
		renderer.vertex2f(200.f, 100.f);
		renderer.nextVertex();
		renderer.vertex2f(200.f, 200.f);
		renderer.nextVertex();
		renderer.vertex2f(100.f, 200.f);
		renderer.endShape();

		renderer.setColor(Renderer::Color(255, 255, 255, 150));
		renderer.drawRect(150, 150, 200, 200);

		renderer.render();

		window.swapBuffers();

		window2.makeCurrent();
		glClear(GL_COLOR_BUFFER_BIT);

		renderer2.bindShader(nullptr);
		renderer2.bindTexture(nullptr);

		renderer2.beginShape(Renderer::DrawType::TRIANGLE, 3, 0);
		renderer2.vertex2f(300.f, 200.f);
		renderer2.vertex4f(1.f, 1.f, 0.f, 1.f);
		renderer2.vertex2f(0.f, 0.f);
		renderer2.nextVertex();
		renderer2.vertex2f(400.f, 300.f);
		renderer2.vertex4f(1.f, 1.f, 0.f, 1.f);
		renderer2.vertex2f(0.f, 1.f);
		renderer2.nextVertex();
		renderer2.vertex2f(400.f, 400.f);
		renderer2.vertex4f(1.f, 1.f, 0.f, 1.f);
		renderer2.vertex2f(1.f, 0.f);
		renderer2.endShape();

		renderer2.setAngle(rotation_angle);
		renderer2.setAlign(Renderer::HorizontalAlign::CENTER, Renderer::VerticalAlign::CENTER);
		renderer2.drawImage(test_texture, 500, 500, 200, 200);
		renderer2.setAngle(0.f);
		rotation_angle += 0.02;

		renderer2.render();

		window2.swapBuffers();
		window.makeCurrent();

		Renderer::Window::pollEvents();
	}

	Renderer::Window::GLFWFinished();
	return 0;
}
