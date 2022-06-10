#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include "Particle.h"

struct ParticleEmitterSettings
{
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
	float BASE_COLOR[3]{ 1.f, 1.f, 1.f };
	int TRANSPARENCY{ 100 };
	
};

class ParticleEmitter
{
private:
	sf::RenderTexture _texture;
	sf::Sprite _sprite;
	sf::VertexArray _gfxPixels;

	ParticleEmitterSettings _settings;
	RandomGenerator _rand;

	sf::Texture _smokeTexture;
	sf::RenderStates _renderState;

	std::vector<std::shared_ptr<Particle>> _particles;

	sf::Time _emitLimiter{ sf::milliseconds(_settings.EMIT_SPEED) };
	sf::Time _emitCounter;
	bool _isEmitting{ false };

	sf::Vector2f _position;

	std::string _id;

	void initParticle()
	{
		float angle = (float)_rand.getNormal(-_settings.ANGLE_OFFSET, _settings.ANGLE_OFFSET);

		float vx = std::sinf(_settings.TARGET_ANGLE + angle) * _settings.MAX_VELOCITY;
		float vy = std::cosf(_settings.TARGET_ANGLE + angle) * _settings.MAX_VELOCITY;

		sf::Vector2f velocity{ vx, vy };

		float lifetime = (float)_rand.getNormal(0.05f, _settings.MAX_LIFETIME);

		auto p = std::make_shared<Particle>(_position, velocity, sf::seconds(lifetime));
		p->friction() = _settings.FRICTION;
		p->size() = _settings.MAX_SIZE;
		p->acceleration().y = -0.1f;
		p->rotationAngle() = (float)_rand.getNormal(0.f, 360.f);
		p->angularVelocity() = (float)_rand.getNormal(-_settings.MAX_ANGULAR_VELOCITY, _settings.MAX_ANGULAR_VELOCITY);

		_particles.push_back(p);
	}

public:
	bool& isEmitting() { return _isEmitting; }
	sf::Vector2f& position() { return _position; }

	std::size_t particleCount() { return _particles.size(); }

	ParticleEmitter(int windowWidth, int windowHeight, std::string id)
		:_id{ id }
	{
		if (!_texture.create(windowWidth, windowHeight))
			std::cout << "Failed to create _texture!" << std::endl;

		_gfxPixels.setPrimitiveType(sf::Quads);

		if (!_smokeTexture.loadFromFile("assets/smoke.png"))
			std::cout << "Failed to create _smokeTexture!" << std::endl;

		_renderState.texture = &_smokeTexture;
		_renderState.blendMode = sf::BlendAlpha;
	}

	void update(sf::Time elapsed)
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
			_emitLimiter = sf::milliseconds(_settings.EMIT_SPEED);
			initParticle();
		}
	}

	void render(sf::RenderWindow& window, sf::RenderStates& windowRenderState)
	{
		_texture.clear();

		// begin - "draw" the texture
		_gfxPixels.clear();

		for (const auto p : _particles)
		{
			auto ratio = p->lifetime() / _settings.MAX_LIFETIME;
			sf::Uint8 red{ static_cast<uint8_t>(_settings.BASE_COLOR[0] * 255 * ratio) };
			sf::Uint8 green{ static_cast<uint8_t>(_settings.BASE_COLOR[1] * 255 * ratio) };
			sf::Uint8 blue{ static_cast<uint8_t>(_settings.BASE_COLOR[2] * 255 * ratio) };
			sf::Uint8 alpha{ static_cast<uint8_t>(_settings.TRANSPARENCY) };
			sf::Color color{ red, green, blue, alpha };

			float x{ p->position().x };
			float y{ p->position().y };

			sf::Transform translation;
			translation.translate(x, y);

			sf::Transform rotation;
			rotation.rotate(p->rotationAngle());

			sf::Transform transform = translation * rotation;

			float tx = (float)_smokeTexture.getSize().x;
			float ty = (float)_smokeTexture.getSize().y;

			sf::Vector2f point1{ transform.transformPoint(-p->size(),  -p->size()) };
			sf::Vector2f textCoord1{ 0.f,  0.f };

			sf::Vector2f point2{ transform.transformPoint(p->size(),  -p->size()) };
			sf::Vector2f textCoord2{ tx, 0.f };

			sf::Vector2f point3{ transform.transformPoint(p->size(),  p->size()) };
			sf::Vector2f textCoord3{ tx, ty };

			sf::Vector2f point4{ transform.transformPoint(-p->size(),  p->size()) };
			sf::Vector2f textCoord4{ 0.f, ty };

			sf::Vertex vert1{ point1, color, textCoord1 };
			sf::Vertex vert2{ point2, color, textCoord2 };
			sf::Vertex vert3{ point3, color, textCoord3 };
			sf::Vertex vert4{ point4, color, textCoord4 };

			_gfxPixels.append(vert1); _gfxPixels.append(vert2); _gfxPixels.append(vert3); _gfxPixels.append(vert4);
		}
		// end - "draw" the texture

		if (_settings.RENDER_AS_TEXTURE)
			_texture.draw(_gfxPixels, _renderState);
		else
			_texture.draw(_gfxPixels);

		_texture.display();
		_sprite.setTexture(_texture.getTexture());

		window.draw(_sprite, windowRenderState);
	}

	void customImGui()
	{
		std::string imguiTitle = "Emitter: " + _id;
		ImGui::Begin(imguiTitle.c_str());
		ImGui::SetWindowSize("Settings", ImVec2{ 500, 250 });

		ImGui::SliderFloat("Friction", &_settings.FRICTION, 0.f, 1.f);
		ImGui::SliderFloat("Max Lifetime", &_settings.MAX_LIFETIME, 0.05f, 10.f);
		ImGui::SliderFloat("Max Velocity", &_settings.MAX_VELOCITY, 10.f, 2500.f);
		ImGui::SliderFloat("Max Size", &_settings.MAX_SIZE, 10.f, 100.f);
		ImGui::SliderFloat("Target Angle", &_settings.TARGET_ANGLE, 0.f, _settings.PI * 2.f);
		ImGui::SliderFloat("Angle Offset", &_settings.ANGLE_OFFSET, 0.f, _settings.PI * 2.f);
		ImGui::SliderInt("Emit Speed", &_settings.EMIT_SPEED, 0, 100);
		ImGui::Checkbox("Render as Texture", &_settings.RENDER_AS_TEXTURE);		
		ImGui::SliderInt("Transparency", &_settings.TRANSPARENCY, 0, 255);
		ImGui::ColorEdit3("Color", _settings.BASE_COLOR);
		ImGui::End();
	}
};
#endif