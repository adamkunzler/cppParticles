#pragma once

#include "utils/SimplexNoise.h"

class BaseScene;

class TestScene : public BaseScene
{
private:	
	sf::RenderTexture _texture;
	sf::Sprite _sprite;
	sf::VertexArray _gfxPixels;
	
	sf::Image _theNoise;
	sf::Sprite _spriteNoise;
	sf::Texture _textureNoise;
		
public:
	TestScene(int width, int height, std::string title)
		:BaseScene{ width, height, title }
	{		
		if (!_texture.create(width, height))
			std::cout << "Failed to create _texture!" << std::endl;

		_gfxPixels.setPrimitiveType(sf::Quads);

		sf::Clock clock;
		OpenSimplexNoise noise{ 0 };
		const int dim = 512;
		std::vector<double> noiseMap(dim * dim);	
		sf::Uint8* pixels = new sf::Uint8[dim * dim * 4];
		double scale = 0.03;
		for (int y = 0; y < dim; ++y)
		{			
			for (int x = 0; x < dim; ++x)
			{
				auto n = noise.eval(x * scale, y * scale);
				noiseMap.push_back(n);

				sf::Uint8 val = static_cast<sf::Uint8>(mapRange(n, -1, 1, 0, 255));

				int index = 4 * (x * dim + y);
				pixels[index] = val; // red
				pixels[index + 1] = val; // green
				pixels[index + 2] = val; // blue
				pixels[index + 3] = 255; // alpha
			}
		}

		if (!_textureNoise.create(dim, dim))
			std::cout << "Failed to create noise texture!" << std::endl;

		//_theNoise.create(dim, dim, pixels); // create the image from pixel data
		_textureNoise.update(pixels); // create texture from image
		_spriteNoise.setTexture(_textureNoise); // create the sprite from the texture

		_spriteNoise.setPosition(100, 100);
		
		delete[] pixels;

		sf::Time elapsed = clock.restart();
		std::cout << "noise gen took " << elapsed.asMilliseconds() << " ms." << std::endl;
	}	

	void processSceneEvents(const sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Space)
			{
				std::cout << "Spacebar Pressed: " << event.key.code << std::endl;
			}			
		}

		if (event.type == sf::Event::MouseMoved)
		{
			/*std::cout << "Mouse X: " << event.mouseMove.x << std::endl;
			std::cout << "Mouse Y: " << event.mouseMove.y << std::endl;*/
		}
	}

	void updateScene(sf::Time elapsed) override
	{
		
	}

	void drawScene(sf::Time elapsed) override
	{				
		//_texture.clear();

		//// begin - "draw" the texture
		//_gfxPixels.clear();		
		//sf::Vertex v1{ sf::Vector2f{100.0f, 100.0f} };
		//sf::Vertex v2{ sf::Vector2f{200.0f, 100.0f} };
		//sf::Vertex v3{ sf::Vector2f{200.0f, 200.0f} };
		//sf::Vertex v4{ sf::Vector2f{100.0f, 200.0f} };
		//_gfxPixels.append(v1); _gfxPixels.append(v2); _gfxPixels.append(v3); _gfxPixels.append(v4);		
		//// end - "draw" the texture

		//_texture.draw(_gfxPixels);
		//_texture.display();
		//_sprite.setTexture(_texture.getTexture());
		
		window.draw(_spriteNoise);

		//window.draw(shape);
	}

	std::vector<std::string> getOverlayMessages()
	{
		std::vector<std::string> msg;
		
		return msg;
	}
};