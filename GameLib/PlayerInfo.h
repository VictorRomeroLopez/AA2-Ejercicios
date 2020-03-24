#pragma once
#include <iostream>
#include <string>
#include <SFML/Network.hpp>
#include "Utils.h"

class PlayerInfo
{
protected:
	std::size_t playerId;
	std::string nick;
	sf::Vector2i position;

public:
	float timeDifference;
	bool connectedUser = true;

	PlayerInfo();
	PlayerInfo(std::size_t, std::string, sf::Vector2i);
	~PlayerInfo();

	std::size_t GetId();
	void SetId(std::size_t _playerId);
	std::string GetNick();
	void SetNick(std::string _nick);
	sf::Vector2i GetPosition();
	void SetPosition(sf::Vector2i _position);
	void ResetTimeoutTimer();

	friend sf::Packet& operator<<(sf::Packet& _packet, const PlayerInfo& _playerInfo);
	friend sf::Packet& operator>>(sf::Packet& _packet, PlayerInfo& _playerInfo);

};