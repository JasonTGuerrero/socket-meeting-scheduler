#include "headers.h"
#include <iostream>
#include <sstream>
#include <set>
#include <algorithm>
#include <cctype>
using namespace std;

#define MAIN_SERVER_TCP_PORT 24786
#define HOSTNAME "127.0.0.1"

#define BUFFER_SIZE 1024

int client_sockfd;
struct sockaddr_in serv_addr;
char buffer[1024];

void create_TCP_socket_and_connect() {
    client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sockfd == -1) {
        perror("Client: socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(MAIN_SERVER_TCP_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(HOSTNAME);

    if (connect(client_sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Client: socket connection failed");
        exit(EXIT_FAILURE);
    }
}

set<string> get_names(string client_input) {
    set<string> s;
    stringstream ss(client_input);
    string cur;
    while (ss >> cur) {
        s.insert(cur);
    }
    return s;
}


int main(int argc, char *argv[]) {
    printf("Client is up and running.\n");
    cout << "Please enter the usernames to check schedule availability:" << endl;
    string client_input;
    set<string> name_set = get_names(client_input);
    getline(cin, client_input); 

    create_TCP_socket_and_connect();
    send(client_sockfd, client_input.c_str(), strlen(client_input.c_str()), 0);
    // read(client_sockfd, buffer, BUFFER_SIZE);

    printf("Client received the reply from Main Server using TCP over port 24786:\n");

    memset(&buffer, 0, sizeof buffer);
    int n = recv(client_sockfd, buffer, BUFFER_SIZE, 0);
    buffer[n] = '\0';
    string dne = string(buffer);
    printf("%sdoes not exist\n", dne.c_str());
    // printf("%s\n", buffer);

    printf("Client received the reply from Main Server using TCP over port 24786:\n");


    memset(&buffer, 0, sizeof buffer);
    int n1 = recv(client_sockfd, buffer, BUFFER_SIZE, 0);
    buffer[n1] = '\0';
    string final_result = string(buffer);
    printf("Time intervals %s works for %s\n", final_result.c_str(), client_input.c_str());
    // printf("%s\n", buffer);
    

    close(client_sockfd);
    return 0;
}