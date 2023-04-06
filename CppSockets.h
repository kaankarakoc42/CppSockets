#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H
#include <map>
#include <string>
#include <ws2tcpip.h>
#define AF_INET 2
#define SOCK_STREAM 1

namespace CppSockets{

struct sockaddrdata{
	   SOCKET client;
	   sockaddr_in client_addr;
	   sockaddrdata(SOCKET client,sockaddr_in client_addr):client(client),client_addr(client_addr){};
};

struct conndata{
	   std::string host;
	   std::string port;
	   conndata(std::string host,std::string port) :host(host),port(port){}
};

class socketServer{
       public:
	      SOCKET server;       
		  bool runclient=true; 
          WSADATA wsData;
          WORD ver = MAKEWORD(2,2);
          int wsOK = WSAStartup(ver,&wsData);
		  int af,type;
	   
	   socketServer(int af,int type);
	   ~socketServer();

       void Bind(int port,std::string ipAddress);
	   std::string recvdata(SOCKET target);
	   void sendData(SOCKET target,std::string data);
	   sockaddrdata acceptconnection();
	   void closeConnection();
};

class Sock{
       public:
	   socketServer server;
	   SOCKET client;
	   sockaddr_in addr;
	   char host[NI_MAXHOST];		
	   char service[NI_MAXSERV];
	   std::map<std::string,void(*)(Sock sock)> channels;	

	   Sock(socketServer server, sockaddrdata &data);
       ~Sock();

	   std::string recv();
	   void send(std::string data);
	   conndata show();
};

struct ServerChannels{
	   std::map<std::string,void(*)(Sock sock,std::string data)> channels;
       ServerChannels();
	   void add(std::string name,void(*func)(Sock sock,std::string data));
	   void run(std::string name,Sock sock,std::string data);
};

class socketClient {
public:
    socketClient(int af, int type);
    ~socketClient();
    void Connect(std::string ip, int port);
    int sendData(std::string data);
    std::string recvData();
    void close();

private:
    int sock;
    char buf[4096];
};


class ClientChannels {
public:
    ClientChannels();
    void add(std::string name, void(*func)(socketClient sock, std::string data));
    void run(std::string name, socketClient sock, std::string data);

private:
    std::map<std::string, void(*)(socketClient, std::string)> channels;
};
}
#endif



/*
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
*/