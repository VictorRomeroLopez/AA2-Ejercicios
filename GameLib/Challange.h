#pragma once
#include <iostream>
#include <SFML/Network.hpp>

class Challange
{
private:
	unsigned short firstValue;
	unsigned short secondValue;

public:
	Challange();
	Challange(unsigned short _fistValue, unsigned short _secondValue);
	~Challange();

	unsigned short GetFirstValue();
	unsigned short GetSecondValue();
	unsigned short GetResult();

	friend sf::Packet& operator<< (sf::Packet& _packet, const Challange& _challange) {
		return _packet << _challange.firstValue << _challange.secondValue;
	}

	friend sf::Packet& operator>> (sf::Packet& _packet, Challange& _challange) {
		return _packet >> _challange.firstValue >> _challange.secondValue;
	}

};

