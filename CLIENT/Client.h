#pragma once

#include "SDL_net.h"
#include "SDL.h"
#include <string>
#include <list>
#include "MessageStruct.h"

class Client
{
public:
	Client() = default;
	
	void Init(std::string name);
	
private:
	// Connection Window
	void InitRLWindow();
	void OpenConnectionWindow();
	bool ConnectTo(const char* address, int port);
	void HandleInputInConnectionField(bool& tryOpen);
	void DrawnConnectionWindow();

	void LaunchChat();
	//ChatWindow
	void SendMessage(std::string message);
	void ReceiveFromServer();
	void DrawChat();
	void DrawAllMessages();
	void DrawCurrentMessage();
	void HandleInput();

private:
	IPaddress m_serverAddress;
	int m_serverPort;
	TCPsocket m_socket;
	SDLNet_SocketSet m_serverSocket;
	bool m_isConnected = true;
	std::string m_name;
	std::list<MessageStruct> m_messageReceived;

	std::string m_currentText;
	std::string m_ipAddress = "localhost";
	std::string m_port ="4242";
	int m_width, m_height;
	bool m_inputOnAddress = true;
};

