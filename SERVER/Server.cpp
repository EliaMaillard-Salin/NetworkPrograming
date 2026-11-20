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
	m_connectedClients = SDLNet_AllocSocketSet(4);	//Put the maximum number of sockets
	return true;
}

void Server::LaunchServer()
{
	std::cout << "Server Started !\n";
	while(m_runServer)
	{
		m_currentSendDataCD += 1.0f;
		WaitForData();
	}
}

void Server::ResetRun()
{
	for (DuckData ducks : m_ducksRunning)
	{
		ducks.DuckDist = 0;
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
	if (SDLNet_CheckSockets(m_connectedClients, 0) <= 0)
		return;

	for (auto it : m_clients)
	{
		if (it.first)
		{
			if (SDLNet_SocketReady(it.first))
				ReceiveMessage(it.first, it.second);
		}
	}

}
void Server::UpdateDataFrame()
{
	MessageStruct updateData;
	updateData.type = MessageStruct::FRAME_UPDATE;
	updateData.title = "Server";
	for (DuckData duckCOnnected : m_ducksRunning)
	{
		if (duckCOnnected.duckID == -1)
			continue;
		updateData.content += duckCOnnected.ConvertToString();
	}

	for (auto it : m_clients)
	{
		SendMessage(updateData,it.first);
	}
}

void Server::UpdateReceivedData(DuckData duck)
{
	m_ducksRunning[duck.duckID] = duck;
	UpdateDataFrame();
}

void Server::DispatchMessageToAllClients(MessageStruct message)
{
	for (auto it : m_clients)
	{
		SendMessage(message, it.first);
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
		std::cout << "Client Name: " << received.title << std::endl;
		m_clients.insert({ clientSocket,received.title });
		MessageStruct sendID;
		sendID.type = MessageStruct::Type::LOG;
		sendID.title = "ID";
		sendID.content = std::to_string(m_lastIDAvailable);
		SendMessage(sendID, clientSocket);

		DuckData newDuck = DuckData::GetFromString(received.content);
		newDuck.duckID = m_lastIDAvailable;

		MessageStruct sendNewDuck;
		sendNewDuck.type = MessageStruct::FRAME_UPDATE;
		sendNewDuck.title = "NewDucks";
		sendNewDuck.content = newDuck.ConvertToString();
		m_ducksRunning[m_lastIDAvailable] = newDuck;
		m_lastIDAvailable++;
		SDLNet_AddSocket(m_connectedClients, reinterpret_cast<SDLNet_GenericSocket>(clientSocket));

		UpdateDataFrame();

	}
	else
	{
		std::cout << "Client Name not received client Not Connected" << std::endl;
		MessageStruct toSend;
		toSend.type = MessageStruct::Type::LOG;
		toSend.title = "FAIL";
		toSend.content = "Connection Failed";
		SendMessage(toSend, clientSocket);
	}
}

void Server::DisconnectClient()
{
}

void Server::HandleGameInfos(MessageStruct message)
{
	if (message.title.find("Start") == 0)
	{
		m_runGoing = true;
		MessageStruct startRun;
		startRun.type = MessageStruct::GAME_INFO;
		startRun.title = "Start";
		DispatchMessageToAllClients(startRun);
	}
	if (message.title.find("Win") == 0)
	{
	}
}

void Server::ReceiveMessage(TCPsocket socket, std::string fromName)
{
	char messageBuffer[1024];
	int bytesRead = SDLNet_TCP_Recv(socket, messageBuffer, sizeof(messageBuffer));
	if (bytesRead > 0)
	{
		MessageStruct receiveMessage = MessageStruct::GetFromBuffer(messageBuffer);
		if (receiveMessage.type == MessageStruct::FRAME_UPDATE)
			UpdateReceivedData(DuckData::GetFromString(receiveMessage.content));
		if (receiveMessage.type == MessageStruct::GAME_INFO)
			HandleGameInfos(receiveMessage);
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


