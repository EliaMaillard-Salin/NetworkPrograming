#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include "Client.h"
//#include <string>
using namespace std;

int main(int argc, char* argv[])
{

	Client client = Client();
	client.Init("Eliaaaaaa");
	cout << "Thank you for using ChArtFX !\n";
	return 0;
}
