#pragma once
#include <SDL_net.h>
#include <map>
#include <string>
#include "MessageStruct.h"

class Server
{
public:
	Server() = default;
	bool Init(int port);

	void LaunchServer();

private:
	void WaitForData();
	void ConnectNewClient(TCPsocket newSocket);
	void DisconnectClient();

	void ReceiveMessage();
	void SendMessage(MessageStruct message, TCPsocket clientSocket);

	void DispatchMessageToAllClients(std::string name, std::string message);
private:
	IPaddress m_address;
	TCPsocket m_serverSocket;
	bool m_runServer = true;
	SDLNet_SocketSet m_connectedClients;
	std::map<TCPsocket, std::string> m_clients;
};

