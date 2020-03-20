#include "SocketManager.h"

bool SocketManager::BindToPort(sf::UdpSocket* _socket, int _port){
	return HandleSocketErrors(_socket->bind(_port));
}

 bool SocketManager::HandleSocketErrors(sf::Socket::Status _status) {

	 switch (_status) {
	 case sf::Socket::Status::Done: {
		 if(printMessages) Utils::print("Success!");
		 return true;
	 }
	 case sf::Socket::Status::Error: {
		 if (printMessages) Utils::print("Error!");
		 return false;
	 }
	 case sf::Socket::Status::Disconnected: {
		 if (printMessages) Utils::print("Disconnected!");
		 return false;
	 }
	 case sf::Socket::Status::NotReady: {
		 if (printMessages) Utils::print("NotReady!");
		 return false;
	 }
	 case sf::Socket::Status::Partial: {
		 if (printMessages) Utils::print("Partal!");
		 return false;
	 }
	 default:
		 break;
	 }

	 return false;
 }