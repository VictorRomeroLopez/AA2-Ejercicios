#pragma once
#include <limits.h>
#include <SFML/Network.hpp>

namespace cns {

	const sf::IpAddress SERVER_IP = sf::IpAddress("127.0.0.1");
	const unsigned short SERVER_PORT = 50000;

	const unsigned short GAME_WIDTH = 1360;
	const unsigned short GAME_HEIGHT = 768;
	const unsigned short GAME_CELL_SIZE = 16;

	const unsigned short MAX_RANDOM_SALT = USHRT_MAX;
	const float PERCENT_PACKETLOSS = 0.01f;
	const float TIME_CLIENTS_TIMEOUT = 50.f;

};