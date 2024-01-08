#include "Texture.hpp"

namespace Renderer
{
	unsigned int Texture::s_activeSlot = 0;
	std::vector<Texture*> Texture::s_boundedTextures;
	Texture::Texture(unsigned int _channelSize, bool _autoBind)
		: m_channelSize(_channelSize), m_data(nullptr), m_channels(0), m_width(0), m_height(0), m_textureId(0),
		m_validImage(false), m_textureWrapS(GL_CLAMP_TO_EDGE), m_textureWrapT(GL_CLAMP_TO_EDGE),
		m_textureFilterMag(GL_LINEAR), m_textureFilterMin(GL_LINEAR), m_useMipmaps(true), m_autobind(_autoBind),
		m_window(nullptr), m_borderColor(0), m_fromFile(false)
	{
	}

	void Texture::create(Renderer::Window* _window, unsigned int _width, unsigned int _height,
			unsigned int _channels, unsigned char* _data)
	{
		// NOTE: put this in Renderer::Render, this should be default
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		m_window = _window;
		assertCurrentContext();

		m_channels = _channels;
		m_width = _width;
		m_height = _height;

		if(m_fromFile)
			m_data = _data;
		else
		{
			m_data = new unsigned char[_width * _height * _channels];
			memcpy(m_data, _data, sizeof(unsigned char) * _width * _height * _channels);
		}

		glGenTextures(1, &m_textureId);

		bind(0);

		if(m_textureWrapS == GL_CLAMP_TO_BORDER || m_textureWrapT == GL_CLAMP_TO_BORDER)
		{
			float border_color[] = {
				m_borderColor.red / 255.f,
				m_borderColor.green / 255.f,
				m_borderColor.blue / 255.f,
				m_borderColor.alpha / 255.f
			};
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_textureWrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_textureWrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_textureFilterMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_textureFilterMag);

		GLenum data_format = GL_RGB;
		switch(m_channels)
		{
			case 1:
				data_format = GL_RED;
				break;
			case 2:
				data_format = GL_RG;
				break;
			case 3:
				data_format = GL_RGB;
				break;
			case 4:
				data_format = GL_RGBA;
				break;
			default:
				throw Renderer::InvalidFormat("Invalid channel count. There are only 1, 2, 3, or 4 channels!");
				break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, getInternalFormat(), m_width, m_height, 0, data_format, GL_UNSIGNED_BYTE, m_data);
		if(m_useMipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		bind(0);
	}

	void Texture::load(Renderer::Window* _window, const char* _path)
	{
		m_window = _window;
		assertCurrentContext();

		stbi_set_flip_vertically_on_load(1);

		int image_width, image_height;
		int image_channels;
		m_data = stbi_load(_path, &image_width, &image_height, &image_channels, 0);

		m_fromFile = true;

		create(_window, image_width, image_height, image_channels, m_data);

		stbi_set_flip_vertically_on_load(0);
	}

	void Texture::readPixels()
	{
		assertCurrentContext();
		assertBound("readPixels()");
		
		GLenum data_format = GL_RGB;
		switch(m_channels)
		{
			case 1:
				data_format = GL_RED;
				break;
			case 2:
				data_format = GL_RG;
				break;
			case 3:
				data_format = GL_RGB;
				break;
			case 4:
				data_format = GL_RGBA;
				break;
			default:
				throw Renderer::InvalidFormat("Invalid channel count. There are only 1, 2, 3, or 4 channels!");
				break;
		}
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data);
	}

	Color Texture::getPixel(unsigned int _x, unsigned int _y)
	{
		assertCurrentContext();
		assertBound("getPixel()");

		_y = m_height - _y - 1;

		unsigned char* red_color = m_data + (m_width * _y + _x) * m_channels;

		switch(m_channels)
		{
			case 1:
				return { *red_color, 0, 0, 0 };
				break;
			case 2:
				return { *red_color, *(red_color + 1), 0, 0 };
				break;
			case 3:
				return { *red_color, *(red_color + 1), *(red_color + 2), 0 };
				break;
			case 4:
				return { *red_color, *(red_color + 1), *(red_color + 2), *(red_color + 3) };
				break;
			default:
				break;
		}

		return { 0 };
	}

	void Texture::setPixel(unsigned int _x, unsigned int _y, Color _color)
	{
		assertCurrentContext();
		assertBound("setPixel()");

		unsigned char pixel_data[] = {
			static_cast<unsigned char>(_color.red),
			static_cast<unsigned char>(_color.green),
			static_cast<unsigned char>(_color.blue),
			static_cast<unsigned char>(_color.alpha)
		};
		setPixels(_x, _y, 1, 1, pixel_data);
	}

	void Texture::setPixels(unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned char* _data)
	{
		assertCurrentContext();
		assertBound("setPixels()");
		
		_y = m_height - _y - _height;

		GLenum data_format = GL_RGB;
		switch(m_channels)
		{
			case 1:
				data_format = GL_RED;
				break;
			case 2:
				data_format = GL_RG;
				break;
			case 3:
				data_format = GL_RGB;
				break;
			case 4:
				data_format = GL_RGBA;
				break;
			default:
				throw Renderer::InvalidFormat("Invalid channel count. There are only 1, 2, 3, or 4 channels!");
				break;
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, _x, _y, _width, _height, data_format, GL_UNSIGNED_BYTE, _data);
	}

	void Texture::bind(unsigned int _slot)
	{
		assertCurrentContext();

		if(s_activeSlot != _slot)
			glActiveTexture(GL_TEXTURE0 + _slot);
		s_activeSlot = _slot;

		glBindTexture(GL_TEXTURE_2D, m_textureId);

		// ensure there are enough space in the std::vector
		while(s_boundedTextures.size() <= _slot)
			s_boundedTextures.push_back(nullptr);

		s_boundedTextures.at(_slot) = this;
	}

	bool Texture::isBound(unsigned int _slot) const
	{
		if(s_boundedTextures.size() <= _slot) return false;
		return s_boundedTextures.at(_slot) == this;
	}

	void Texture::setTextureWrap(GLenum _wrapX, GLenum _wrapY)
	{
		m_textureWrapS = _wrapX;
		m_textureWrapT = _wrapY;
	}

	void Texture::setTextureFilter(GLenum _minFilter, GLenum _magFilter)
	{
		m_textureFilterMin = _minFilter;
		m_textureFilterMag = _magFilter;
	}

	void Texture::write(const char* _path, TextureType _type)
	{
		unsigned char* flip_data = new unsigned char[m_width * m_height * m_channels];
		memcpy(flip_data, m_data, sizeof(unsigned char) * m_width * m_height * m_channels);
		flipColorVertically(m_width, m_height, m_channels, flip_data);

		int result = 0;
		switch(_type)
		{
			case TextureType::PNG:
				result = stbi_write_png(_path, m_width, m_height, m_channels, flip_data, m_width * m_channels);
				break;
			case TextureType::JPG:
				result = stbi_write_jpg(_path, m_width, m_height, m_channels, flip_data, 100);
				break;
			default:
				break;
		}

		if(result == 0)
			throw FileNotFoundException("Unable to write to file: " + std::string(_path) + "!");

		delete[] flip_data;
	}

	GLenum Texture::getInternalFormat()
	{
		if(m_channels == 1)
		{
			switch(m_channelSize)
			{
				case 8:
					return GL_R8;
					break;
				case 16:
					return GL_R16;
					break;
				case 32:
					return GL_R32F;
					break;
				default:
					throw Renderer::InvalidFormat("Invalid texture bits. Must be 8 bits, 16 bits, or 32 bits!");
					break;
			}
		}

		if(m_channels == 2)
		{
			switch(m_channelSize)
			{
				case 8:
					return GL_RG8;
					break;
				case 16:
					return GL_RG16;
					break;
				case 32:
					return GL_RG32F;
					break;
				default:
					throw Renderer::InvalidFormat("Invalid texture bits. Must be 8 bits, 16 bits, or 32 bits!");
					break;
			}
		}

		if(m_channels == 3)
		{
			switch(m_channelSize)
			{
				case 8:
					return GL_RGB8;
					break;
				case 16:
					return GL_RGB16;
					break;
				case 32:
					return GL_RGB32F;
					break;
				default:
					throw Renderer::InvalidFormat("Invalid texture bits. Must be 8 bits, 16 bits, or 32 bits!");
					break;
			}
		}

		if(m_channels == 4)
		{
			switch(m_channelSize)
			{
				case 8:
					return GL_RGBA8;
					break;
				case 16:
					return GL_RGBA16;
					break;
				case 32:
					return GL_RGBA32F;
					break;
				default:
					throw Renderer::InvalidFormat("Invalid texture bits. Must be 8 bits, 16 bits, or 32 bits!");
					break;
			}
		}

		throw Renderer::InvalidFormat("Invalid channel count. There are only 1, 2, 3, or 4 channels!");
	}

	void Texture::assertBound(const char* _func)
	{
		if(s_boundedTextures.size() <= s_activeSlot)
			throw Renderer::TextureOperationRejected("Texture must be bound before using ." + std::string(_func) + "!");

		if(s_boundedTextures.at(s_activeSlot) == this)
			return;

		if(m_autobind)
		{
			bind(s_activeSlot);
			return;
		}

		throw Renderer::TextureOperationRejected("Texture must be bound before using ." + std::string(_func) + "!");
	}

	void Texture::assertCurrentContext()
	{
		if(m_window->isCurrentContext())
			return;

		if(m_window->willAutoMakeCurrent())
		{
			m_window->makeCurrent();
			return;
		}

		throw Renderer::InvalidWindowContext("The corresponding window must be made current first!");
	}

	Texture::~Texture()
	{
		if(m_channels == 0)
			return;

		glDeleteTextures(1, &m_textureId);

		if(m_fromFile)
			stbi_image_free(m_data);
		else
			delete[] m_data;
	}
	
	bool operator==(const Color& _lhs, const Color& _rhs)
	{
		if(_lhs.red != _rhs.red) return false;
		if(_lhs.green != _rhs.green) return false;
		if(_lhs.blue != _rhs.blue) return false;
		if(_lhs.alpha != _rhs.alpha) return false;

		return true;
	}

	bool operator!=(const Color& _lhs, const Color& _rhs)
	{
		if(_lhs.red != _rhs.red) return true;
		if(_lhs.green != _rhs.green) return true;
		if(_lhs.blue != _rhs.blue) return true;
		if(_lhs.alpha != _rhs.alpha) return true;

		return false;
	}

	std::ostream& operator<<(std::ostream& _os, const Color& _color)
	{
		_os << "Color( " << _color.red << " " << _color.green << " " << _color.blue << " " << _color.alpha << " )";
		return _os;
	}

	unsigned char* colorToUCharPtr(unsigned int _channels, unsigned int _count, const Color* _color)
	{
		if(_channels < 1 || _channels > 4)
			throw Renderer::InvalidFormat("Invalid channel count. There are only 1, 2, 3, or 4 channels!");

		unsigned char* color_data = new unsigned char[_count * _channels];

		for(unsigned int i=0;i<_count;++i)
		{
			*(color_data + i * _channels) = static_cast<unsigned char>(_color[i].red);
			if(_channels < 2) continue;
			*(color_data + i * _channels + 1) = static_cast<unsigned char>(_color[i].green);
			if(_channels < 3) continue;
			*(color_data + i * _channels + 2) = static_cast<unsigned char>(_color[i].blue);
			if(_channels < 4) continue;
			*(color_data + i * _channels + 3) = static_cast<unsigned char>(_color[i].alpha);
		}
		
		return color_data;
	}

	void flipColorVertically(unsigned int _width, unsigned int _height, unsigned int _channels, unsigned char* _data)
	{
		unsigned char* track_row = new unsigned char[_width * _channels];

		int half_height = _height / 2;
		for(int i=0;i<half_height;++i)
		{
			int row_start_index = i * _width * _channels;
			int corresponding_row = (_height - i - 1) * _width * _channels;
			
			// copy the current row into a temporary buffer
			memcpy(track_row, _data + row_start_index, sizeof(unsigned char) * _width * _channels);
			// copy the corresponding row to the current row
			memcpy(_data + row_start_index, _data + corresponding_row, sizeof(unsigned char) * _width * _channels);
			// copy the temporary buffer to the corresponding row
			memcpy(_data + corresponding_row, track_row, sizeof(unsigned char) * _width * _channels);
		}

		delete[] track_row;
	}
}
