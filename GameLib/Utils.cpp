#include "Utils.h"

void Utils::print(std::string text)
{
	std::cout << text << std::endl;
}

unsigned short Utils::GenerateRandomSalt()
{
	srand(time(NULL));
	return rand() % cns::MAX_RANDOM_SALT;
}

unsigned short Utils::GenerateRandom(unsigned short _rand)
{
	srand(time(NULL));
	return rand() % _rand;
}
