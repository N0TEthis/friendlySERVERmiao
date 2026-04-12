#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

const char* IP = "127.0.0.1";
const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {

    // создаём TCP сокет
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // структура, где храним адрес сервера
    sockaddr_in server{};

    
    server.sin_family = AF_INET;

    // порт (переводим в сетевой формат)
    server.sin_port = htons(PORT);

    // перевод IP строки в бинарный формат
    inet_pton(AF_INET, IP, &server.sin_addr);

    // подключаемся к серверу
    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        cout << "Connect error\n";
        return 1;
    }

    cout << "Connected!\n";

    // бесконечный цикл общения с сервером
    while (true) {

        // ввод команды от пользователя
        cout << "> ";
        string msg;
        getline(cin, msg);

        // если пусто — пропускаем
        if (msg.empty()) continue;

        // добавляем перевод строки (серверу так проще читать)
        msg += "\n";

        // отправляем сообщение серверу
        send(sock, msg.c_str(), msg.size(), 0);

        // если команда exit — выходим
        if (msg == "exit\n") break;

        // буфер для ответа сервера
        string response;
        char buffer[BUFFER_SIZE];

        // читаем ответ 
        while (true) {

            // очищаем буфер
            memset(buffer, 0, BUFFER_SIZE);

            // получаем данные от сервера
            int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);

            // если сервер отключился
            if (bytes <= 0) break;

            // добавляем часть ответа
            response += buffer;

            // проверяем конец сообщения
            if (response.find("END\n") != string::npos)
                break;
        }

        // убираем служебный маркер END
        size_t pos = response.find("END\n");
        if (pos != string::npos)
            response.erase(pos);

        // выводим ответ сервера
        cout << response << endl;
    }

    // закрываем соединение
    close(sock);
}