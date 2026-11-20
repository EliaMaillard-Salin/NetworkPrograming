#pragma once
#include <SDL_net.h>
#include <map>
#include <array>
#include <string>
#include "MessageStruct.h"

class Server
{
public:
	Server() = default;
	bool Init(int port);

	void LaunchServer();

private:
	void ResetRun();
	void WaitForData();
	void ConnectNewClient(TCPsocket newSocket);
	void DisconnectClient();
	void HandleGameInfos(MessageStruct message);

	void ReceiveMessage(TCPsocket socket, std::string fromName );
	void SendMessage(MessageStruct message, TCPsocket clientSocket);

	void UpdateDataFrame();
	void UpdateReceivedData(DuckData duck);
	void DispatchMessageToAllClients(MessageStruct message);
private:
	float m_sendDataCD = 60.0f;
	float m_currentSendDataCD;
	int m_lastIDAvailable = 0;
	bool m_runGoing;

	IPaddress m_address;
	TCPsocket m_serverSocket;
	bool m_runServer = true;
	SDLNet_SocketSet m_connectedClients;
	std::map<TCPsocket, std::string> m_clients;
	std::array<DuckData, 4> m_ducksRunning;
};

