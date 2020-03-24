#pragma once
#include "Utils.h"

class PlayerSalt
{
private:
	unsigned short clientSalt;
	unsigned short serverSalt;

public:
	PlayerSalt();
	PlayerSalt(unsigned short _clientSalt);
	PlayerSalt(unsigned short _clientSalt, unsigned short _serverSalt);

	unsigned short GetClientSalt();
	unsigned short GetServerSalt();
	void SetServerSalt(unsigned short _serverSalt);

	unsigned short GenerateRandomClientSalt();
	unsigned short GenerateRandomServerSalt();

	bool operator==(const PlayerSalt& _playerSalt);
	bool operator!=(const PlayerSalt& _playerSalt);
	friend sf::Packet& operator<<(sf::Packet& _packet, const PlayerSalt& _playerSalt);
	friend sf::Packet& operator>>(sf::Packet& _packet, PlayerSalt& _playerSalt);
};

