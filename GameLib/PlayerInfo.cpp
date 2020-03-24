#include "PlayerInfo.h"

PlayerInfo::PlayerInfo() :
	playerId(0), nick(""), position({0,0}) {}

PlayerInfo::PlayerInfo(std::size_t _playerId, std::string _nick, sf::Vector2i _position) :
	playerId(_playerId), nick(_nick), position(_position) {}

PlayerInfo::~PlayerInfo() {}

std::size_t PlayerInfo::GetId()
{
	return playerId;
}

void PlayerInfo::SetId(std::size_t _playerId) {
	playerId = _playerId;
}

std::string PlayerInfo::GetNick()
{
	return nick;
}

void PlayerInfo::SetNick(std::string _nick)
{
	nick = _nick;
}

sf::Vector2i PlayerInfo::GetPosition()
{
	return position;
}

void PlayerInfo::SetPosition(sf::Vector2i _position) 
{
	position = _position;
}

void PlayerInfo::ResetTimeoutTimer()
{
	timeDifference = clock();
}

sf::Packet& operator<<(sf::Packet& _packet, const PlayerInfo& _playerInfo)
{
	return _packet << _playerInfo.playerId << _playerInfo.nick << _playerInfo.position;
}

sf::Packet& operator>>(sf::Packet& _packet, PlayerInfo& _playerInfo)
{
	std::size_t _playerId;
	std::string _nick;
	sf::Vector2i _position;

	_packet >> _playerId >> _nick >> _position;
	_playerInfo = PlayerInfo(_playerId, _nick, _position);
	return _packet;
}
