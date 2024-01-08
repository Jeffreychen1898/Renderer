#include <iostream>

#include <cstring>
#include <cmath>

#include <Renderer.hpp>

float sinsq(float val);
int calculateColorRed(int i, int j);
int calculateColorGreen(int i, int j);
int calculateColorBlue(int i, int j);

int main()
{
	Renderer::Window::GLFWInit();
	Renderer::Window window;
	window.init(200, 200, "Window");

	// create the texture
	/*Renderer::Color texture_color[] = {
		Renderer::Color(255, 255, 0), Renderer::Color(255, 0, 255), Renderer::Color(255, 100, 25),
		Renderer::Color(34, 255, 100), Renderer::Color(233, 53, 126), Renderer::Color(230, 42, 123),
		Renderer::Color(86, 133, 165), Renderer::Color(12, 145, 190), Renderer::Color(34, 140, 67)
	};*/
	int image_width = 1024;
	int image_height = 768;

	Renderer::Color* texture_color = new Renderer::Color[image_width * image_height];
	for(int i=0;i<image_height;++i)
	{
		for(int j=0;j<image_width;++j)
		{
			int color_red = calculateColorRed(i, j);
			int color_green = calculateColorGreen(i, j);
			int color_blue = calculateColorBlue(i, j);

			texture_color[i * image_width + j] = Renderer::Color(color_red, color_green, color_blue);
		}
	}

	unsigned char* large_texture = Renderer::colorToUCharPtr(4, image_width * image_height, texture_color);
	Renderer::flipColorVertically(image_width, image_height, 4, large_texture);

	Renderer::Texture texture(8);
	texture.setTextureFilter(GL_LINEAR, GL_LINEAR);
	texture.create(&window, image_width, image_height, 4, large_texture);
	delete[] large_texture;

	texture.write("largeTexture.png", Renderer::TextureType::PNG);
	return 0;
}

int calculateColorRed(int i, int j)
{
	return sinsq(j / 1024.f * 3.1415f) * sinsq(i / 768.f * 3.1415f) * 255.f * 255.f;
}

int calculateColorGreen(int i, int j)
{
	return 0;
}

int calculateColorBlue(int i, int j)
{
	return 0;
}

float sinsq(float val)
{
	return std::sin(val) * std::sin(val);
}
