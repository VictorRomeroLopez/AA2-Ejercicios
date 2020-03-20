#pragma once
#include <limits.h>
#include <SFML/Network.hpp>
namespace cns {

	const sf::IpAddress SERVER_IP = sf::IpAddress("127.0.0.1");
	const unsigned short SERVER_PORT = 50000;

	const unsigned short MAX_RANDOM_SALT = USHRT_MAX;

};