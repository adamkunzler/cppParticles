#pragma once

#include "noise/SimplexNoise.h"
#include "noise/NoiseTextureHelper.h"

class BaseScene;

class TestScene : public BaseScene
{
private:
	NoiseTextureHelper _noiseTextureHelper;

	sf::Image _noiseImage;
	sf::Sprite _noiseSprite;
	sf::Texture _noiseTexture;

	int dim{ 256 };
	float scale{ 1024.f / (float)dim };

	int dimWidth{ dim };
	int dimHeight{ dim };

	NoiseSettings _settings;

	bool _canDragMap{ true };
	bool _mouseIsDown{ false };
	sf::Vector2f _mouseMovePosition;

	void generateNoiseTexture()
	{
		_noiseTextureHelper.generate(_noiseImage, _noiseTexture, dimWidth, dimHeight, _settings);
	}

public:
	TestScene(int width, int height, std::string title)
		:BaseScene{ width, height, title }
	{
		if (!_noiseTexture.create(dimWidth, dimHeight))
			std::cout << "Failed to create noise texture!" << std::endl;

		_noiseSprite.setTexture(_noiseTexture);

		_noiseImage.create(dimWidth, dimHeight, sf::Color::Black);

		generateNoiseTexture();

		//_noiseSprite.setPosition(100, 100);
		_noiseSprite.setScale(scale, scale);
	}

	void processSceneEvents(const sf::Event& event)
	{
		if (event.type == sf::Event::MouseButtonPressed)
		{
			_mouseIsDown = true;
			_mouseMovePosition.x = mousePosition().x;
			_mouseMovePosition.y = mousePosition().y;
		}
		else if (event.type == sf::Event::MouseButtonReleased)
		{
			_mouseIsDown = false;
		}
		else if (event.type == sf::Event::MouseMoved)
		{
			if (!_canDragMap) return;
			if (!_mouseIsDown) return;

			float dx = (_mouseMovePosition.x - (float)event.mouseMove.x) / scale;
			float dy = (_mouseMovePosition.y - (float)event.mouseMove.y) / scale;

			_settings.xPosition += dx;
			_settings.yPosition += dy;

			_mouseMovePosition.x = event.mouseMove.x;
			_mouseMovePosition.y = event.mouseMove.y;

			//std::cout << "dx, dy: " << dx << ", " << dy << std::endl;
		}
		else if (event.type == sf::Event::MouseWheelScrolled)
		{
			if (event.mouseWheelScroll.delta > 0)
			{
				_settings.zoom += 0.1f;
			}
			else if (event.mouseWheelScroll.delta < 0)
			{
				_settings.zoom -= 0.1f;
				if (_settings.zoom < 1.f) _settings.zoom = 1.f;
			}
		}
	}

	void updateScene(sf::Time elapsed) override
	{
		generateNoiseTexture();
	}

	void drawScene(sf::Time elapsed) override
	{
		window.draw(_noiseSprite);
	}

	std::vector<std::string> getOverlayMessages()
	{
		std::vector<std::string> msg;

		return msg;
	}

	void customImGui() override
	{
		std::string imguiTitle = "Settings";
		ImGui::Begin(imguiTitle.c_str());
		ImGui::SetWindowSize("Settings", ImVec2{ 500, 250 });

		ImGui::SliderInt("Octaves", &_settings.octaves, 1, 6);
		ImGui::SliderFloat("Frequency", &_settings.frequency, 1.f, 10.f);
		ImGui::SliderFloat("Redistribution", &_settings.redistribution, 0.01f, 10.f);
		ImGui::SliderFloat("Zoom", &_settings.zoom, 1.0f, 100.f);
		ImGui::Checkbox("Drag Map", &_canDragMap);
		ImGui::SliderFloat("Sclae", &_settings.scale, 0.001f, 0.02f);

		ImGui::End();
	}
};