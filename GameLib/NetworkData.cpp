#include "NetworkData.h"

NetworkData::NetworkData() : ip(sf::IpAddress()), port(0) {}

NetworkData::NetworkData(sf::IpAddress _ip, unsigned short _port): ip(_ip), port(_port) {}

NetworkData::~NetworkData() {}

sf::IpAddress NetworkData::GetIpAddress()
{
	return ip;
}

unsigned short NetworkData::GetPort()
{
	return port;
}

bool NetworkData::operator== (const NetworkData& _networkData) const
{
	return (ip == _networkData.ip && 
		port == _networkData.port);
}