#pragma once
#include <iostream>
#include <string>
#include "Constants.h"
#include <time.h>
#include <random>

class Utils
{
public:
	static void print(std::string text); 
	static unsigned short GenerateRandomSalt();
	static unsigned short GenerateRandom(unsigned short);
	static sf::Vector2i GenerateRandomMapPosition();
	static float GetRandomFloat();

	friend sf::Packet& operator<<(sf::Packet& _packet, const sf::Vector2i& _vector);
	friend sf::Packet& operator>>(sf::Packet& _packet, sf::Vector2i& _vector);
};