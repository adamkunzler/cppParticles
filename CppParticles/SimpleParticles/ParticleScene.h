#ifndef PARTICLESCENE_H
#define PARTICLESCENE_H

#include "physics/Particle.h";
#include "physics/ParticleEmitter.h"

class ParticleScene : public BaseScene
{
private:

	sf::RenderStates _renderState;

	sf::Vector2f _mousePosition{ halfWidth, halfHeight };

	ParticleEmitter _emitter;
	// 1000x1700
	ParticleEmitter _emitter2;

public:
	ParticleScene(int width, int height, std::string title)
		: BaseScene{ width, height, title },
		_emitter{ width, height, "mouse position" },
		_emitter2{ width, height, "static" }
	{
		_emitter2.position().x = 1000.f;
		_emitter2.position().y = 1700.f;
		_emitter2.isEmitting() = true;
		
		_renderState.blendMode = sf::BlendAdd;
	}

	void processSceneEvents(const sf::Event& event)
	{
		if (event.type == sf::Event::MouseMoved)
		{
			_mousePosition.x = event.mouseMove.x;
			_mousePosition.y = event.mouseMove.y;

			_emitter.position().x = event.mouseMove.x;
			_emitter.position().y = event.mouseMove.y;
		}

		if (event.type == sf::Event::MouseButtonPressed)
			_emitter.isEmitting() = true;

		if (event.type == sf::Event::MouseButtonReleased)
			_emitter.isEmitting() = false;
	}

	void updateScene(sf::Time elapsed) override
	{
		_emitter.update(elapsed);
		_emitter2.update(elapsed);
	}

	void drawScene(sf::Time elapsed) override
	{
		_emitter.render(window, _renderState);
		_emitter2.render(window, _renderState);
	}

	std::vector<std::string> getOverlayMessages()
	{
		std::vector<std::string> msg;

		std::size_t totalParticles{ _emitter.particleCount() + _emitter2.particleCount() };
		msg.push_back("# of Particles: " + std::to_string(totalParticles));

		return msg;
	}

	void customImGui()
	{
		_emitter.customImGui();
		_emitter2.customImGui();

	}
};

#endif