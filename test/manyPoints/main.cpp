#include <iostream>
#include <cmath>

#include <Renderer.hpp>

// renderer todo:
//		renderer.clear()
//		renderer throw exception on too large of a shape
//		test: render large shape (or tons of points)

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BATCH_COUNT 50

// uses:
//		vec2 position
//		vec3 color
const char* vertex_shader = R"(
#version 410 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec3 a_color;

uniform mat4 u_projection;

out vec3 v_color;

void main()
{
	gl_FragCoord = u_projection * vec4(a_position, 0.0, 1.0);
	v_color = a_color;
}
)";
const char* fragment_shader = R"(
#version 410 core

in vec3 v_color;

out vec4 FragColor;

void main()
{
	FragColor = vec4(v_color, 1.0);
}
)";

int main()
{
	Renderer::Window::GLFWInit();

	Renderer::Window window;
	window.init(WINDOW_WIDTH, WINDOW_HEIGHT, "Ray Tracing");

	// create the renderer
	Renderer::Render renderer;
	renderer.attach(&window);
	renderer.init();

	// create the shader
	Renderer::Shader shader;
	shader.attach(&window);
	shader.create(vertex_shader, fragment_shader);
	shader.vertexAttribAdd(0, Renderer::AttribType::VEC2);
	shader.vertexAttribAdd(1, Renderer::AttribType::VEC3);
	shader.vertexAttribsEnable();
	shader.uniformAdd("u_projection", Renderer::UniformType::MAT4);
	Renderer::Mat4<float> projection_matrix = Renderer::Math::projection2D(
		0.f, static_cast<float>(WINDOW_WIDTH),
		0.f, static_cast<float>(WINDOW_HEIGHT),
		1.f, -1.f
	);
	shader.setUniformMatrix("u_projection", *projection_matrix);

	while(window.isOpened())
	{
		int rh = std::ceil(WINDOW_HEIGHT / static_cast<float>(BATCH_COUNT));
		std::cout << shader.getVertexBitSize() * WINDOW_WIDTH * rh << std::endl;
		// clear buffer
		glClear(GL_COLOR_BUFFER_BIT);

		// draw a point on the screen
		renderer.bindShader(&shader);
		for(int b=0;b<BATCH_COUNT;++b)
		{
			int render_height = std::ceil(WINDOW_HEIGHT / static_cast<float>(BATCH_COUNT));
			renderer.render();
			renderer.beginShape(Renderer::DrawType::POINTS, WINDOW_WIDTH*render_height, 0);

			for(int i=0;i<render_height;++i)
			{
				for(int j=0;j<WINDOW_WIDTH;++j)
				{
					if(!(i == 0 && j == 0))
						renderer.nextVertex();
					renderer.vertex2f(j, i + render_height * b);
					renderer.vertex3f(1.f, 0.f, 1.f);
				}
			}
			
			renderer.endShape();
		}

		renderer.render();

		window.swapBuffers();
		Renderer::Window::pollEvents();
	}

	return 0;
}
