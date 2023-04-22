// client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

//#include "pch.h"
#pragma comment(lib, "ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS
#include <winsock.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

void SendData2Server(const char *mes, const char *name)
{
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (client == INVALID_SOCKET)
	{
		printf("Error create socket\n");
		return;
	}
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(5510); //the same as in server
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //special look-up address
	if (connect(client, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Can't connect to server\n");
		closesocket(client);
		return;
	}
	char message[1024]="";
	sprintf(message, "%s: %s", name, mes);
	
	int ret = send(client, message, strlen(message), 0);
	if (ret == SOCKET_ERROR)
	{
		printf("Can't send message\n");
		closesocket(client);
		return;
	}
	printf("Client: %s\n", mes);
	ret = SOCKET_ERROR;
	int i = 0;
	while (ret == SOCKET_ERROR)
	{
		//полчение ответа
		ret = recv(client, message, sizeof(message), 0);
		//обработка ошибок
		if (ret == 0 || ret == WSAECONNRESET)
		{
			printf("Connection closed\n");
			break;
		}
		if (ret < 0)
		{
			//printf("Can't resieve message\n");
			/*closesocket(client);
			return;*/
			continue;
		}
		//вывод на экран количества полученных байт и сообщение
		printf("Server: %s\n", message);
	}
	closesocket(client);
}

int main()
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
	{
		printf("Can't connect to socket lib");
		return 1;
	}
	printf("Hello, input your name!\n");
	char name[100];
	scanf("%s", &name);
	printf("Input messages\n");
	while (true)
	{
		char mes[100];
		gets_s(mes);
		if (strcmp(mes, "break")==0)
			break;
		SendData2Server(mes, name);
		
	}
	printf("Session is closed\n");
	Sleep(1000);
	return 0;
}

