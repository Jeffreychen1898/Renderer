#include <iostream>
#include <cstring>
#include <cassert>

#include <Renderer.hpp>

const char* vertex_code = R"(
#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texCoord;

out vec2 v_texCoord;

void main()
{
	gl_Position = vec4(a_position, 1.0);
	v_texCoord = a_texCoord;
}
)";

const char* fragment_code = R"(
#version 410 core

in vec2 v_texCoord;

out vec4 FragColor;

uniform sampler2D u_sampler[];

void main()
{
	vec4 texel = texture(u_sampler[0], v_texCoord);
	vec4 texel1 = texture(u_sampler[1], v_texCoord * 2.0 - vec2(0.5));
	FragColor = vec4(mix(texel.rgb, texel1.rgb, 0.5), 1.0);
}
)";

void createLargeImage(Renderer::Texture& _image);

int main()
{
	Renderer::Window::GLFWInit();
	Renderer::Window window;
	// window not really needed here lul
	window.init(200, 200, "Window");

	// create the texture
	unsigned char simple_texture[] = {
		255, 255, 0,	255, 0, 255,	255, 100, 25,
		34, 255, 100,	233, 53, 126,	230, 42, 123,
		86, 133, 165,	12, 145, 190,	34, 140, 67
	};
	Renderer::flipColorVertically(3, 3, 3, simple_texture);

	Renderer::Texture texture(32);
	texture.setTextureFilter(GL_NEAREST, GL_NEAREST);
	texture.create(&window, 3, 3, 3, simple_texture);

	assert(texture.getPixel(0, 0) == Renderer::Color(255, 255, 0, 0));
	assert(texture.getPixel(1, 1) == Renderer::Color(233, 53, 126, 0));
	assert(texture.getPixel(2, 2) == Renderer::Color(34, 140, 67, 0));

	unsigned char new_img_data[] = {
		255, 0, 0,		255, 255, 0,
		255, 0, 0,		255, 0, 0
	};
	Renderer::flipColorVertically(2, 2, 3, new_img_data);

	texture.setPixels(1, 1, 2, 2, new_img_data);
	texture.setPixel(2, 2, Renderer::Color(0, 0, 255));

	// load the second texture
	Renderer::Texture texture2;
	texture2.setTextureBorderColor(Renderer::Color(0, 255, 0));
	texture2.setTextureWrap(GL_CLAMP_TO_BORDER, GL_REPEAT);
	texture2.setTextureFilter(GL_LINEAR, GL_LINEAR);
	texture2.load(&window, "./largeTexture.png");
	texture2.bind(1);
	texture.bind(0);
	
	// create the shader
	Renderer::Shader shader;
	shader.attach(&window);
	shader.create(vertex_code, fragment_code, true);
	shader.vertexAttribAdd(0, Renderer::AttribType::VEC3);
	shader.vertexAttribAdd(1, Renderer::AttribType::VEC2);
	shader.vertexAttribsEnable();
	shader.uniformAdd("u_sampler", Renderer::UniformType::INT_ARR);
	int sampler_arr[] = { 0, 1 };
	shader.setUniformInt("u_sampler", 2, sampler_arr);

	float vertices[] = {
		-0.5f, -0.5f, 0.f,
		0.f, 0.f,

		-0.5f,  0.5f, 0.f,
		0.f, 1.f,

		 0.5f, -0.5f, 0.f,
		 1.f, 0.f,

		 0.5f,  0.5f, 0.f,
		 1.f, 1.f
	};
	unsigned int indices[] = {0, 1, 2, 3};

	shader.verticesData(vertices, 20 * sizeof(float));
	shader.indicesData(indices, 4);

	while(window.isOpened())
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr);

		window.swapBuffers();
		Renderer::Window::pollEvents();
	}

	texture.readPixels();
	texture.write("simple.jpg", Renderer::TextureType::JPG);
	return 0;
}