// server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#define NOMINMAX
#define HAVE_STRUCT_TIMESPEC
#define _CRT_SECURE_NO_WARNINGS
#include <pthread.h>
#pragma comment(lib, "ws2_32.lib")
#include <winsock.h>
#include <stdio.h>
#include <Windows.h>

pthread_mutex_t mutex;
pthread_mutex_t mutex_file;

SOCKET Connections[100];
int Counter = 0;
char choice[] = "Select the interlocutor(s) (enter the numbers separated by a space)\n";
char nousers[] = "No users online\nPlease wait a request to chat\n";
char nickstosend[300] = { 0 }, num, inlocs[256] = { 0 };
int inls[100] = { 0 }, inlscounter = 0;
char nicks[100][256];//Первое[] - количество ников. 2[] - максимальное количество знаков в нике
FILE* fp;
void ClientHandler(int index) {
	char msg[256];
	while (true) {
		if (recv(Connections[index], msg, sizeof(msg), NULL) > 0) {
			for (int i = 0; i < Counter; i++) {
				if (i == index || Connections[i] == INVALID_SOCKET) {
					continue;
				}
				char newmsg[100];
				sprintf_s(newmsg, "%s : %s\n", nicks[index], msg);
				fputs(newmsg, fp);
				send(Connections[i], newmsg, sizeof(newmsg), NULL);
				
			}
		}
		else {
			::closesocket(Connections[index]);
			Connections[index] = INVALID_SOCKET;
			return;
		}

	}
}
void History(SOCKET connections) {
	char historymsg[100]="";
	while (!feof(fp)) {
		if (!feof(fp)) {
			fgets(historymsg, 256, fp);
			printf("%s", historymsg);
			send(connections, historymsg, sizeof(historymsg), NULL);
		}
	
	}
	fseek(fp, 0L, SEEK_SET);
}

int main(int argc, char* argv[]) {
	
	if ((fp = fopen("history.txt", "a+")) == NULL)
	{
		printf("Anaible to open file");
		getchar();
		return 0;
	}


	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		printf("Error\n");
		exit(1);
	}
	printf("Server is started\n");
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5510);//port number is for example, must be more than 1024


	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	for (int i = 0; i < 100; i++) {
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			printf("Error\n");
		}
		else {
			printf("Client Connected!\n");
			char msg[256] = { 0 };
			Connections[i] = newConnection;
			if (!(getc(fp) == EOF))
				History(Connections[i]);
			recv(Connections[i], nicks[i], sizeof(nicks[i]), NULL);//Принимает ник и записывает в массив nicks[i]
			Counter++;
			if (Counter != 1)
			{
				send(Connections[i], choice, sizeof(choice), NULL); //отправляет запрос на выбор собеседников
				for (int j = 0; i < Counter; ++i) // отправляет ники подключенных пользователей
				{
					if (i != j)
					{
						nickstosend[0] = j + 49;
						nickstosend[1] = '.';
						nickstosend[2] = ' ';
						strcat(nickstosend, nicks[j]);
						strcat(nickstosend, "\n");
						send(Connections[i], nickstosend, sizeof(nickstosend), NULL);
						_strset(nickstosend, 0);
					}
				}
				recv(Connections[i], inlocs, sizeof(inlocs), NULL); // принимает список собеседников
				for (int j = 0; j < strlen(inlocs); ++j)
				{
					if (inlocs[j] != ' ')
					{
						inls[inlscounter] = inls[inlscounter] * 10 + (48 + inlocs[j]);
					}
					else
					{
						inlscounter++;
					}
				}
			}
			else
			{
				send(Connections[i], nousers, sizeof(nousers), NULL);
			}
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
		}
	}

	fclose(fp);
	system("pause");
	return 0;
}