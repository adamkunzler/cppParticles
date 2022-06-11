#pragma once

class BaseScene;

class TestScene : public BaseScene
{
private:
	sf::RenderTexture _texture;
	sf::Sprite _sprite;
	sf::VertexArray _gfxPixels;

	float _x{ halfWidth };
	float _y{ halfHeight };
	float _size{ 100.f };
	float _angle{ 0.f };

public:
	TestScene(int width, int height, std::string title)
		:BaseScene{ width, height, title }
	{
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
		_angle += 72.f * elapsed.asSeconds();
		if (_angle > 360.f) _angle -= 360.f;
	}

	void drawScene(sf::Time elapsed) override
	{
		_texture.clear();

		// begin - "draw" the texture
		_gfxPixels.clear();

		sf::Transform translation;
		translation.translate(_x, _y);
		sf::Transform rotation;
		rotation.rotate(_angle);
		sf::Transform transform = translation * rotation;

		sf::Vertex v1{ sf::Vector2f{transform.transformPoint(-_size, -_size)} };
		sf::Vertex v2{ sf::Vector2f{transform.transformPoint(_size, -_size)} };
		sf::Vertex v3{ sf::Vector2f{transform.transformPoint(_size, _size)} };
		sf::Vertex v4{ sf::Vector2f{transform.transformPoint(-_size, _size)} };
		_gfxPixels.append(v1); _gfxPixels.append(v2); _gfxPixels.append(v3); _gfxPixels.append(v4);
		// end - "draw" the texture

		_texture.draw(_gfxPixels);
		_texture.display();
		_sprite.setTexture(_texture.getTexture());
		window.draw(_sprite);
	}

	std::vector<std::string> getOverlayMessages()
	{
		std::vector<std::string> msg;

		return msg;
	}

	void customImGui()
	{
	}
};