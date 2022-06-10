#pragma once

class BaseScene;

class TestScene : public BaseScene
{
private:
	sf::CircleShape shape;
	sf::RenderTexture _texture;
	sf::Sprite _sprite;
	sf::VertexArray _gfxPixels;
		
public:
	TestScene(int width, int height, std::string title)
		:BaseScene{ width, height, title }
	{
		shape.setRadius(100.0f);
		shape.setFillColor(sf::Color::Green);
		shape.setPosition(halfWidth - 100.0f, halfHeight - 100.0f);	

		if (!_texture.create(width, height))
			std::cout << "Failed to create _texture!" << std::endl;

		_gfxPixels.setPrimitiveType(sf::Quads);
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
		_texture.clear();

		// begin - "draw" the texture
		_gfxPixels.clear();		
		sf::Vertex v1{ sf::Vector2f{100.0f, 100.0f} };
		sf::Vertex v2{ sf::Vector2f{200.0f, 100.0f} };
		sf::Vertex v3{ sf::Vector2f{200.0f, 200.0f} };
		sf::Vertex v4{ sf::Vector2f{100.0f, 200.0f} };
		_gfxPixels.append(v1); _gfxPixels.append(v2); _gfxPixels.append(v3); _gfxPixels.append(v4);		
		// end - "draw" the texture

		_texture.draw(_gfxPixels);
		_texture.display();
		_sprite.setTexture(_texture.getTexture());
		window.draw(_sprite);

		window.draw(shape);
	}

	std::vector<std::string> getOverlayMessages()
	{
		std::vector<std::string> msg;
		
		return msg;
	}
};