#include <iostream>
#include <ws2tcpip.h>
#include <vector>
#include "cppargs.h"
#include <map>
#include <thread>


#pragma comment (lib,"ws2_32.lib")
using namespace std;

struct sockaddrdata{
	   SOCKET client;
	   sockaddr_in client_addr;
	   sockaddrdata(SOCKET client,sockaddr_in client_addr):client(client),client_addr(client_addr){};
};

struct conndata{
	   string host;
	   string port;
	   conndata(string host,string port) :host(host),port(port){}
};

class socketserver{
       public:
	      SOCKET server;       
		  bool runclient=true; 
          WSADATA wsData;
          WORD ver = MAKEWORD(2,2);
          int wsOK = WSAStartup(ver,&wsData);
		  int af,type;
	   
	   socketserver(int af,int type): af(af),type(type){
          server = socket(af,type,0);
	   }

       void Bind(int port,string ipAddress){
		    sockaddr_in hint;
            hint.sin_family = af;
            hint.sin_port = htons(port);
			inet_pton(af, ipAddress.c_str(), &hint.sin_addr);
            //hint.sin_addr.S_un.S_addr = INADDR_ANY;
            bind(server,(sockaddr*)&hint,sizeof(hint)); 
            listen(server,SOMAXCONN);
	   }

	   string recvdata(SOCKET target){
		    char buf[4096];
			ZeroMemory(buf, 4096);
			cout<<target<<" recving "<<endl;
		    int bytesReceived = recv(target, buf, 4096, 0);
	     	if (bytesReceived == SOCKET_ERROR){cerr << "Error in recv(). Quitting" << endl;runclient = false;}
		    if (bytesReceived == 0){cout << "Client disconnected " << endl;runclient = false;}
			return string(buf, 0, bytesReceived);
	   }
	   
	   void sendData(SOCKET target,string data){
		    cout<<target<<" sending  "<<endl;
		    send(target,data.c_str(),data.size() + 1, 0);
	   }

	   sockaddrdata acceptconnection(){
		      sockaddr_in client;
              int client_size = sizeof(client);
              SOCKET clientsocket = accept(server,(sockaddr*)&client,&client_size);
			  return sockaddrdata(clientsocket,client);
	   }

	   void closeConnection(){
	       closesocket(server);
           closesocket(server);
           WSACleanup();
	   }
};

class Sock{
       public:
	   socketserver server;
	   SOCKET client;
	   sockaddr_in addr;
	   char host[NI_MAXHOST];		// Client's remote name
	   char service[NI_MAXSERV];
	   map<string,void(*)(Sock sock)> channels;	

	   Sock(socketserver server, sockaddrdata &data):server(server){
		    this->client = data.client;
			this->addr = data.client_addr;
	   }

	   string recv(){
		       return server.recvdata(client);
	   }  

	   void send(string data){
		    server.sendData(client,data);
	   }

	   conndata show(){
	         ZeroMemory(host, NI_MAXHOST); 
	         ZeroMemory(service, NI_MAXSERV);
			 if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
 	        	cout << host << " connected on port " << service << endl;
			 else
			 {
				inet_ntop(AF_INET, &addr.sin_addr, host, NI_MAXHOST);
				cout << host << " connected on port " <<ntohs(addr.sin_port) << endl;
					return conndata(host,to_string(ntohs(addr.sin_port)));
			 }
			 return conndata("","");
	   }
};

struct Channels{
	   map<string,void(*)(Sock sock,string data)> channels;
       Channels(){};
	   void add(string name,void(*func)(Sock sock,string data)){
		    channels[name]=func;
	   }
	   void run(string name,Sock sock,string data){
		    channels[name](sock,data);
	   }  
};

void handleclient(Sock sock,Channels &channels){
	 while(true){
     vector<string> data=split("/",sock.recv());
	 for(auto& i :data)
	    cout<<i<<endl;
	 if(!sock.server.runclient){cout<<to_string(ntohs(sock.addr.sin_port))<<" disconnected !"<<endl;break;}
     channels.run(data[0],sock,data[1]);
	 }
}


int main(){
    socketserver server = socketserver(AF_INET,SOCK_STREAM);
	server.Bind(54000,"127.0.0.1");
	Channels channels =Channels();
    channels.add("echo",[](Sock sock,string data){
        sock.send("echo/"+data);         
    });	
	while (true)
	{
	   cout<<"[!] server ready for new connections!"<<endl;
	   auto acceptedclient = server.acceptconnection();
       Sock conn = Sock(server,acceptedclient);
	   conn.show();
       thread(handleclient,conn,ref(channels)).detach();
	}
    server.closeConnection();

}
