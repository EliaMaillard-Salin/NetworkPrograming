#include "DuckRun.h"
#include "Raylib.h"
#include <iostream>
#include <SDL_video.h>

void DuckRun::Init(std::string name)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
        return;
    }
    if (SDLNet_Init() == -1)
    {
        std::cerr << "SDLNet Initialization error: " << SDLNet_GetError() << std::endl;
        return;
    }
    m_name = name;

    InitRLWindow();
    OpenConnectionWindow();
}

void DuckRun::InitRLWindow()
{
    m_width = 1800; m_height = 500;
    InitWindow(m_width, m_height, "My first chat window!");
    SetTargetFPS(60);
    // Duck Anim
    Image duck1 = LoadImage("Assets/Ducks/Duck1.png");
    Image duck2 = LoadImage("Assets/Ducks/Duck2.png");
    Texture2D tDuck1 = LoadTextureFromImage(duck1);
    m_duckSprite[0] = tDuck1;
    Texture2D tDuck2 = LoadTextureFromImage(duck2);
    m_duckSprite[1] = tDuck2;

    // Duck Skin
    Image hat = LoadImage("Assets/Ducks/partyHat.png");
    Image glasses = LoadImage("Assets/Ducks/glasses.png");
    Texture2D tHat = LoadTextureFromImage(hat);
    m_skin.push_back(tHat);
    Texture2D tGlasses = LoadTextureFromImage(glasses);
    m_skin.push_back(tGlasses);
}

bool DuckRun::ConnectTo(const char* address, int port)
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
        m_selfData.duckName = m_name;
        MessageStruct token;
        token.type = MessageStruct::LOG;
        token.from = m_name;
        token.content = m_selfData.ConvertToString();
        SendTokenToServer(token.CreateBufferFromMessage());
    }

    return true;
}

void DuckRun::HandleInputInConnectionField(bool& tryOpen)
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

void DuckRun::DrawnConnectionWindow()
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

void DuckRun::OpenConnectionWindow()
{
    bool tryToOpen = true;
    while (tryToOpen && !WindowShouldClose())
    {
        HandleInputInConnectionField(tryToOpen);
        DrawnConnectionWindow();
    }
    if (m_isConnected)
        JoinGame();
    else
    {
        CloseWindow();
        SDLNet_Quit();
    }
}

void DuckRun::SendTokenToServer(std::string message)
{
    int bytesSent = SDLNet_TCP_Send(m_socket, message.c_str(), message.length() + 1);
    if (bytesSent < message.length() + 1) {
        std::cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << std::endl;
        return;
    }
    std::cout << "Sent " << bytesSent << " bytes to the server !" << std::endl;
}

void DuckRun::ParseMessageFromServer(MessageStruct message)
{
    if (message.type == MessageStruct::FRAME_UPDATE)
    {

        std::string ducksUpdate = message.content;
        while (!ducksUpdate.empty())
        {
            DuckData newData = DuckData::GetFromString(ducksUpdate);
            m_ducksRunning[newData.duckID] = newData;
        }
    }
    if (message.type == MessageStruct::LOG)
    {
        if (message.from.find("NewDucks") == 0)
        {
            DuckData duck = DuckData::GetFromString(message.content);
            m_ducksRunning[duck.duckID] = duck;
        }
    }
    if (message.type == MessageStruct::GAME_INFO)
    {
        if (message.from.find("Start") == 0)
        {
            m_runStarted = true;
        }
    }
}

void DuckRun::JoinGame()
{
    while (m_isConnected)
    {
        ReceiveFromServer();
        HandleInput();
        DrawGame();
    }
    CloseWindow();
    SDLNet_Quit();
    //Disconnect();
}

void DuckRun::ReceiveFromServer()
{
    if (SDLNet_CheckSockets(m_serverSocket, 0) == 0)
        return;
    char messageBuffer[1024];
    int bytesRead = SDLNet_TCP_Recv(m_socket, messageBuffer, sizeof(messageBuffer));
    if (bytesRead > 0)
    {
        MessageStruct received = MessageStruct::GetFromBuffer(messageBuffer);
        if (received.type == MessageStruct::LOG && received.from.find("ID") == 0)
        {
            m_selfData.duckID = std::stoi(received.content);
            m_ID = m_selfData.duckID;
            std::cout << "ID Received\n";
            m_ducksRunning[m_ID] = m_selfData;
        }
        else
            ParseMessageFromServer(received);
    }
}

void DuckRun::DrawGame()
{
    BeginDrawing();
    ClearBackground(SKYBLUE);
    DrawRectangle(0, m_height - 300, m_width, 300, DARKGREEN);
    DrawRectangle(10, m_height - 25, m_width-20, 2, WHITE);
    DrawRectangle(10, m_height - 100, m_width-20, 2, WHITE);
    DrawRectangle(10, m_height - 175, m_width-20, 2, WHITE);
    DrawRectangle(10, m_height - 250, m_width-20, 2, WHITE);

    DrawRectangle(10, m_height - 290, 20, 300, BROWN);
    DrawRectangle(100, m_height - 450, 20, 200, BROWN);
    DrawRectangle(m_width - 30, m_height - 290, 20, 300, BROWN);
    DrawRectangle(m_width - 100, m_height - 450, 20, 200, BROWN);

    DrawDucks();
    EndDrawing();

}

void DuckRun::DrawDucks ()
{
    int offset = m_height - 250 - 125;
    for (DuckData data : m_ducksRunning)
    {
        offset += 75;
        if (data.duckID == -1)
            continue;

        DrawTexture(m_duckSprite[data.duckSpriteState], data.DuckDist, offset, WHITE);
        if (data.duckSpriteID > 0)
        {
            DrawTexture(m_skin[data.duckSpriteID - 1], data.DuckDist, offset, WHITE);
        }
        DrawText(data.duckName.c_str(), data.DuckDist - data.duckName.length() * 3, offset - 15, 20, BLACK);
    }
}


void DuckRun::HandleInput()
{
    if (IsKeyPressed(KEY_SPACE) && m_runStarted)
    {
        m_selfData.DuckDist += 10;
        m_selfData.duckSpriteState = !m_selfData.duckSpriteState;
        MessageStruct duckDatas;
        duckDatas.type = MessageStruct::FRAME_UPDATE;
        duckDatas.from = m_name;
        duckDatas.content = m_selfData.ConvertToString();
        SendTokenToServer(duckDatas.CreateBufferFromMessage());
    }
    if (IsKeyPressed(KEY_ENTER))
    {
        MessageStruct duckDatas;
        duckDatas.type = MessageStruct::GAME_INFO;
        duckDatas.from = "Start";
        SendTokenToServer(duckDatas.CreateBufferFromMessage());
    }

    if (WindowShouldClose())
        m_isConnected = false;
}

std::string DuckData::ConvertToString()
{
    std::string out;
    out += std::to_string(duckID);
    out += "#";
    out += std::to_string(DuckDist);
    out += "#";
    out += std::to_string(duckSpriteID);
    out += "#";
    out += std::to_string(duckSpriteState);
    out += "#";
    out += duckName;
    out += "#";
    return out;
}

DuckData DuckData::GetFromString(std::string& in)
{
    DuckData out;
    std::string duckIDStr = in.substr(0, in.find("#"));
    out.duckID = std::stoi(duckIDStr);
    in = in.erase(0, in.find("#") + 1);
    std::string duckDistStr = in.substr(0, in.find("#"));
    out.DuckDist = std::stoi(duckDistStr);
    in = in.erase(0, in.find("#") + 1);
    std::string duckSpriteIDStr = in.substr(0, in.find("#"));
    out.duckSpriteID = std::stoi(duckSpriteIDStr);
    in = in.erase(0, in.find("#") + 1);
    std::string duckSpriteStateStr = in.substr(0, in.find("#"));
    out.duckSpriteState = std::stoi(duckSpriteStateStr);
    in = in.erase(0, in.find("#") + 1);
    out.duckName = in.substr(0, in.find("#"));
    in = in.erase(0, in.find("#") + 1);

    return out;
}
