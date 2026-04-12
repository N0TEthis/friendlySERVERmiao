#include <iostream>     
#include <vector>       
#include <string>       
#include <cmath>        
#include <cstring>      
#include <algorithm>    
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <unistd.h>     
#include <sstream>      

using namespace std;

// информация о программе
const string AUTHOR = "Miao";     // автор
const string VERSION = "1.2.3.";  // версия
const int PORT = 8080;            // порт сервера
const int BUFFER_SIZE = 1024;     // размер буфера

//  проверка числа на простое
bool is_prime(int n) {
    if (n < 2) return false;          // числа < 2 не простые
    if (n % 2 == 0 && n != 2) return false; // чётные (кроме 2) не простые

    // проверяем делители до sqrt(n)
    for (int i = 3; i <= sqrt(n); i += 2)
        if (n % i == 0) return false;

    return true;
}

//  генерация спирали
string generate_spiral() {
    const int size = 30; // размер поля
    vector<vector<char>> grid(size, vector<char>(size, ' ')); // 2D поле

    int x = size / 2, y = size / 2; // старт из центра
    int step = 1, count = 0, dir = 0;

    // направления движения: вправо, вверх, влево, вниз
    int dx[] = {1, 0, -1, 0};
    int dy[] = {0, -1, 0, 1};

    // заполняем поле
    for (int n = 1; n <= size * size; n++) {

        // если внутри границ — записываем символ
        if (x >= 0 && x < size && y >= 0 && y < size)
            grid[y][x] = is_prime(n) ? '*' : ' ';

        // двигаемся
        x += dx[dir];
        y += dy[dir];
        count++;

        // смена направления
        if (count == step) {
            count = 0;
            dir = (dir + 1) % 4;

            // увеличиваем шаг каждые 2 поворота
            if (dir % 2 == 0) step++;
        }
    }

    // превращаем поле в строку
    stringstream ss;
    for (auto& row : grid) {
        for (char c : row) ss << c << ' ';
        ss << '\n';
    }

    return ss.str();
}

// обработка команд клиента
string handle_command(const string& msg) {

    if (msg == "-h") {
        return "help u :)\n Commands:\n-h help\n-v version\n-a author\n-s spiral\nexit\n";
    }
    else if (msg == "-v") {
        return "Version: " + VERSION + "\n";
    }
    else if (msg == "-a") {
        return "Author: " + AUTHOR + "\n";
    }
    else if (msg == "-s") {
        return generate_spiral(); // возвращаем спираль
    }
    else if (msg == "ping") {
        return "pong\n";
    }
    else {
        return "Unknown command (-h for help)\n";
    }
}

//  основной сервер
int main() {

    // создаём TCP сокет
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // разрешаем повторное использование порта
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // структура адреса сервера
    sockaddr_in addr{};
    addr.sin_family = AF_INET;        
    addr.sin_port = htons(PORT);      
    addr.sin_addr.s_addr = INADDR_ANY; 

    // привязка сокета к адресу
    bind(serverSocket, (sockaddr*)&addr, sizeof(addr));

    // перевод в режим ожидания подключений
    listen(serverSocket, 5);

    cout << "Server started on port " << PORT << endl;

    // бесконечный цикл сервера
    while (true) {

        sockaddr_in clientAddr{};
        socklen_t size = sizeof(clientAddr);

        // принимаем клиента
        int client = accept(serverSocket, (sockaddr*)&clientAddr, &size);
        cout << "Client connected\n";

        char buffer[BUFFER_SIZE];

        // цикл общения с клиентом
        while (true) {
            memset(buffer, 0, BUFFER_SIZE); // очистка буфера

            // получаем данные
            int bytes = recv(client, buffer, BUFFER_SIZE - 1, 0);

            if (bytes <= 0) break; // клиент отключился

            string msg(buffer);

            // удаляем переносы строк
            msg.erase(remove(msg.begin(), msg.end(), '\n'), msg.end());
            msg.erase(remove(msg.begin(), msg.end(), '\r'), msg.end());

            if (msg == "exit") break; // выход клиента

            // обработка команды
            string response = handle_command(msg);

            response += "END\n"; //  маркер конца сообщения

            // отправка ответа клиенту
            send(client, response.c_str(), response.size(), 0);
        }

        // закрываем соединение с клиентом
        close(client);
        cout << "Client disconnected\n";
    }

    // закрываем сервер
    close(serverSocket);
}