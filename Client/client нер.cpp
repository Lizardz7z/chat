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
char  msg1[256], newmsg[256], buf[256] = { 0 };


void ClientHandler() {
	while (true) {
		
		recv(Connection, newmsg, sizeof(newmsg), NULL);

		printf("%s", newmsg);
	}
}


void SignIn() {
	char  SignInMsg[100] = "", password[50] = "", nick[256]="";
	printf("Input your login\n");
	scanf("%s", &nick);
	send(Connection, nick, sizeof(nick), NULL);//Отправляет свой ник серверу
	while (1) {
		recv(Connection, SignInMsg, sizeof(SignInMsg), NULL);

		if (strcmp(SignInMsg, "logged in to account\n") == 0) {
			printf("logged in to account\n");
			break;
		}
		if (strcmp(SignInMsg, "Enter your password\n") == 0) {
			printf("Enter your password\n");
			scanf("%s", &password);
			sprintf_s(password, "%s\n", password);
			send(Connection, password, sizeof(password), NULL);
		}
		if (strcmp(SignInMsg, "Wrong password!Try again\n") == 0) {
			printf("Wrong password!Try again\nEnter your password\n");
			scanf("%s", &password);
			sprintf_s(password, "%s\n", password);
			send(Connection, password, sizeof(password), NULL);
		}
		if (strcmp(SignInMsg, "Users is not Found!\n") == 0) {
			printf("Users is not Found!\nInput:\n't' - try again\n'r' - registration\n");
			char temp[10];
			scanf("%s", &temp);
			if (strcmp(temp, "t") == 0) {
				scanf("%s", &nick);
				send(Connection, "again", sizeof("again"), NULL);
				send(Connection, nick, sizeof(nick), NULL);
				continue;
			}
			/*if (temp == 'r') {
				scanf("%s", &nick);
				send(Connection, nick, sizeof(nick), NULL);
			}*/
		}

	}
}
void Registration() {
	char NewLog[50] = "", NewPassword[50] = "", NewRassword1[50] = "", SignInMsg[100]="";
	printf("Input Login\n");
	scanf("%s", &NewLog);
	send(Connection, NewLog, sizeof(NewLog), NULL);
	while (1) {
		recv(Connection, SignInMsg, sizeof(SignInMsg), NULL);
		if (strcmp(SignInMsg, "login not available! try another\n") == 0) {
			printf("login not available! try another\nInput Login\n");
			scanf("%s", &NewLog);
			send(Connection, NewLog, sizeof(NewLog), NULL);
		}
		else
			break;
	}
	printf("Input password\n");
	scanf("%s", &NewPassword);
	while (1) {
		printf("Repeat password\n");
		scanf("%s", &NewRassword1);
		if (strcmp(NewPassword, NewRassword1) == 0)
			break;
	}
	//send(Connection, NewLog, sizeof(NewLog), NULL);
	sprintf_s(NewPassword, "%s\n", NewPassword);
	send(Connection, NewPassword, sizeof(NewPassword), NULL);
}

int main(int argc, char* argv[]) {

		//WSAStartup
	WSAData wsaData;
	char temp;
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

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		printf("Error: Failed connection\n");
		system("pause");
		return 1;
	}

	printf("Hello!\nYou want to register or login?\n'r' - registration\n'l'- login\n");
	scanf("%c", &temp);

	if (temp == 'l')
	{
		send(Connection, "l", sizeof("l"), NULL);
		SignIn();
	}
	if (temp == 'r')
	{
		send(Connection, "r", sizeof("l"), NULL);
		Registration();
	}
	//char NewLog[50] = "";
	//printf("Input Login\n");
	//scanf("%s", &NewLog);
	//send(Connection, NewLog, sizeof(NewLog), NULL);
	printf("Connected!\n");

	recv(Connection, buf, sizeof(buf), NULL);
	if (strcmp(buf, "No available interlocutors\nYou are connected to the global chat") == 0)
	{
		puts(buf);
	}
	else
	{
		puts(buf);
		scanf("%s", &buf);
		send(Connection, buf, sizeof(buf), NULL);
		while (1)
		{
			recv(Connection, buf, sizeof(buf), NULL);
			if (strcmp(buf, "Interlocutor is found\n") == 0)
			{
				puts(buf);
				break;
			}
			puts(buf);
			scanf("%s", &buf);
			send(Connection, buf, sizeof(buf), NULL);
		}
	}
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	while (true) {
		gets_s(msg1);		
		send(Connection, msg1, sizeof(msg1), NULL);
		Sleep(10);
	}

	system("pause");
	return 0;
}