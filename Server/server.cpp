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
FILE* log, * globalhistory, * personalchat;
int Counter = 0, countP = 0, countL = 0;
char nicks[100][256], passwords[100][50], logins[100][50];
char filename[200], history[256];
char interlocutors[100][50] = { 0 };

int LoginNumber(char login1[], char login2[])
{
	int c = max(strlen(login1), strlen(login2));
	for (int i = 0; i < c; ++i)
	{
		if (login1[i] < login2[i])
			return 1;
		if (login1[i] > login2[i])
			return 2;
	}
}

void AddtoHistory(char filename[], char msg[], char flag)
{
	if (flag == 'g')
	{
		if ((globalhistory = fopen("globalchat.txt", "a+")) == NULL)
		{
			printf("Error opening historyfile");
			return;
		}
		fseek(globalhistory, NULL, SEEK_END);
		fputs(msg, globalhistory);
		fclose(globalhistory);
	}
	if (flag == 'p')
	{
		if ((personalchat = fopen(filename, "a+")) == NULL)
		{
			printf("Error opening historyfile");
			return;
		}
		fseek(globalhistory, NULL, SEEK_END);
		fputs(msg, personalchat);
		fclose(personalchat);
	}
}

void SendHistory(char log1[], char log2[], SOCKET user)
{
	int l;
	char historymsg[256];
	l = LoginNumber(log1, log2);
	if (l == 1)
	{
		sprintf(filename, "%s %s", log1, log2);
	}
	else
	{
		sprintf(filename, "%s %s", log2, log1);
	}
	strcat(filename, ".txt");
	if ((personalchat = fopen(filename, "a+")) == NULL)
	{
		printf("Error opening historyfile");
		return;
	}
	fseek(globalhistory, NULL, SEEK_SET);
	int fl = 0;
	while (fgets(historymsg, 256, personalchat))
	{
		++fl;
		send(user, historymsg, sizeof(historymsg), NULL);
	}
	if (fl==0)
		send(user, "This is a new chat\nSend first message\n", sizeof("This is a new chat\nSend first message\n"), NULL);
	fclose(personalchat);
	send(user, "EOH", sizeof("EOH"), NULL);
}

void ClientHandler(int index) {
	char msg[256], hismsg[256];
	int hisflag, hisoutput = -1;
	while (true) {
		if (recv(Connections[index], msg, sizeof(msg), NULL) > 0) {
			if (strcmp(msg, "change") == 0) {
				char NewNick[50] = "", msg1[256]="";
				recv(Connections[index], NewNick, sizeof(NewNick), NULL);
				if (interlocutors[index] != 0)
					if (strcmp(NewNick, interlocutors[index]) != 0 || interlocutors[index][0] == 0)
					{
						SendHistory(nicks[index], NewNick, Connections[index]);
					}
				sprintf(interlocutors[index], "%s", NewNick);
				recv(Connections[index], msg1, sizeof(msg1), NULL);
				//вставить проверку существует ли такой пользователь вообще
				hisflag = LoginNumber(nicks[index], NewNick);
				sprintf_s(hismsg, "%s : %s\n", nicks[index], msg1);
				if (strlen(msg1) != 0)
				{
					if (hisflag == 1)
					{
						sprintf(filename, "%s %s", nicks[index], NewNick);
					}
					else
					{
						sprintf(filename, "%s %s", NewNick, nicks[index]);
					}
					strcat(filename, ".txt");
					//if (hisoutput)
					AddtoHistory(filename, hismsg, 'p');
				}
				for (int i = 0; i < 100; i++) {
				  if (strcmp(nicks[i], NewNick) == 0 && Connections[i] != INVALID_SOCKET) {
					char newmsg[100];
					sprintf_s(newmsg, "%s : %s\n", nicks[index], msg1);
					//fputs(newmsg, fp);
					//printf("%s %s", msg1, NewNick);
					send(Connections[i], newmsg, sizeof(newmsg), NULL);
					break;
				  }
				}
			}
			else{
				sprintf_s(hismsg, "%s : %s\n", nicks[index], msg);
				_strset(filename, 0);
				strcat(filename, "globalchat.txt");
				if (strlen(msg) != 0)
					AddtoHistory(filename, hismsg, 'g');
				for (int i = 0; i < Counter; i++) {
					if (i == index || Connections[i] == INVALID_SOCKET) {
						continue;
					}
					char newmsg[100];
					sprintf_s(newmsg, "%s : %s\n", nicks[index], msg);
					//fputs(newmsg, fp);
					if (strcmp(msg, "")!=0)
						send(Connections[i], newmsg, sizeof(newmsg), NULL);
				}
			}
		}
		else {
			::closesocket(Connections[index]);
			Connections[index] = INVALID_SOCKET;
			return;
		}

	}
}

void SignIn(char* nick, SOCKET Connections, char* temp, int index ) {
	char password[50] = "", nick1[50]="";
	int flag = 0;
	sprintf_s(nick1, "%s\n", nick);
	for (int i = 0; i < 100; i++) {
		if (strcmp(nick1, logins[i]) == 0 && strcmp(temp, "l")==0) {
			send(Connections, "Enter your password\n", sizeof("Enter your password\n"), NULL);
			recv(Connections, password, sizeof(password), NULL);
			//printf("%s\n", password);
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
	if (flag == 0 || strcmp(temp, "r") == 0) {
		recv(Connections, password, sizeof(password), NULL);
		strcpy(passwords[index], password);
		strcpy(logins[index], nick1);
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

int main(int argc, char* argv[]) {
	
	if ((log = fopen("log.txt", "a+")) == NULL)
	{
		printf("Error opening logfile");
		return -1;
	}
	if ((globalhistory = fopen("globalchat.txt", "a+")) == NULL)
	{
		printf("Error opening logfile");
		return -1;
	}
	fclose(globalhistory);
	for (int i = 0; i < 100; i++) {
		strcpy(passwords[i], "");
		strcpy(logins[i], "");
	}
	while (!(feof(log))) {
		fgets(logins[countL++], 256, log);
		fgets(passwords[countP++], 256, log);
	}

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
	addr.sin_port = htons(5510);

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
			
			char temp[10];
			recv(Connections[i], nicks[i], sizeof(nicks[i]), NULL);//Принимает ник и записывает в массив nicks[i]
			recv(Connections[i], temp, sizeof(temp), NULL);
			if (strcmp(temp, "r")==0)
				CheckLogin(nicks[i], Connections[i], i);
			SignIn(nicks[i], Connections[i], temp, i);
			Counter++;
			if ((globalhistory = fopen("globalchat.txt", "a+")) == NULL)
			{
				printf("Error opening logfile");
				return -1;
			}
			fseek(globalhistory, 0L, SEEK_SET);
			while (fgets(history, 256, globalhistory))
			{
				send(Connections[i], history, sizeof(history), NULL);
			}
			fclose(globalhistory);
			send(Connections[i], "EOH", sizeof("EOH"), NULL);

			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
		}
	}

	system("pause");
	return 0;
}