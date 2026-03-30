#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
const char* SERVER_IP="127.00.1";
const int SERVER_PORT=8080;
const int BUFFER_SIZE=1024;
int main (){
    //семейство ти ппротокол
    int clientSocket = socket(AF_INET, SOCK_STREAM,0);// АВТОВЫБОР
    if(clientSocket<0){
        std::cerr << " "<<std::endl;
        return 1;
    }
    std::cout<<"сокет создан"<<std::endl;
    sockaddr_in serverAddr;
    memset(& serverAddr, 0, sizeof(serverAddr));
    serverAddr. sin_family = AF_INET;
    serverAddr. sin_port = htons (SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr. sin_addr);
    if (connect(clientSocket, (sockaddr*)& serverAddr, sizeof(serverAddr)) < 0){
        close(clientSocket);
        return 2;
    }
    std::cout << "коннект ура :)"<<std::endl;
    for (int i = 2; i<=4; i++){
        std::string message = "ping";
        send(clientSocket, message.c_str(),message.length(),0);
        char buffer[BUFFER_SIZE];
        memset (buffer, 0, BUFFER_SIZE);
        int bytesRecievd = recv(clientSocket, buffer, BUFFER_SIZE-1, 0);
        if (bytesRecievd>0){
            std::cout<<buffer<<std::endl;
        }
    }
    send(clientSocket, "exit",4,0);
    close(clientSocket);
    return 0;
}