#include "PlayerSalt.h"
#include "PlayerSalt.h"

PlayerSalt::PlayerSalt() {}

PlayerSalt::PlayerSalt(unsigned short _clientSalt) : clientSalt(_clientSalt) {}

PlayerSalt::PlayerSalt(unsigned short _clientSalt, unsigned short _serverSalt) : clientSalt(_clientSalt), serverSalt(_serverSalt) {}

unsigned short PlayerSalt::GetClientSalt()
{
	return clientSalt;
}

unsigned short PlayerSalt::GetServerSalt()
{
	return serverSalt;
}

void PlayerSalt::SetServerSalt(unsigned short _serverSalt)
{
	serverSalt = _serverSalt;
}

unsigned short PlayerSalt::GenerateRandomClientSalt()
{
	clientSalt = Utils::GenerateRandomSalt();
	return clientSalt;
}

unsigned short PlayerSalt::GenerateRandomServerSalt()
{
	serverSalt = Utils::GenerateRandomSalt();
	return serverSalt;
}

bool PlayerSalt::operator==(const PlayerSalt& _playerSalt)
{
	return clientSalt == _playerSalt.clientSalt && serverSalt == _playerSalt.serverSalt;
}

bool PlayerSalt::operator!=(const PlayerSalt& _playerSalt)
{
	return clientSalt != _playerSalt.clientSalt || serverSalt != _playerSalt.serverSalt;
}

sf::Packet& operator<<(sf::Packet& _packet, const PlayerSalt& _playerSalt)
{
	return _packet << _playerSalt.clientSalt << _playerSalt.serverSalt;
}

sf::Packet& operator>>(sf::Packet& _packet, PlayerSalt& _playerSalt)
{
	unsigned short clientSalt;
	unsigned short serverSalt;
	_packet >> clientSalt >> serverSalt;
	_playerSalt = PlayerSalt(clientSalt, serverSalt);
	return _packet;
}
