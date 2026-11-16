#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include "Server.h"
using namespace std;

int main(int argc, char* argv[])
{
	Server serv = Server();
	if (serv.Init(4242) == false)
	{
		return 0;
	}

	serv.LaunchServer();

	cout << "Thank you for using ChArtFX !\n";
	return 0;
}
