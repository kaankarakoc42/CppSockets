#include "CppSockets.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")
//socketServer
CppSockets::socketServer::socketServer(int af,int type) :af(af),type(type){
          server = socket(af,type,0);
};

CppSockets::socketServer::~socketServer(){};

void CppSockets::socketServer::Bind(int port,std::string ipAddress){
		    sockaddr_in hint;
            hint.sin_family = af;
            hint.sin_port = htons(port);
			inet_pton(af, ipAddress.c_str(), &hint.sin_addr);
            //hint.sin_addr.S_un.S_addr = INADDR_ANY;
            bind(server,(sockaddr*)&hint,sizeof(hint)); 
            listen(server,SOMAXCONN);
}
std::string CppSockets::socketServer::recvdata(SOCKET target){
		    char buf[4096];
			ZeroMemory(buf, 4096);
			std::cout<<target<<" recving "<<std::endl;
		    int bytesReceived = recv(target, buf, 4096, 0);
	     	if (bytesReceived == SOCKET_ERROR){std::cerr << "Error in recv(). Quitting" << std::endl;runclient = false;}
		    if (bytesReceived == 0){std::cout << "Client disconnected " << std::endl;runclient = false;}
			return std::string(buf, 0, bytesReceived);
};
void CppSockets::socketServer::sendData(SOCKET target,std::string data){
		    send(target,data.c_str(),data.size() + 1, 0);
};

CppSockets::sockaddrdata CppSockets::socketServer::acceptconnection(){
		      sockaddr_in client;
              int client_size = sizeof(client);
              SOCKET clientsocket = accept(server,(sockaddr*)&client,&client_size);
			  return CppSockets::sockaddrdata(clientsocket,client);
};

void CppSockets::socketServer::closeConnection(){
	       closesocket(server);
           closesocket(server);
           WSACleanup();
};

//Sock
CppSockets::Sock::Sock(CppSockets::socketServer server, sockaddrdata &data):server(server){
		    this->client = data.client;
			this->addr = data.client_addr;
};

CppSockets::Sock::~Sock(){

};

std::string CppSockets::Sock::recv(){
		       return server.recvdata(client);
}  
void CppSockets::Sock::send(std::string data){
		    server.sendData(client,data);
}
CppSockets::conndata CppSockets::Sock::show(){
	         ZeroMemory(host, NI_MAXHOST); 
	         ZeroMemory(service, NI_MAXSERV);
			 if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
 	        	std::cout << host << " connected on port " << service << std::endl;
			 else
			 {
				inet_ntop(AF_INET, &addr.sin_addr, host, NI_MAXHOST);
				std::cout << host << " connected on port " <<ntohs(addr.sin_port) << std::endl;
					return CppSockets::conndata(host,std::to_string(ntohs(addr.sin_port)));
			 }
			 return CppSockets::conndata("","");
}

//ServerChannels
CppSockets::ServerChannels::ServerChannels(){};
void CppSockets::ServerChannels::add(std::string name,void(*func)(CppSockets::Sock sock,std::string data)){
		    channels[name]=func;
}
void CppSockets::ServerChannels::run(std::string name,CppSockets::Sock sock,std::string data){
		    channels[name](sock,data);
}  

//socketClient
CppSockets::socketClient::socketClient(int af, int type) {
    WSADATA wsaData;
    int rv = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rv != 0) {
        std::cerr << "WSAStartup failed with error: " << rv << std::endl;
        exit(1);
    }

    sock = socket(af, type, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }
}

CppSockets::socketClient::~socketClient() {
    close();
    
}

void CppSockets::socketClient::Connect(std::string ip, int port) {
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server.sin_addr);

    int rv = connect(sock, (sockaddr*)&server, sizeof(server));
    if (rv == SOCKET_ERROR) {
        std::cerr << "connect failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        exit(1);
    }
}

int CppSockets::socketClient::sendData(std::string data) {
    return send(sock, data.c_str(), data.size() + 1, 0);
}

std::string CppSockets::socketClient::recvData() {
    ZeroMemory(buf, 4096);
    int bytesReceived = recv(sock, buf, 4096, 0);
    return std::string(buf, 0, bytesReceived);
}

void CppSockets::socketClient::close() {
    closesocket(sock);
    WSACleanup();
}

//ClientChannels
CppSockets::ClientChannels::ClientChannels() {}

void CppSockets::ClientChannels::add(std::string name, void(*func)(socketClient sock, std::string data)) {
    channels[name] = func;
}

void CppSockets::ClientChannels::run(std::string name, socketClient sock, std::string data) {
    if (channels.find(name) != channels.end()) {
        channels[name](sock, data);
    }
    else {
        std::cerr << "Channel not found: " << name << std::endl;
    }
}