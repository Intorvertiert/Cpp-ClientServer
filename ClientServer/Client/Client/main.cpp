#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma warning(disable: 4996)

SOCKET Connection;

enum Packet
{
	P_ChatMessage,
	P_Test
};

bool ProcessPacket(Packet packettype)
{
	switch (packettype)
	{
	case P_ChatMessage:
	{
		int msg_size;
		recv(Connection, (char*)& msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connection, msg, msg_size, NULL);
		std::cout << msg << std::endl;
		delete[] msg;
		break;
	}
	case P_Test:
		std::cout << "Test packet\n";
		break;
	default:
		std::cout << "Unrecognized packet: " << packettype << std::endl;
		break;
	}
	return true;
}


void ClientHandler()
{
	Packet packettype;
	while (true)
	{
		recv(Connection, (char*)& packettype, sizeof(Packet), NULL);

		if (!ProcessPacket(packettype))
		{
			break;
		}
	}
	closesocket(Connection);
}


int main(int argc, char** argv)
{
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1); //запрашиваемая версия библиотеки winsock
	if (WSAStartup(DLLVersion, &wsaData) != 0) //проверка на загрузку библиотеки
	{
		std::cout << "Error load" << std::endl;
		exit(1);
	}
	SOCKADDR_IN addr; //структура хранения адреса (для интернет протоколов)
	int sizeOfaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");//local host adders
	addr.sin_port = htons(1111);//портировка программы с поступающими данными
	addr.sin_family = AF_INET;//семейстро протоколов  (для интернета AF_INET)

	Connection = socket(AF_INET, SOCK_STREAM, NULL);  //создание нового сокета для соединения с сервером
	if(connect(Connection, (SOCKADDR*)& addr, sizeof(addr)) != 0)
	{
		std::cout << "Error: faled connect to server.\n";
		Sleep(100000);
		exit(1);
	}
	std::cout << "Connected!\n";
	

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	std::string msg1;
	while (true)
	{
		std::getline(std::cin, msg1);
		size_t msg_size = msg1.size(); //размер сообщения

		Packet packettype = P_ChatMessage;
		send(Connection, (char*)&packettype, sizeof(Packet), NULL);

		send(Connection, (char*)& msg_size, sizeof(size_t), NULL);
		send(Connection, msg1.c_str(), msg_size, NULL);
		Sleep(1000);
	}

	system("pause >> NULL");
	return 0;
}