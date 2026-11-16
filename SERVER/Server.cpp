#include "Server.h"
#include <iostream>


bool Server::Init(int port)
{
	if (SDLNet_Init() == -1)
	{
		std::cerr << "SDLNet Initialization error: " << SDLNet_GetError() << std::endl;
		return false;
	}
	if (SDLNet_ResolveHost(&m_address, nullptr, port) == -1)
	{
		std::cerr << "Resolve Host error: " << SDLNet_GetError() << std::endl;
		SDLNet_Quit();
		return false;
	}
	m_serverSocket = SDLNet_TCP_Open(&m_address);
	if (!m_serverSocket)
	{
		std::cerr << "TCP Open error: " << SDLNet_GetError() << std::endl;
		SDLNet_Quit();
		return false;
	}
	m_connectedClients = SDLNet_AllocSocketSet(8);	//Put the maximum number of sockets
	return true;
}

void Server::LaunchServer()
{
	std::cout << "Server Started !\n";
	while(m_runServer)
	{
		WaitForData();
	}
}

void Server::WaitForData()
{
	// Test new Connection
	TCPsocket clientSocket = SDLNet_TCP_Accept(m_serverSocket);
	if (clientSocket)
	{
		ConnectNewClient(clientSocket);
	}

	// Check MessageReceived
	if (SDLNet_CheckSockets(m_connectedClients, 0) == 0)
		return;

	for (auto it : m_clients)
	{
		if (it.first)
		{
			char messageBuffer[1024];
			int bytesRead = SDLNet_TCP_Recv(it.first, messageBuffer, sizeof(messageBuffer));
			if (bytesRead > 0)
			{
				std::string messageReceived = messageBuffer;
				std::cout << "Incoming message: " << messageReceived << std::endl;
				DispatchMessageToAllClients(it.second, messageReceived);
			}
		}
	}

}
void Server::DispatchMessageToAllClients(std::string name, std::string message)
{
	for (auto it : m_clients)
	{
		SendMessage(name + ": " + message, it.first);
	}
}

void Server::ConnectNewClient(TCPsocket clientSocket)
{
	std::cout << "A client reached the server!" << std::endl;
	char buffer[1024];
	int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
	if (bytesRead > 0)
	{
		std::cout << "Client Name: " << buffer << std::endl;
		m_clients.insert({ clientSocket,buffer });
		//SendMessage("Connection Worked", clientSocket);
		SDLNet_AddSocket(m_connectedClients, reinterpret_cast<SDLNet_GenericSocket>(clientSocket));
	}
	else
	{
		std::cout << "Client Name not received client Not Connected" << std::endl;
		SendMessage("Failed Connection", clientSocket);
	}
}

void Server::DisconnectClient()
{
}

void Server::ReceiveMessage()
{
}

void Server::SendMessage(std::string message, TCPsocket clientSocket)
{
	int bytesSent = SDLNet_TCP_Send(clientSocket, message.c_str(), message.length() + 1);
	if (bytesSent < message.length() + 1) {
		std::cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << std::endl;
		return;
	}
	std::cout << "Sent " << bytesSent << " bytes to the Client !" << std::endl;
}


