#include "CriticalPacket.h"

CriticalPacket::CriticalPacket() {}

CriticalPacket::CriticalPacket(sf::Packet _criticalPacket, NetworkData _networkData): criticalPacket(_criticalPacket), networkData(_networkData) {}

CriticalPacket::~CriticalPacket()
{
}

sf::Packet CriticalPacket::GetPacket()
{
	return criticalPacket;
}

NetworkData CriticalPacket::GetNetworkData()
{
	return networkData;
}
