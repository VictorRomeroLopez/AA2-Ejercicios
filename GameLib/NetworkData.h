#pragma onc
#include <SFML/Network.hpp>

class NetworkData
{
public:
	struct Hasher {
		std::size_t operator() (const NetworkData& nd) const
		{
			return (std::hash<std::string>()(nd.ip.toString())
				^ (std::hash<unsigned short>()(nd.port) << 1));
		}
	};

private:
	sf::IpAddress ip;
	unsigned short port;

public:

#pragma region CONSTRUCTORS

	NetworkData();
	NetworkData(sf::IpAddress _ip, unsigned short _port);
	~NetworkData();

#pragma endregion

#pragma region GETTERS & SETTERS

	sf::IpAddress GetIpAddress();
	unsigned short GetPort();

#pragma endregion

#pragma region OPERATORS

	bool operator== (const NetworkData& _networkData) const;

#pragma endregion
};

