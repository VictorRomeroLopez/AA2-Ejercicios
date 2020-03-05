#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>

const sf::IpAddress SERVER_IP = sf::IpAddress("127.0.0.1");
const unsigned short SERVER_PORT = 50000;

int main()
{
	PlayerInfo playerInfo;

	sf::UdpSocket socket;
	sf::Packet packet;
	packet << 30;
	socket.send(packet, SERVER_IP, SERVER_PORT);

	while (true) {

	}

	return 0;
}