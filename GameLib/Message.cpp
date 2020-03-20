#include "Message.h"

Message::Header Message::StringToHeader(std::string _header) {
	static const std::unordered_map<std::string, Header> stringedHeader = {
		{"HELLO", Header::HELLO},
		{"CHALLANGE", Header::CHALLANGE},
		{"WELCOME", Header::WELCOME}
	};

	auto it = stringedHeader.find(_header);
	return it != stringedHeader.end() ? it->second : Header::NONE;
}

std::string Message::HeaderToString(Header _header)
{
	static const std::unordered_map<Header, std::string> headers = {
		   {Header::HELLO, "HELLO"},
		   {Header::CHALLANGE, "CHALLANGE"},
		   {Header::WELCOME, "WELCOME"}
	};

	auto it = headers.find(_header);
	return it != headers.end() ? it->second : "NONE";
}

sf::Packet& operator<<(sf::Packet& _packet, const Message::Header& _header)
{
	return _packet << static_cast<unsigned short>(_header);
}

sf::Packet& operator>>(sf::Packet& _packet, Message::Header& _header)
{
	unsigned short catchedHeader;
	_packet >> catchedHeader;
	_header = static_cast<Message::Header>(catchedHeader);
	return _packet;
}
