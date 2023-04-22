// client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

//#include "pch.h"
#pragma comment(lib, "ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <winsock.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
//#include <SFML/Graphics.hpp>
//#include <SFML/Window.hpp>
//#include <SFML/System.hpp>
//using namespace sf;

SOCKET Connection;
char msg[256];
char nick[256];//256 - Максимальное количество знаков в нике
char msg1[256];
char newmsg[256];
int countmsg = 0;

void ClientHandler() {
	while (true) {
		
		recv(Connection, newmsg, sizeof(newmsg), NULL);

		printf("%s", newmsg);
	}
}

int main(int argc, char* argv[]) {

		//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		printf("Error\n");
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5510); //the same as in server
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	

	printf("Enter your nickname\n");
	scanf("%s", &nick);

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		printf("Error: Failed connection\n");
		system("pause");
		return 1;
	}
	printf("Connected!\n");

	send(Connection, nick, sizeof(nick), NULL);//Отправляет свой ник серверу
	while (recv(Connection, msg, sizeof(msg), NULL)) // получает запрос о выборе собеседников от сервера
	{
		printf("%s", msg);
	}
	gets_s(msg);
	send(Connection, msg, sizeof(msg), NULL); // отправляет список собеседников на сервер
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);
	/*RenderWindow window(VideoMode(800, 600), "chat", Style::Titlebar | Style::Close);
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);
	Event ev;
	Texture img;
	img.loadFromFile("background.jpg");
	Sprite background;
	background.setTexture(img);
	background.setScale(1.3, 1);

	Font font;
	font.loadFromFile("ua-BRAND-regular.otf");
	Text msg("", font, 20);
	msg.setString("Hello!");
	sf::String playerInput;
	sf::Text playerText("", font, 20);
	while (window.isOpen())
	{
		char key;
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			
		}
		if (event.type == Event::TextEntered)
		{
			printf("press");
			playerInput += event.text.unicode;
			playerText.setString(playerInput);
			Sleep(40);
			
		}
		window.clear();
		window.draw(background);
		window.draw(msg);
		window.draw(playerText);
		window.display();
	}*/

	while (true) {

		gets_s(msg1);		
		send(Connection, msg1, sizeof(msg1), NULL);
		Sleep(10);
	}

	system("pause");
	return 0;
}