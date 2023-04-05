#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <thread>
#include <map>
#include <vector>
#include "cppargs.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

namespace socketClient{
class socketClient{
    public:
    string ipAddress = "127.0.0.1";
    int port = 54000;
    WSAData data;
    WORD ver = MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &data);
    SOCKET sock;
    int af,type;
    char buf[4096];

    socketClient(int af,int type):af(af),type(type){
        this->sock = socket(af, type, 0);
        if (wsResult != 0)
            perror("Can't start Winsock, Err #\n");
        if (sock == INVALID_SOCKET){
        cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
        WSACleanup();
        exit(0);
        }

    }
    
    void Connect(string ipAddress ,int port = 54000){
       this->ipAddress=ipAddress;
       this->port = port;
       sockaddr_in hint;
       hint.sin_family = af;
       hint.sin_port = htons(this->port);
       inet_pton(af, ipAddress.c_str(), &hint.sin_addr);
       int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
       if (connResult == SOCKET_ERROR){
           cerr<<"Can't connect to server, Err #" << WSAGetLastError() << endl;
           closesocket(sock);
           WSACleanup();
           exit(0);
       }
    }
    
    int sendData(string data){
        return send(sock,data.c_str(),data.size() + 1, 0);
    }

    string recvData(){
           ZeroMemory(buf, 4096);
           int bytesReceived = recv(sock, buf, 4096, 0);
           return string(buf, 0, bytesReceived);

    }

    void close(){
         closesocket(sock);
         WSACleanup();
    }
};


struct Channels{
       map<string,void(*)(socketClient sock,string data)> channels;
       Channels(){};
       void add(string name,void(*func)(socketClient sock,string data)){
            channels[name]=func;
       }
       void run(string name,socketClient sock,string data){
            channels[name](sock,data);
       }  
};
}

void handle_connection(socketClient &client,Channels &channels){
	 while(true){
		  vector<string> data=split("/",client.recvData());
		  channels.run(data[0],client,data[1]);
	 }
};

int main(){
	socketClient client = socketClient(AF_INET,SOCK_STREAM);
	client.Connect("127.0.0.1",54000);
	Channels channels;
	channels.add("echo",[](socketClient client,string data){
		cout<<"[echo]"<<data<<endl;
	});
	thread(handle_connection,ref(client),ref(channels)).detach();
    while(true){
		string userInput;
		cout<<"\n>>>";
		getline(cin,userInput);
		int rv = client.sendData(userInput);
		if(SOCKET_ERROR == rv)
		    break;
	}
	client.close();
}
