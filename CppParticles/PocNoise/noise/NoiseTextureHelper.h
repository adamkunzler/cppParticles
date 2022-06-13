#ifndef NOISETEXTUREHELPER_H
#define NOISETEXTUREHELPER_H

#include "SimplexNoise.h"

struct NoiseSettings
{
	int octaves{ 6 };
	float redistribution{ 3.0f };

	float frequency{ 1.f };
	float amplitude{ 1.f };

	float xPosition{ 0 };
	float yPosition{ 0 };

	float zoom{ 1.f };
	float scale{ 0.015f };
};

class NoiseTextureHelper
{
private:
	std::unique_ptr<OpenSimplexNoise> _noise;

public:
	NoiseTextureHelper(long noiseSeed = 0)
	{
		_noise = std::make_unique<OpenSimplexNoise>(noiseSeed);
	};

	void generate(sf::Image& image, sf::Texture& texture, int width, int height, NoiseSettings settings)
	{
		//sf::Clock clock;

		float octaveOffset = 10.f;
		float scale = settings.scale;
		float halfWidth = (width / 2.f);
		float halfHeight = (height / 2.f);

		unsigned int pixelX{ 0 };
		unsigned int pixelY{ 0 };

		double zoomStep = (settings.zoom - 1.0) * 10.0;

		double startX{ (settings.xPosition - halfWidth) + zoomStep };
		double endX{ (settings.xPosition + halfWidth) - zoomStep };

		double startY{ (settings.yPosition - halfHeight) + zoomStep };
		double endY{ (settings.yPosition + halfHeight) - zoomStep };

		double xStep = (endX - startX) / width;
		double yStep = (endY - startY) / height;

		

		for (double y = startY; y < endY; y += yStep)
		{
			for (double x = startX; x < endX; x += xStep)
			{
				double n{ 0.0 };

				float frequency{ settings.frequency };
				float amplitude{ settings.amplitude };
				float sumAmplitude{ 0 };

				double nx = x * scale;
				double ny = y * scale;

				for (int octave = 0; octave < settings.octaves; ++octave)
				{
					double noiseX{ (nx + (octaveOffset * octave)) };
					double noiseY{ (ny + (octaveOffset * octave)) };
					n += (_noise->eval(frequency * noiseX, frequency * noiseY) * amplitude);

					sumAmplitude += amplitude;

					frequency *= 2.f;
					amplitude /= 2.f;
				}

				n /= sumAmplitude;
				float n2 = (n + 1) / 2.f; // between 0 and 1 (instead of -1 and 1)
				n = std::powf(n2 * 1.2f, settings.redistribution);

				sf::Uint8 val = static_cast<sf::Uint8>(mapRange(n, 0, 1, 0, 255));

				sf::Color color{ val, val, val, 255 };
				if (pixelX < width && pixelY < height)
					image.setPixel(pixelX, pixelY, color);

				pixelX++;
			}
			pixelX = 0;
			pixelY++;
		}

		texture.update(image);

		//sf::Time elapsed = clock.restart();
		//std::cout << "noise gen took " << elapsed.asMilliseconds() << " ms." << std::endl;
	}

	void generate2(sf::Image& image, sf::Texture& texture, int width, int height, NoiseSettings settings)
	{
		//sf::Clock clock;

		float octaveOffset = 10.f;
		float scale = settings.scale;
		float halfWidth = width / 2.f;
		float halfHeight = height / 2.f;

		for (double y = 0; y < height; ++y)
		{
			for (double x = 0; x < width; ++x)
			{
				double n{ 0.0 };

				float frequency{ settings.frequency };
				float amplitude{ settings.amplitude };
				float sumAmplitude{ 0 };

				double nx = ((settings.xPosition + x - halfWidth) * scale) / settings.zoom;
				double ny = ((settings.yPosition + y - halfHeight) * scale) / settings.zoom;

				for (int octave = 0; octave < settings.octaves; ++octave)
				{
					double noiseX{ (nx + (octaveOffset * octave)) };
					double noiseY{ (ny + (octaveOffset * octave)) };
					n += (_noise->eval(frequency * noiseX, frequency * noiseY) * amplitude);

					sumAmplitude += amplitude;

					frequency *= 2.f;
					amplitude /= (2.f);
				}

				n /= sumAmplitude;
				float n2 = (n + 1) / 2.f; // between 0 and 1 (instead of -1 and 1)
				n = std::powf(n2 * 1.2f, settings.redistribution);

				sf::Uint8 val = static_cast<sf::Uint8>(mapRange(n, 0, 1, 0, 255));

				sf::Color color{ val, val, val, 255 };
				image.setPixel(x, y, color);
			}
		}

		texture.update(image);

		//sf::Time elapsed = clock.restart();
		//std::cout << "noise gen took " << elapsed.asMilliseconds() << " ms." << std::endl;
	}
};

#endif