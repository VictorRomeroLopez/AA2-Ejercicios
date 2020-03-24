#pragma once
#include <iostream>
#include <unordered_map>
#include <SFML/Network.hpp>

class Message
{
public:
	enum class Header {
		HELLO = 0,
		CHALLANGE = 1,
		WELCOME = 2,
		EXIT = 3,
		MESSAGE = 4,
		PLEAVE = 5,
		PJOIN = 6,
		ACK = 7,
		NONE = -1
	};

	static Header StringToHeader(std::string _header);
	static std::string HeaderToString(Header _header);

	friend sf::Packet& operator<< (sf::Packet& _packet, const Header& _header);
	friend sf::Packet& operator>> (sf::Packet& _packet, Header& _header);
};

