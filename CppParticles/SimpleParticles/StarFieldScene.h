#pragma once

class BaseScene;

enum class RenderMode
{
	Points,
	Quads,
	Lines
};

class StarFieldScene : public BaseScene
{
private:
	sf::RenderTexture _texture;
	sf::Sprite _sprite;
	sf::VertexArray _gfxPixels;

	RandomGenerator _rand;

	unsigned int _numStars{ 5000 };
	float _starSpeed{ 800.0f };
	float _starSize{ 5.0f };
	float _starZDepth{ 2000.0f };
	RenderMode _renderMode{ RenderMode::Points };

	std::vector<sf::Vector3f> _stars;

	bool _isStep = false;

public:
	StarFieldScene(int width, int height, std::string title)
		:BaseScene{ width, height, title }
	{
		if (!_texture.create(width, height))
			std::cout << "Failed to create _texture!" << std::endl;

		_gfxPixels.setPrimitiveType(sf::Points);
		
		spawnStars();
	}

	void spawnStars()
	{		
		std::cout << "Spawning " << _numStars << " stars..." << std::endl;

		for (size_t i = 0; i < _numStars; ++i)
			_stars.push_back(spawnStar());
		
		std::cout << "\t...done." << std::endl;		
	}

	sf::Vector3f spawnStar()
	{
		auto star = sf::Vector3f{
				(float)_rand.getUniform(-halfWidth, halfWidth),
				(float)_rand.getUniform(-halfHeight, halfHeight),
				(float)_rand.getUniform() * _starZDepth,
		};
		return star;
	}

	void processSceneEvents(const sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
				case sf::Keyboard::Space:				
					_isStep = true;
					break;
				
				case sf::Keyboard::Num1:					
					_renderMode = RenderMode::Points;
					_gfxPixels.setPrimitiveType(sf::Points);
					break;
				
				case sf::Keyboard::Num2:				
					_renderMode = RenderMode::Quads;
					_gfxPixels.setPrimitiveType(sf::Quads);
					break;
				
				case sf::Keyboard::Num3:				
					_renderMode = RenderMode::Lines;
					_gfxPixels.setPrimitiveType(sf::Lines);
					break;
				
				case sf::Keyboard::Up:				
					_starSpeed += 50;
					break;
				
				case sf::Keyboard::Down:				
					_starSpeed -= 50;
					break;

				case sf::Keyboard::Add:					
					for (size_t i = 0; i < 10; ++i)
						_stars.push_back(spawnStar());
					break;

				case sf::Keyboard::Subtract:
					for (size_t i = 0; i < 10; ++i)
					{
						if (_stars.size() == 0) continue;
						_stars.pop_back();
					}
					break;				
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
		//if (!_isStep) return;

		_gfxPixels.clear();

		// move stars								
		float distance = elapsed.asSeconds() * _starSpeed;
		for (auto& star : _stars) {

			// update z position
			star.z -= distance;
			while (star.z <= 1.0f)
				star.z += _starZDepth;

			// calculate x/y
			float dscale = 1.0f / _starZDepth;
			float x = halfWidth + star.x / (star.z * dscale);
			float y = halfHeight + star.y / (star.z * dscale);

			// check bounds
			if (x < 0 || x >= width || y < 0 || y >= height) {
				continue;
			}

			// calculate a distance/brightness for the star (to set color)
			float dist = (star.z / _starZDepth);
			uint8_t brightness = (1 - dist * dist) * 255;
			
			switch (_renderMode)
			{
			case RenderMode::Points:
				drawStarAsPoint(x, y, brightness);
				break;
			case RenderMode::Quads:
				drawStarAsQuad(x, y, star.z, brightness);
				break;
			case RenderMode::Lines:
				drawStarAsLine(star, brightness);
			}					
		}

		_isStep = false;
	}

	void drawStarAsPoint(const float &x, const float &y, const uint8_t &brightness)
	{
		sf::Vertex v{ sf::Vector2f{x, y} };
		v.color = sf::Color{ brightness, brightness, brightness };
		_gfxPixels.append(v);
	}

	void drawStarAsQuad(const float& x, const float& y, const float &z, const uint8_t& brightness) 
	{
		float size = _starSize - mapRange(z, 0.0f, _starZDepth, 0.25f, _starSize);
		float right{ size }; float left{ -size }; float top{ -size }; float bottom{ size };
		
		drawStarAsPoint(x + left, y + top, brightness);		
		drawStarAsPoint(x + right, y + top, brightness);		
		drawStarAsPoint(x + right, y + bottom, brightness);			
		drawStarAsPoint(x + left, y + bottom, brightness);		
	}

	void drawStarAsLine(const sf::Vector3f& star, const uint8_t& brightness)
	{
		float dscale = 1.0f / _starZDepth;
		float x = halfWidth + star.x / (star.z * dscale);
		float y = halfHeight + star.y / (star.z * dscale);

		float z2 = star.z - 100.0f;
		if (z2 < 0.0f) z2 = 0.0f;
		float x2 = halfWidth + star.x / (z2 * dscale);
		float y2 = halfHeight + star.y / (z2 * dscale);

		// check bounds		
		if (x < 0 || x >= width || y < 0 || y >= height)
			return;		
				
		if (x2 < 0 || x2 >= width || y2 < 0 || y2 >= height)
			return;				

		drawStarAsPoint(x, y, brightness);
		drawStarAsPoint(x2, y2, brightness);
	}

	void drawScene(sf::Time elapsed) override
	{
		_texture.clear();		
		_texture.draw(_gfxPixels);
		_texture.display();
		_sprite.setTexture(_texture.getTexture());
		window.draw(_sprite);
	}	

	std::vector<std::string> getOverlayMessages()
	{
		std::vector<std::string> msg;

		msg.push_back("# of Stars: " + std::to_string(_stars.size()));
		msg.push_back("Star Speed: " + std::to_string(_starSpeed));

		return msg;
	}

	void customImGui() 
	{
		//ImGui::ShowDemoWindowWidgets();
	}
};