#ifndef PARTICLESCENE_H
#define PARTICLESCENE_H

#include "physics/Particle.h";

class ParticleScene : public BaseScene
{
private:
	sf::RenderTexture _texture;
	sf::Sprite _sprite;
	sf::VertexArray _gfxPixels;

	sf::Texture _smokeTexture;
	sf::RenderStates _renderState;
	
	sf::Vector2f _mousePosition{ halfWidth, halfHeight };

	RandomGenerator _rand;

	const float PI = 3.1415926f;
	float MAX_LIFETIME{ 4.f };
	float MAX_VELOCITY{ 900.0f };	
	float FRICTION{ 0.999f };
	float MAX_SIZE{ 50.f };
	float MAX_ANGULAR_VELOCITY{ 50.f };
	float TARGET_ANGLE{ PI };
	float ANGLE_OFFSET{ PI / 4.f };
	int EMIT_SPEED{ 50 };
	bool RENDER_AS_TEXTURE{ false };

	std::vector<std::shared_ptr<Particle>> _particles;

	sf::Time _emitLimiter{ sf::milliseconds(EMIT_SPEED) };
	sf::Time _emitCounter;
	bool _isEmitting{ false };
	

	void initParticle()
	{								
		float angle = (float)_rand.getNormal(-ANGLE_OFFSET, ANGLE_OFFSET);

		float vx = std::sinf(TARGET_ANGLE + angle) * MAX_VELOCITY;
		float vy = std::cosf(TARGET_ANGLE + angle) * MAX_VELOCITY;
		
		sf::Vector2f velocity{ vx, vy };
		
		float lifetime = (float)_rand.getNormal(0.05f, MAX_LIFETIME);

		auto p = std::make_shared<Particle>(_mousePosition, velocity, sf::seconds(lifetime));
		p->friction() = FRICTION;
		p->size() = MAX_SIZE;
		p->acceleration().y = -0.1f;
		p->rotationAngle() = (float)_rand.getNormal(0.f, 360.f);	
		p->angularVelocity() = (float)_rand.getNormal(-MAX_ANGULAR_VELOCITY, MAX_ANGULAR_VELOCITY);

		_particles.push_back(p);
	}

public:
	ParticleScene(int width, int height, std::string title)
		:BaseScene{ width, height, title }
	{
		if (!_texture.create(width, height))
			std::cout << "Failed to create _texture!" << std::endl;
		
		if (!_smokeTexture.loadFromFile("assets/smoke.png"))
			std::cout << "Failed to create _smokeTexture!" << std::endl;					

		_gfxPixels.setPrimitiveType(sf::Quads);
				
		_renderState.texture = &_smokeTexture;
		_renderState.blendMode = sf::BlendAlpha;		
	}

	void processSceneEvents(const sf::Event& event)
	{		
		if (event.type == sf::Event::MouseMoved)
		{
			_mousePosition.x = event.mouseMove.x;
			_mousePosition.y = event.mouseMove.y;
		}

		if (event.type == sf::Event::MouseButtonPressed)
			_isEmitting = true;

		if (event.type == sf::Event::MouseButtonReleased)
			_isEmitting = false;
	}

	void updateScene(sf::Time elapsed) override
	{
		_emitCounter += elapsed;

		for (auto p : _particles)
		{
			p->update(elapsed);					
		}
		
		// remove dead particles
		auto removeStart = std::remove_if(_particles.begin(), _particles.end(), [](const std::shared_ptr<Particle> p) { return !p->isAlive(); });
		_particles.erase(removeStart, _particles.end());		

		// init new particles if isEmitting and emit timer is good
		if (_isEmitting && _emitCounter > _emitLimiter)
		{
			_emitCounter = sf::milliseconds(0);
			_emitLimiter = sf::milliseconds(EMIT_SPEED);
			initParticle();
		}		
	}

	void drawScene(sf::Time elapsed) override
	{		
		_texture.clear();

		// begin - "draw" the texture
		_gfxPixels.clear();

		for (const auto p : _particles)
		{	
			auto ratio = static_cast<uint8_t>((p->lifetime() / MAX_LIFETIME) * 255.f);
			sf::Uint8 val{ ratio };
			sf::Color color{ val, val, val, 100 };
			
			float x{ p->position().x };
			float y{ p->position().y };
						
			sf::Transform translation;
			translation.translate(x, y);
			
			sf::Transform rotation;
			rotation.rotate(p->rotationAngle());

			sf::Transform transform = translation * rotation;

			float tx = (float)_smokeTexture.getSize().x;
			float ty = (float)_smokeTexture.getSize().y;

			sf::Vector2f point1{ transform.transformPoint( -p->size(),  -p->size())};
			sf::Vector2f textCoord1{ 0.f,  0.f };

			sf::Vector2f point2{ transform.transformPoint(p->size(),  -p->size()) };
			sf::Vector2f textCoord2{ tx, 0.f};
			
			sf::Vector2f point3{ transform.transformPoint(p->size(),  p->size()) };
			sf::Vector2f textCoord3{ tx, ty };

			sf::Vector2f point4{ transform.transformPoint( -p->size(),  p->size()) };
			sf::Vector2f textCoord4{ 0.f, ty };
			
			sf::Vertex vert1{ point1, color, textCoord1 };
			sf::Vertex vert2{ point2, color, textCoord2 };
			sf::Vertex vert3{ point3, color, textCoord3 };
			sf::Vertex vert4{ point4, color, textCoord4 };
			
			_gfxPixels.append(vert1); _gfxPixels.append(vert2); _gfxPixels.append(vert3); _gfxPixels.append(vert4);
		}
		// end - "draw" the texture
				
		if(RENDER_AS_TEXTURE)
			_texture.draw(_gfxPixels, _renderState);
		else
			_texture.draw(_gfxPixels);
		
		_texture.display();
		_sprite.setTexture(_texture.getTexture());
		window.draw(_sprite);
	}

	std::vector<std::string> getOverlayMessages()
	{
		std::vector<std::string> msg;

		msg.push_back("# of Particles: " + std::to_string(_particles.size()));

		return msg;
	}

	void customImGui()
	{		
		ImGui::Begin("Settings");
		ImGui::SetWindowSize("Settings", ImVec2{ 500, 250 });
		
		ImGui::SliderFloat("Friction", &FRICTION, 0.f, 1.f);
		ImGui::SliderFloat("Max Lifetime", &MAX_LIFETIME, 0.05f, 10.f);
		ImGui::SliderFloat("Max Velocity", &MAX_VELOCITY, 10.f, 2500.f);				
		ImGui::SliderFloat("Max Size", &MAX_SIZE, 10.f, 100.f);		
		ImGui::SliderFloat("Target Angle", &TARGET_ANGLE, 0.f, PI * 2.f);
		ImGui::SliderFloat("Angle Offset", &ANGLE_OFFSET, 0.f, PI * 2.f);
		ImGui::SliderInt("Emit Speed", &EMIT_SPEED, 0, 100);
		ImGui::Checkbox("Render as Texture", &RENDER_AS_TEXTURE);
		ImGui::End();		
	}
};

#endif