#include "CppSockets/CppSockets.h"
#include "CppArgs.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace CppSockets;


void handleclient(Sock sock,ServerChannels &channels){
	 while(true){
     vector<string> data=split("/",sock.recv());
	 for(auto& i :data)
	    cout<<i<<endl;
	 if(!sock.server.runclient){cout<<to_string(ntohs(sock.addr.sin_port))<<" disconnected !"<<endl;break;}
     channels.run(data[0],sock,data[1]);
	 }
}

int main(){
    socketServer server = socketServer(AF_INET,SOCK_STREAM);
	server.Bind(54000,"127.0.0.1");
	ServerChannels channels =ServerChannels();
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
    return 0;
}
