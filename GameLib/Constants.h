#pragma once
#include <limits.h>
#include <SFML/Network.hpp>

namespace cns {

	const sf::IpAddress SERVER_IP = sf::IpAddress("127.0.0.1");
	//const sf::IpAddress SERVER_IP = sf::IpAddress("192.168.1.61");
	const unsigned short SERVER_PORT = 50000;

	const unsigned short GAME_WIDTH = 1360;
	const unsigned short GAME_HEIGHT = 768;

	const unsigned short MAX_RANDOM_SALT = USHRT_MAX;

};