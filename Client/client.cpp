//#include "pch.h"
#pragma comment(lib, "ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS
#include <winsock.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>

SOCKET Connection;
char  msg1[256], newmsg[256];
char history[256];
char ti[5];
int flagchat = 0;

/*void NowTime()
{
	time_t ttime;
	char buf[30] = { 0 };
	ttime = time(NULL);
	ctime_s(buf, 30, &ttime);
	ti[0] = buf[11];
	ti[1] = buf[12];
	ti[2] = buf[13];
	ti[3] = buf[14];
	ti[4] = buf[15];
}*/
void ClientHandler() 
{
	while (true) 
	{
		if (flagchat == 0)
		{
			recv(Connection, newmsg, sizeof(newmsg), NULL);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
			printf("%s", newmsg);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		}
	}
}

void SignIn() 
{
	char  SignInMsg[100] = "", password[50] = "", nick[256]="";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	printf("Input your login\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	scanf("%s", &nick);
	send(Connection, nick, sizeof(nick), NULL);//Отправляет свой ник серверу
	send(Connection, "l", sizeof("l"), NULL);
	while (1) 
	{
		recv(Connection, SignInMsg, sizeof(SignInMsg), NULL);

		if (strcmp(SignInMsg, "logged in to account\n") == 0) 
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			printf("logged in to account\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			break;
		}
		if (strcmp(SignInMsg, "Enter your password\n") == 0) 
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
			printf("Enter your password\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			scanf("%s", &password);
			sprintf_s(password, "%s\n", password);
			send(Connection, password, sizeof(password), NULL);
		}
		if (strcmp(SignInMsg, "Wrong password!Try again\n") == 0) 
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
			printf("Wrong password!Try again\nEnter your password\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			scanf("%s", &password);
			sprintf_s(password, "%s\n", password);
			send(Connection, password, sizeof(password), NULL);
		}
		if (strcmp(SignInMsg, "Users is not Found!\n") == 0) 
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
			printf("Users is not Found!\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
			printf("Input:\n't' - try again\n'r' - registration\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			char temp[10];
			scanf("%s", &temp);
			if (strcmp(temp, "t") == 0) 
			{
				scanf("%s", &nick);
				send(Connection, "again", sizeof("again"), NULL);
				send(Connection, nick, sizeof(nick), NULL);
				continue;
			}
		}
	}
}

void Registration() 
{
	char NewLog[50] = "", NewPassword[50] = "", NewRassword1[50] = "", SignInMsg[100]="";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	printf("Input Login\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	scanf("%s", &NewLog);
	send(Connection, NewLog, sizeof(NewLog), NULL);
	send(Connection, "r", sizeof("r"), NULL);
	while (1) 
	{
		recv(Connection, SignInMsg, sizeof(SignInMsg), NULL);
		if (strcmp(SignInMsg, "login not available! try another\n") == 0) 
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
			printf("login not available! try another\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
			printf("Input Login\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			scanf("%s", &NewLog);
			send(Connection, NewLog, sizeof(NewLog), NULL);
		}
		else
			break;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	printf("Input password\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	scanf("%s", &NewPassword);
	while (1) 
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
		printf("Repeat password\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		scanf("%s", &NewRassword1);
		if (strcmp(NewPassword, NewRassword1) == 0)
			break;
	}
	//send(Connection, NewLog, sizeof(NewLog), NULL);
	sprintf_s(NewPassword, "%s\n", NewPassword);
	send(Connection, NewPassword, sizeof(NewPassword), NULL);
}

int main(int argc, char* argv[]) 
{
	//WSAStartup
	WSAData wsaData;
	char temp = 0, temp1 = 0, mode = 'A'; // A - all, P - personal
	int flagredorlog = 0;
	WORD DLLVersion = MAKEWORD(2, 1);

	if (WSAStartup(DLLVersion, &wsaData) != 0) 
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
		printf("Error\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5510); //the same as in server
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) 
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
		printf("Error: Failed connection\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		system("pause");
		return 1;
	}
	while (1)
	{ 
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
		printf("Hello!\nYou want to register or login?\n'r' - registration\n'l'- login\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		scanf("%c", &temp);
		
		while (temp1!='\n')
		{
			scanf("%c", &temp1);
			++flagredorlog;
		}
		if (flagredorlog == 1)
		{
			if (temp == 'l' || temp == 'r')
				break;
		}
		flagredorlog = 0;
		temp1 = 0;
	}
	if (temp == 'l')
		SignIn();
	if (temp == 'r')
		Registration();
	
	//char NewLog[50] = "";
	//printf("Input Login\n");
	//scanf("%s", &NewLog);
	//send(Connection, NewLog, sizeof(NewLog), NULL);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	printf("Connected!\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	while (recv(Connection, history, sizeof(history), NULL))
	{
		if (strcmp(history, "EOH") == 0)
			break;
		else
			printf("%s", history);
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 240);
	printf("Enter 'change' for send personal messages\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);
	char NewNick[50] = "";
	while (true) {
		gets_s(msg1);
		if (strcmp(msg1, "change") == 0) {
			mode = 'P';
			flagchat = 1;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 240);
			printf("Enter 'exit' for send messages to all\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		}
		if (strcmp(msg1, "exit") == 0) {
			mode = 'A';
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 240);
			printf("Enter 'change' for send personal messages\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		}
		if (mode == 'P') {
			if (strcmp(msg1, "change") == 0) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
				printf("Input the username\n");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				scanf("%s", &NewNick);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
				printf("Input message\n");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				gets_s(msg1);
			}

			send(Connection, "change", sizeof("change"), NULL);
			send(Connection, NewNick, sizeof(NewNick), NULL); 

			if (flagchat == 1)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				puts("=============Personal chat==============");
				while (recv(Connection, history, sizeof(history), NULL))
				{
					if (strcmp(history, "EOH") == 0)
						break;
					else
						printf("%s", history);
				}
				flagchat = 0;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				puts("\n");
			}
			send(Connection, msg1, sizeof(msg1), NULL); 
		}
		else if (strcmp(msg1, "exit")!=0)
			send(Connection, msg1, sizeof(msg1), NULL);
		Sleep(10);
	}

	system("pause");
	return 0;
}