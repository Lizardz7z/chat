// server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#define NOMINMAX
#define HAVE_STRUCT_TIMESPEC
#define _CRT_SECURE_NO_WARNINGS
#include <pthread.h>
#pragma comment(lib, "ws2_32.lib")
#include <winsock.h>
#include <stdio.h>
#include <Windows.h>
#define N 1000
pthread_mutex_t mutex;
pthread_mutex_t mutex_file;

SOCKET Connections[100];
int Counter = 0;
char nicks[100][N], passwords[100][N], logins[100][N];;//Первое[] - количество ников. 2[] - максимальное количество знаков в нике
FILE* fp, *log;
char interlocutors[100][N], ilbuf[N];
int intloccounter = 0;

int InterlocutorIndex(int index)
{
	for (int i = 0; i < Counter; ++i)
	{
		if (strcmp(interlocutors[index], nicks[i]) == 0)
			return i;
	}
}
void ClientHandler(int index) {
	char msg[N];
	int flag = 0, intlocindex = -1;
	if (strcmp(interlocutors[index], "globalchat") == 0)
		flag = 1;
	while (true) {
		if (recv(Connections[index], msg, sizeof(msg), NULL) > 0) 
		{
			if (flag == 1)
			{
				for (int i = 0; i < Counter; i++) 
				{
					if (i == index || Connections[i] == INVALID_SOCKET) 
					{
						continue;
					}
					char newmsg[100];
					sprintf_s(newmsg, "%s : %s\n", nicks[index], msg);
					fputs(newmsg, fp);
					send(Connections[i], newmsg, sizeof(newmsg), NULL);
				}
			}
			else
			{
				intlocindex = InterlocutorIndex(index);
				char newmsg[100];
				sprintf_s(newmsg, "%s : %s\n", nicks[index], msg);
				fputs(newmsg, fp);
				send(Connections[intlocindex], newmsg, sizeof(newmsg), NULL);
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
	char historymsg[256]="";
	while (!feof(fp)) {
			fgets(historymsg, 256, fp);
			//printf("%s", historymsg);
			send(connections, historymsg, sizeof(historymsg), NULL);	
	}
	fseek(fp, 0L, SEEK_SET);
}
void SignIn(char* nick, SOCKET Connections, char* temp, int index ) {
	char password[50] = "", nick1[50]="";
	int flag = 0;
	sprintf_s(nick1, "%s\n", nick);
	for (int i = 0; i < 100; i++) {
		if (strcmp(nick1, logins[i]) == 0 && strcmp(temp, "l")==0) {
			send(Connections, "Enter your password\n", sizeof("Enter your password\n"), NULL);
			recv(Connections, password, sizeof(password), NULL);
			printf("%s\n", password);
			if (strcmp(passwords[i], password) == 0) {
				send(Connections, "logged in to account\n", sizeof("logged in to account\n"), NULL);
				flag = 1;
				break;
			}
			else {
				while (1) {
					send(Connections, "Wrong password!Try again\n", sizeof("Wrong password! Try again\n"), NULL);
					recv(Connections, password, sizeof(password), NULL);

					if (strcmp(passwords[i], password) == 0) {
						send(Connections, "logged in to account\n", sizeof("logged in to account\n"), NULL);
						flag = 1;
						break;
					}
				}
				break;
			}
		}
	}
	if (flag == 0) {
		recv(Connections, password, sizeof(password), NULL);
		strcpy(logins[index], nick1);
		strcpy(passwords[index], password);
		fputs(nick1, log);
		fputs(password, log);
	}
}
void CheckLogin(char* nick, SOCKET Connections, int index) {
	char nick1[50] = "";
	while(1){
	int flag = 0;
	if (strcmp(nick1,"")==0)
		sprintf_s(nick1, "%s\n", nick);
	else
		sprintf_s(nick1, "%s\n", nick1);
	for (int i = 0; i < 100; i++) {
		if (strcmp(nick1, logins[i]) == 0) {
			printf("login not available! try another\n");
				send(Connections, "login not available! try another\n", sizeof("login not available! try another\n"), NULL);
				recv(Connections, nicks[index], sizeof(nicks[index]), NULL);
				flag = 1;
				break;
			}
		}
	if (flag == 0) {
		send(Connections, "ok\n", sizeof("ok\n"), NULL);
		break;
	}
	}
}

void CheckInterlocutor(char* nick, SOCKET Connections, int index) {
	char nick1[N] = "";
	while (1) {
		int flag = -1;
		if (strcmp(nick1, "") == 0)
			sprintf_s(nick1, "%s\n", nick);
		for (int i = 0; i < 100; i++) {
			if (strcmp(nick1, logins[i]) == 0) {
				send(Connections, "Interlocutor is found\n", sizeof("Interlocutor is found\n"), NULL);
				flag = 1;
				break;
			}
			else
				flag = 0;
		}
		if (flag == 1)
		{
			break;
		}
		if (flag == 0) {
			send(Connections, "Interlocutor does not exist! Check that the input is correct or enter another\n", sizeof("Interlocutor does not exist!Check that the input is correct or enter another\n"), NULL);
			recv(Connections, nick1, sizeof(nick1), NULL);
		}
	}
}
int main(int argc, char* argv[]) {
	
	if ((log = fopen("log.txt", "a+")) == NULL || (fp = fopen("history.txt", "a+")) == NULL)
	{
		printf("Unable to open file");
		return 0;
	}
	
	int countP = 0, countL = 0;

	for (int i = 0; i < 100; i++) {
		strcpy(passwords[i], "");
		strcpy(logins[i], "");
	}

	while (!(feof(log))) {
		fgets(logins[countL++], N, log);
		fgets(passwords[countP++], N, log);
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
			char msg[N] = { 0 };
			Connections[i] = newConnection;
			
			char temp[256];
			recv(Connections[i], temp, sizeof(temp), NULL);
			recv(Connections[i], nicks[i], sizeof(nicks[i]), NULL);//Принимает ник и записывает в массив nicks[i]
			if (strcmp(temp, "r")==0)
				CheckLogin(nicks[i], Connections[i], i);
			SignIn(nicks[i], Connections[i], temp, i);
			Counter++;
			if (countL <= 1)
			{
				send(Connections[i], "No available interlocutors\nYou are connected to the global chat", sizeof("No available interlocutors\nYou are connected to the global chat"), NULL);
				sprintf(interlocutors[i], "globalchat");
			}
			else
			{
				send(Connections[i], "Enter interlocutor's login\n", sizeof("Enter interlocutor's login\n"), NULL);
				recv(Connections[i], ilbuf, sizeof(ilbuf), NULL);
				CheckInterlocutor(ilbuf, Connections[i], i);
				strcat(interlocutors[i], ilbuf);
			}
			if (!(getc(fp) == EOF))
				History(Connections[i]);
			printf("\n");
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
			
		}
	}

	fclose(fp);
	system("pause");
	return 0;
}