#pragma once
#include <SFML/Network.hpp>

class HelloPacket : public sf::Packet
{
public:
	static bool hasReceivedPacket;
	static bool hasSentPacket;

	virtual const void* onSend(std::size_t& size)
	{
		
		hasSentPacket = true;
		return getData();
	}
	
	virtual void onReceive(const void* data, std::size_t size)
	{
		hasReceivedPacket = true;
		append(data, size);
	}
};

