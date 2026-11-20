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
			ReceiveMessage(it.first, it.second);
		}
	}

}
void Server::DispatchMessageToAllClients(std::string name, std::string message)
{
	for (auto it : m_clients)
	{
		MessageStruct toSend;
		toSend.type = MessageStruct::Type::MESSAGE;
		toSend.from = name;
		toSend.content = message;
		SendMessage(toSend, it.first);
	}
}

void Server::ConnectNewClient(TCPsocket clientSocket)
{
	std::cout << "A client reached the server!" << std::endl;
	char buffer[1024];
	int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
	if (bytesRead > 0)
	{
		MessageStruct received = MessageStruct::GetFromBuffer(buffer);
		std::cout << "Client Name: " << received.from << std::endl;
		m_clients.insert({ clientSocket,received.from });
		MessageStruct toSend;
		toSend.type = MessageStruct::Type::LOG;
		toSend.from = "Server :";
		toSend.content = "Connection Worked";
		SendMessage(toSend, clientSocket);
		SDLNet_AddSocket(m_connectedClients, reinterpret_cast<SDLNet_GenericSocket>(clientSocket));
	}
	else
	{
		std::cout << "Client Name not received client Not Connected" << std::endl;
		MessageStruct toSend;
		toSend.type = MessageStruct::Type::LOG;
		toSend.from = "Server :";
		toSend.content = "Connection Failed";
		SendMessage(toSend, clientSocket);
	}
}

void Server::DisconnectClient()
{
}

void Server::ReceiveMessage(TCPsocket socket, std::string fromName)
{
	char messageBuffer[1024];
	int bytesRead = SDLNet_TCP_Recv(socket, messageBuffer, sizeof(messageBuffer));
	if (bytesRead > 0)
	{
		MessageStruct receiveMessage = MessageStruct::GetFromBuffer(messageBuffer);

		MessageStruct toSend;
		toSend.type = MessageStruct::Type::LOG;
		toSend.from = "Server :";
		toSend.content = "Message Received";
		//SendMessage(toSend, socket);
		DispatchMessageToAllClients(receiveMessage.from, receiveMessage.content);
	}
}

void Server::SendMessage(MessageStruct message, TCPsocket clientSocket)
{
	std::string toSend = message.CreateBufferFromMessage();
	int bytesSent = SDLNet_TCP_Send(clientSocket, toSend.c_str(), toSend.length() + 1);

	if (bytesSent < toSend.length() + 1) {
		std::cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << std::endl;
		return;
	}
	std::cout << "Sent " << bytesSent << " bytes to the Client !" << std::endl;
}


