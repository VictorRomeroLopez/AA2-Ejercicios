#pragma once
#include <SFML/Network.hpp>
#include "Utils.h"

class SocketManager
{
private:
	static const bool printMessages = true;

public:
	static bool BindToPort(sf::UdpSocket* _socket, int _port);
	static bool HandleSocketErrors(sf::Socket::Status _status);

};

