#pragma once
#include <SFML/Network.hpp>
#include "PlayerToValidateInfo.h"

class CriticalPacket
{
private:
	sf::Packet criticalPacket;
	NetworkData networkData;

public:
	CriticalPacket();
	CriticalPacket(sf::Packet, NetworkData);
	~CriticalPacket();

	sf::Packet GetPacket();
	NetworkData GetNetworkData();

};

