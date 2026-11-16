#include "Client.h"
#include "Raylib.h"
#include <iostream>
#include <SDL_video.h>

void Client::Init(std::string name)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
        return;
    }
    if (SDLNet_Init() == -1)
    {
        std::cerr << "SDLNet Initialization error: " << SDLNet_GetError() << std::endl;
        return ;
    }
    m_name = name;

    InitRLWindow();
    OpenConnectionWindow();
}

void Client::InitRLWindow()
{
    m_width = 500; m_height = 750;
    InitWindow(m_width, m_height, "My first chat window!");
    SetTargetFPS(60);
}

bool Client::ConnectTo(const char* address,int port)
{
    m_serverSocket = SDLNet_AllocSocketSet(1);
    if (SDLNet_ResolveHost(&m_serverAddress, address, port) == -1)
    {
        std::cerr << "Resolve Host error: " << SDLNet_GetError() << std::endl;
        return false;
    }
    m_socket = SDLNet_TCP_Open(&m_serverAddress);
    if (!m_socket) {
        std::cerr << "TCP Open error: " << SDLNet_GetError() << std::endl;
        return false;
    }
    else
    {
        SDLNet_AddSocket(m_serverSocket, reinterpret_cast<SDLNet_GenericSocket>(m_socket));
        SendMessage(m_name);
    }

    return true;
}

void Client::HandleInputInConnectionField(bool& tryOpen)
{
    Vector2 mousePoint = GetMousePosition();
    Rectangle rect = { 150, 400, 175, 50 }; // Connect Button
    if (CheckCollisionPointRec(mousePoint, rect))
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            if (ConnectTo(m_ipAddress.c_str(), std::stoi(m_port)))
            {
                m_isConnected = true;
                tryOpen = false;
            }
    }
    Rectangle rectIP = { 15, 150, m_width - 40, 50 }; // IPAddress
    if (CheckCollisionPointRec(mousePoint, rectIP))
        m_inputOnAddress = true;
    Rectangle rectPort = { 15, 300, m_width - 40, 50 }; // Port
    if (CheckCollisionPointRec(mousePoint, rectPort))
        m_inputOnAddress = false;

    int keyPressed = GetKeyPressed();
    bool upperCase = (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT));
    if ((keyPressed >= 65 && keyPressed <= 90))
    {
        if (upperCase)
        {
            if (m_inputOnAddress)
                m_ipAddress += keyPressed;
            else
                m_port += keyPressed;
        }
        else
        {
            if (m_inputOnAddress)
                m_ipAddress += (keyPressed + 32);
            else
                m_port += (keyPressed + 32);
        }
    }
    else if ((keyPressed >= 32 && keyPressed <= 126))
        if (m_inputOnAddress)
            m_ipAddress += keyPressed;
        else
            m_port += keyPressed;
    if (IsKeyPressed(KEY_BACKSPACE))
        if (m_inputOnAddress)
            m_ipAddress.pop_back();
        else
            m_port.pop_back();
    if (WindowShouldClose())
        tryOpen = false;

}

void Client::DrawnConnectionWindow()
{
    BeginDrawing();
    ClearBackground(GRAY);
    DrawText("Welcome to ChArtFX!", 20, 15, 45, WHITE);
    DrawText("IP Address :", 10, 100, 30, WHITE);
    DrawRectangle(15, 150, m_width - 40, 50, LIGHTGRAY);
    DrawText(m_ipAddress.c_str(), 20, 160, 30, BLACK);
    DrawText("Port :", 10, 250, 30, WHITE);
    DrawRectangle(15, 300, m_width - 40, 50, LIGHTGRAY);
    DrawText(m_port.c_str(), 20, 310, 30, BLACK);

    if (m_inputOnAddress)
        DrawRectangle(17, 160, 2, 30, BLACK); //UIBArr
    else
        DrawRectangle(17, 310, 2, 30, BLACK); //UIBArr

    DrawRectangle(150, 400, 175, 50, DARKGRAY);
    DrawText("CONNECT", 160, 410, 30, WHITE);
    EndDrawing();
}

void Client::OpenConnectionWindow()
{
    bool tryToOpen = true;
    while (tryToOpen)
    {
        HandleInputInConnectionField(tryToOpen);
        DrawnConnectionWindow();
    }
    if (m_isConnected)
        LaunchChat();
    else
        SDLNet_Quit();
}

void Client::SendMessage(std::string message)
{
    int bytesSent = SDLNet_TCP_Send(m_socket, message.c_str(), message.length() + 1);
    if (bytesSent < message.length() + 1) {
        std::cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << std::endl;
        return;
    }
    std::cout << "Sent " << bytesSent << " bytes to the server !" << std::endl;
}

void Client::LaunchChat()
{
    InitRLWindow();
    while(m_isConnected)
    {
        ReceiveFromServer();
        HandleInput();
        DrawChat();
    }
    CloseWindow();
    SDLNet_Quit();
    //Disconnect();
}

void Client::ReceiveFromServer()
{
    if (SDLNet_CheckSockets(m_serverSocket, 0) == 0)
        return;

    char messageBuffer[1024];
    int bytesRead = SDLNet_TCP_Recv(m_socket, messageBuffer, sizeof(messageBuffer));
    if (bytesRead > 0)
    {
        MessageStruct::Type type;
        std::memcpy(&type, messageBuffer, sizeof(type));
        //std::string strMessage = messageBuffer;
        //std::string pseudo = strMessage.substr(0, strMessage.find(":"));
        //std::string content = strMessage.erase(0, strMessage.find(":") + 1);
        //MessageStruct received = { MessageStruct::Type::MESSAGE, pseudo,content };
        //std::string messageReceived = messageBuffer;
        //std::cout << "Incoming message: " << messageReceived << std::endl;
        //m_messageReceived.push_front(received);
        SendMessage("Received");
    }
}

void Client::DrawChat()
{
    BeginDrawing();
    ClearBackground(GRAY);
    DrawText("Welcome to ChArtFX!", 220, 15, 25, WHITE);
    DrawRectangle(20, 50, m_width - 40, m_height - 150, DARKGRAY);
    DrawRectangle(20, m_height - 90, m_width - 40, 50, LIGHTGRAY);
    DrawAllMessages();
    DrawCurrentMessage();
    EndDrawing();

}

void Client::DrawAllMessages()
{
    int offset = m_height - 120;
    for (MessageStruct message : m_messageReceived)
    {
        std::string c = message.from + " -> " + message.content;
        DrawText(c.c_str(), 25, offset, 20, WHITE);
        offset -= 20;
    }
}

void Client::DrawCurrentMessage()
{
    DrawText(m_currentText.c_str(), 20, m_height - 90, 25, BLACK);
}

void Client::HandleInput()
{
    int keyPressed = GetKeyPressed();
    bool upperCase = (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT));
    if ((keyPressed >= 65 && keyPressed <= 90))
    {
        if (upperCase)
            m_currentText += keyPressed;
        else
            m_currentText += (keyPressed + 32);
    }
    else if ((keyPressed >= 32 && keyPressed <= 126))
        m_currentText += keyPressed;
    if (IsKeyPressed(KEY_BACKSPACE))
        m_currentText.pop_back();
    if (IsKeyPressed(KEY_ENTER))
    {
        SendMessage(m_currentText);
        m_currentText = "";
    }

    if (WindowShouldClose())
        m_isConnected = false;
}
