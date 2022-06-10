#ifndef PARTICLE_H
#define PARTICLE_H

class Particle
{
private:
	sf::Vector2f _position;
	sf::Vector2f _velocity;
	sf::Vector2f _acceleration;

	float _friction{ 1.f };
	float _size{ 1.f };

	float _rotationAngle{ 0.f };
	float _angularVelocity{ 0.f };

	sf::Time _lifetime;
	bool _isAlive{ true };

public:

#pragma region ctor

	Particle(sf::Vector2f position, sf::Vector2f velocity, sf::Time lifetime)
		: _position{ position }, _velocity{ velocity }, _lifetime{ lifetime }, _isAlive{ true }
	{
	}

#pragma endregion ctor

	void update(sf::Time elapsed)
	{
		if (!_isAlive) return;

		_velocity -= (_velocity * _friction * elapsed.asSeconds());

		_position += _velocity * elapsed.asSeconds();
		_velocity += _acceleration;

		_rotationAngle += _angularVelocity * elapsed.asSeconds();

		_size += 75.f * elapsed.asSeconds();

		_lifetime -= elapsed;
		if (_lifetime <= sf::Time::Zero)
			_isAlive = false;
	}

#pragma region Properties

	sf::Vector2f& position() { return _position; }
	sf::Vector2f& velocity() { return _velocity; }
	sf::Vector2f& acceleration() { return _acceleration; }

	bool isAlive() { return _isAlive; }
	float lifetime() { return _lifetime.asSeconds(); }

	float& friction() { return _friction; }
	float& size() { return _size; }

	float& rotationAngle() { return _rotationAngle; }
	float& angularVelocity() { return _angularVelocity; }


#pragma endregion Properties
};

#endif