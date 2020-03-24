#include "Utils.h"

void Utils::print(std::string text)
{
	std::cout << text << std::endl;
}

unsigned short Utils::GenerateRandomSalt()
{
	srand(time(NULL));
	return rand() % cns::MAX_RANDOM_SALT;
}

unsigned short Utils::GenerateRandom(unsigned short _rand)
{
	srand(time(NULL));
	return rand() % _rand;
}

sf::Vector2i Utils::GenerateRandomMapPosition()
{
	return sf::Vector2i{ rand() & cns::GAME_WIDTH, rand() % cns::GAME_HEIGHT };
}

sf::Packet& operator<<(sf::Packet& _packet, const sf::Vector2i& _vector)
{
	return _packet << _vector.x << _vector.y;
}

sf::Packet& operator>>(sf::Packet& _packet, sf::Vector2i& _vector)
{
	return _packet >> _vector.x >> _vector.y;
}
