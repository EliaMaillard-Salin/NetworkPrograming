#pragma once

#include "SDL_net.h"
#include "SDL.h"
#include "raylib.h"
#include <string>
#include <array>
#include <vector>
#include "MessageStruct.h"

struct DuckData
{
	int duckID = -1;
	int DuckDist;
	int duckSpriteID = 1;
	bool duckSpriteState;
	std::string duckName;

	std::string ConvertToString();
	static DuckData GetFromString(std::string& in);
};
class DuckRun
{
public:
	DuckRun() = default;
	void Init(std::string runnerName);

private:
	// Connection Window
	void InitRLWindow();
	void OpenConnectionWindow();
	bool ConnectTo(const char* address, int port);
	void HandleInputInConnectionField(bool& tryOpen);
	void DrawnConnectionWindow();

	//ChatWindow
	void JoinGame();
	void DrawGame();
	void DrawDucks();

	void HandleInput();
	void ReceiveFromServer();
	void SendTokenToServer(std::string message);

	void ParseMessageFromServer(MessageStruct message);

private:
	int m_ID;
	IPaddress m_serverAddress;
	int m_serverPort;
	TCPsocket m_socket;
	SDLNet_SocketSet m_serverSocket;
	bool m_isConnected = false;
	std::string m_name;
	std::array<DuckData,4> m_ducksRunning;
	std::vector<Texture2D> m_skin;
	std::array<Texture2D,2> m_duckSprite;
	DuckData m_selfData;
	// ConnexionWindow
	std::string m_ipAddress = "localhost";
	std::string m_port = "4242";
	bool m_runStarted = false;


	int m_width, m_height;
	bool m_inputOnAddress = true;
};

