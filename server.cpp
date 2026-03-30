#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

using namespace std;
//инфо
const string AUTHOR = "Miao";
const string VERSION = "1.2.3.";
const int PORT = 8080;
const int BUFFER_SIZE = 1024;

//проверка чисел на натуральность
bool is_prime(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}


string generate_spiral() {
    const int size = 100;
    vector<vector<char>> grid(size, vector<char>(size, ' '));
    int x = size / 2, y = size / 2;
    int step_limit = 1, step_count = 0, direction = 0; 
    int dx[] = {1, 0, -1, 0}, dy[] = {0, -1, 0, 1};

    //делаем снег 
    for (int n = 1; n <= size * size; ++n) {
        if (x >= 0 && x < size && y >= 0 && y < size) {
            grid[y][x] = is_prime(n) ? '*' : ' ';
        }
        x += dx[direction];
        y += dy[direction];
        step_count++;
        if (step_count == step_limit) {
            step_count = 0;
            direction = (direction + 1) % 4;
            if (direction % 2 == 0) step_limit++;
        }
    }
    stringstream ss;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            ss << grid[i][j] << " ";
        }
        ss << "\n";
    }
    cout << "gde-to padal sneg...\n"<<endl;
    return ss.str();
}

//инфо помогашки
int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-v" || arg == "--version") {
            cout << "Version: " << VERSION << endl;
            return 0;
        }
        if (arg == "-a" || arg == "--autho") {
            cout << "Author: " << AUTHOR << endl;
            return 0;
        }
        
        if (arg == "-s" || arg == "--spiral") {
            cout << generate_spiral();
            return 0;
        }
        if (arg == "-h" || arg == "--help") {
            cout << "help u :)\n"<<"To see snow, enter into the console: './server -s'\n" << endl;
            return 0;
        }
    }

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "Socket error!" << endl;
        return 1;
    }

    // Позволяет повторно использовать порт сразу после перезапуска сервера
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Bind error!" << endl;
        close(serverSocket);
        return 2;
    }

    listen(serverSocket, 3);
    cout << "Server started on port " << PORT << " :)" << endl;

    sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr); // ИСПРАВЛЕНО
    int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    
    if (clientSocket < 0) {
        cerr << "Accept error!" << endl;
        close(serverSocket);
        return 4;
    }

    cout << "Client connected!" << endl;

    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) break;

        string message(buffer);
        message.erase(remove(message.begin(), message.end(), '\n'), message.end());
        message.erase(remove(message.begin(), message.end(), '\r'), message.end());

        string response;
        if (message == "spiral") {
            response = generate_spiral();
            cout << "--- Spiral generated and sent to client ---" << endl;
        } else if (message == "ping") {
            response = "pong\n";
        } else if (message == "exit") {
            break;
        } else {
            response = "Unknown command. Try 'spiral', 'ping' or 'exit'\n";
        }

        

        send(clientSocket, response.c_str(), response.length(), 0);
    }

    close(clientSocket);
    close(serverSocket);
    return 0;
}
