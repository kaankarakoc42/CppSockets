#include "CppSockets/CppSockets.h"
#include "CppArgs.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace CppSockets;

void handle_connection(socketClient &client,ClientChannels &channels){
	 while(true){
          try
          {
		  vector<string> data=split("/",client.recvData());// I recomment you to use json to pass data there is nlohmann json library for cpp
		  channels.run(data[0],client,data[1]);
          }
          catch(const std::exception& e)
          {
            std::cerr << e.what() << '\n';
            break;
          }
          
	 }
};

int main(){
	socketClient client = socketClient(AF_INET,SOCK_STREAM);
	client.Connect("127.0.0.1",54000);
	ClientChannels channels;
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