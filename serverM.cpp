#include "headers.h"
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

#define HOSTNAME "127.0.0.1"
#define SERVER_A_UDP_PORT 21786
#define SERVER_B_UDP_PORT 22786
#define MAIN_SERVER_UDP_PORT 23786
#define MAIN_SERVER_TCP_PORT 24786
#define BUFFER_SIZE 1024

string dne_list_str;

// server A
int servA_sockfd;
struct sockaddr_in servA_addr;
char servA_recv_buffer[BUFFER_SIZE];
string servA_user_list_str;
string servA_sublist_str;
set<string> servA_user_set;

// server B
int servB_sockfd;
struct sockaddr_in servB_addr;
char servB_recv_buffer[BUFFER_SIZE];
string servB_user_list_str;
string servB_sublist_str;
set<string> servB_user_set;


// client
int client_sockfd;
int new_client_sockfd;
struct sockaddr_in client_addr;
char client_read_buffer[1024];
string msg_to_client = "Hello from Main Server";

set<string> store_users(const string& user_list_str) {
    set<string> name_set;
    stringstream s(user_list_str);
    string name;
    while(s >> name) {
        name_set.insert(name);
    }
    return name_set;
}

void query_backend_servers(const string& names) {
    stringstream s(names);
    string name;
    while (s >> name) {
        if (servA_user_set.find(name) != servA_user_set.end()) {
            servA_sublist_str += name + " ";
        }
        else if (servB_user_set.find(name) != servB_user_set.end()) {
            servB_sublist_str += name + " ";
        }
        else {
            dne_list_str += name + " ";
        }
    }
    // servA_sublist_str = a.substr(0, a.size()-1);
    // servB_sublist_str = b.substr(0, b.size()-1);
    // dne_list_str = dne.substr(0, dne.size()-1);
    // servA_sublist_str = servA_sublist_str.substr(0, servA_sublist_str.size() - 1);
    // servB_sublist_str = servB_sublist_str.substr(0, servB_sublist_str.size() - 1);
    // dne_list_str = dne_list_str.substr(0, dne_list_str.size() - 1);
}

void create_UDP_socket_and_send_message(int port, string msg) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Main Server: failed to create UDP socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(HOSTNAME);

    int len = sendto(sockfd, (const char *) msg.c_str(), strlen(msg.c_str()),
    0, (const struct sockaddr*) &servaddr, sizeof(servaddr));
    if (len == -1) {
        perror("Main Server: failed to send message to Server A");
    }
    close(sockfd);
}

void create_serverA_UDP_socket_and_receive_message(int port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    char buffer[BUFFER_SIZE];

    if (sockfd == -1) {
        perror("Main Server: failed to create UDP socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MAIN_SERVER_UDP_PORT);
    servaddr.sin_addr.s_addr = inet_addr(HOSTNAME);

    int rc = bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    if (rc == -1) {
        perror("Main Server: failed to bind UDP socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    socklen_t len = 0;
    int n = recvfrom(sockfd, (char *) buffer, BUFFER_SIZE, MSG_WAITALL, 0, &len);

    // buffer[sizeof(buffer)] = '\0';
    buffer[n] = '\0';

    if(port == SERVER_A_UDP_PORT) {
        servA_user_list_str = string(buffer);
        servA_user_set = store_users(servA_user_list_str);
    }
    else if (port == SERVER_B_UDP_PORT) {
        servB_user_list_str = string(buffer);
        servB_user_set = store_users(servB_user_list_str);
    }
    close(sockfd);
}

void create_serverB_UDP_socket_and_receive_message(int port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    char buffer[BUFFER_SIZE];

    if (sockfd == -1) {
        perror("Main Server: failed to create UDP socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MAIN_SERVER_UDP_PORT);
    servaddr.sin_addr.s_addr = inet_addr(HOSTNAME);

    int rc = bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    if (rc == -1) {
        perror("Main Server: failed to bind UDP socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    socklen_t len = 0;
    int n = recvfrom(sockfd, (char *) buffer, BUFFER_SIZE, MSG_WAITALL, 0, &len);

    // buffer[sizeof(buffer)] = '\0';
    buffer[n] = '\0';

    if(port == SERVER_A_UDP_PORT) {
        servA_user_list_str = string(buffer);
        servA_user_set = store_users(servA_user_list_str);
    }
    else if (port == SERVER_B_UDP_PORT) {
        servB_user_list_str = string(buffer);
        servB_user_set = store_users(servB_user_list_str);
    }
    close(sockfd);
}


int create_TCP_socket_and_bind() {
    int sockfd, new_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Main Server: failed to create Client TCP socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sockfd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)) == -1) {
        perror("setsockopt");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(HOSTNAME);
    address.sin_port = htons(MAIN_SERVER_TCP_PORT);

    if (bind(sockfd, (struct sockaddr*)&address,
             sizeof(address))
        == -1) {
        perror("Main Server: failed to bind Client TCP socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void create_client_socket() {
    client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sockfd == -1) {
        perror("socket failed");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(MAIN_SERVER_TCP_PORT);
    client_addr.sin_addr.s_addr = inet_addr(HOSTNAME);

    if (bind(client_sockfd, (struct sockaddr*)&client_addr,
             sizeof(client_addr))
        == -1) {
        perror("Main Server: failed to bind client TCP socket");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(client_sockfd, 3) == -1) {
        perror("Main Server: failed to listen on client TCP socket");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }

    int addrlen = sizeof(client_addr);
    new_client_sockfd = accept(client_sockfd, (struct sockaddr*)&client_addr,
                  (socklen_t*)&addrlen);

    if (new_client_sockfd == -1) {
        perror("Main Server: failed to accept connection on client TCP socket");
        exit(EXIT_FAILURE);
    }
}

void create_UDP_socket_and_receive_message() {
    char buffer[BUFFER_SIZE];
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Server A: failed to create socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(MAIN_SERVER_UDP_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(HOSTNAME);

    int rc = bind(sockfd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (rc == -1) {
        perror("Server A: failed to bind UDP socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    socklen_t len = 0;
    int n = recvfrom(sockfd, (char *) buffer, BUFFER_SIZE, MSG_WAITALL, 0, &len);
    buffer[n] = '\0';
    printf("%s\n", buffer);
    close(sockfd);
}

void extractBounds(const string& str, int& num1, int& num2) {
    int i = 0;
    num1 = 0;
    num2 = 0;

    while (i < str.size() && !isdigit(str[i])) {
        i++;
    }

    while (i < str.size() && isdigit(str[i])) {
        num1 = num1 * 10 + (str[i] - '0');
        i++;
    }

    while (i < str.size() && !isdigit(str[i])) {
        i++;
    }

    while (i < str.size() && isdigit(str[i])) {
        num2 = num2 * 10 + (str[i] - '0');
        i++;
    }
}


vector<vector<int> > extract(const string& str) {
    vector<vector<int> > result;
    stringstream ss(str);
    string rowString;

    while (getline(ss, rowString, ']')) {

        if (rowString.empty()) {
            return result;
        }
        vector<int> row;
        stringstream rowSS(rowString);

        int num1, num2;
        extractBounds(rowString, num1, num2);
        row.push_back(num1);
        row.push_back(num2);
        result.push_back(row);
    }
   
    return result;
}

vector<vector<int> > find_intersection(vector<vector<int> > a, vector<vector<int> > b) {
    vector<vector<int> > res;
    int i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        int lower = max(a[i][0], b[j][0]);
        int upper = min(a[i][1], b[j][1]);
        if (lower < upper) {
            vector<int> v;
            v.push_back(lower);
            v.push_back(upper);
            res.push_back(v);
        }

        if (a[i][0] < b[j][0]) {
            i++;
        }
        else {
            j++;
        }
    }
    return res;
}

string create_UDP_socket_and_receive_list() {
    char buffer[BUFFER_SIZE];
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Server A: failed to create socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(MAIN_SERVER_UDP_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(HOSTNAME);

    int rc = bind(sockfd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (rc == -1) {
        perror("Server A: failed to bind UDP socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    socklen_t len = 0;
    int n = recvfrom(sockfd, (char *) buffer, BUFFER_SIZE, MSG_WAITALL, 0, &len);
    // buffer[sizeof(buffer)] = '\0';
    buffer[n] = '\0';
    string main_server_request = string(buffer);
    // cout << main_server_request << endl;
    // printf("%s\n", buffer);
    close(sockfd);
    return main_server_request;
}

string stringify(const vector<vector<int> >& v) {
    stringstream ss;
    ss << "[";
    for (int i = 0; i < v.size(); ++i) {
        if (i > 0) {
            ss << ",";
        }
        ss << "[";
        for (int j = 0; j < v[i].size(); ++j) {
            if (j > 0) {
                ss << ",";
            }
            ss << v[i][j];
        }
        ss << "]";
    }
    ss << "]";
    return ss.str();
}

int main() {
    printf("Main Server is up and running.\n");

    create_serverA_UDP_socket_and_receive_message(SERVER_A_UDP_PORT);
    printf("Main Server received the username list from server A using UDP over port %d.\n", MAIN_SERVER_UDP_PORT);
    // create_UDP_socket_and_send_message(SERVER_A_UDP_PORT, "Hello from Main Server");

    create_serverB_UDP_socket_and_receive_message(SERVER_B_UDP_PORT);
    printf("Main Server received the username list from server B using UDP over port %d.\n", MAIN_SERVER_UDP_PORT);

    // create_UDP_socket_and_send_message(SERVER_B_UDP_PORT, "Hello from Main Server");

    int client_sockfd = create_TCP_socket_and_bind();
    int new_client_sockfd;
    int numbytes;
    char client_buf[BUFFER_SIZE];
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(HOSTNAME);
    address.sin_port = htons(MAIN_SERVER_TCP_PORT);

    if (listen(client_sockfd, 5) == -1) {
        perror("Main Server: failed to listen on client socket");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
    
    new_client_sockfd = accept(client_sockfd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_client_sockfd == -1) {
        perror("Main Server: failed to accept client TCP connection");
        close(new_client_sockfd);
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
    socklen_t len;
    // int n = recvfrom(new_client_sockfd, (char *) client_buf, BUFFER_SIZE, MSG_WAITALL, 0, &len);
    numbytes = read(new_client_sockfd, client_buf, sizeof(client_buf));


    if (numbytes == -1) {
        perror("Main Server: failed to recv on client TCP socket");
        close(client_sockfd);
        close(new_client_sockfd);
        exit(EXIT_FAILURE);
    }

    client_buf[numbytes] = '\0';
    string names(client_buf);

    printf("Main Server received the request from client using TCP over port %d.\n", MAIN_SERVER_TCP_PORT);

    query_backend_servers(names);

    if (!dne_list_str.empty()) {
        cout << dne_list_str + "do not exist. Send a reply to the client." << endl;
        // string msg = dne_list_str + "do not exist.";
        send(new_client_sockfd, (const char*) dne_list_str.c_str(), strlen(dne_list_str.c_str()), 0);
    }
    string intersectionA;
    string intersectionB;
    string final_result;
    string namesToSend;
    if (!servA_sublist_str.empty()) {
        cout << "Found " << servA_sublist_str << "at Server A. Send to Server A." << endl;
        create_UDP_socket_and_send_message(SERVER_A_UDP_PORT, servA_sublist_str);
        intersectionA = create_UDP_socket_and_receive_list();
        printf("Main Server received from Server A the intersection result using UDP over port %d:\n", MAIN_SERVER_UDP_PORT);
        cout << intersectionA << endl;
        final_result = intersectionA;
        namesToSend += servA_sublist_str;
    }

    if (!servB_sublist_str.empty()) {
        cout << "Found " << servB_sublist_str << "at Server B. Send to Server B." << endl;
        create_UDP_socket_and_send_message(SERVER_B_UDP_PORT, servB_sublist_str);
        intersectionB = create_UDP_socket_and_receive_list();
        printf("Main Server received from Server B the intersection result using UDP over port %d:\n", MAIN_SERVER_UDP_PORT);
        cout << intersectionB << endl;
        final_result = intersectionB;
        namesToSend += servB_sublist_str;
    }

    if (!servA_sublist_str.empty() && !servB_sublist_str.empty()) {
        vector<vector<int> > vA = extract(intersectionA);
        vector<vector<int> > vB = extract(intersectionB);
        vector<vector<int> > intersection = find_intersection(vA, vB);
        final_result = stringify(intersection);
        printf("Found the intersection between the results from server A and B:\n");
        cout << final_result << endl;
    }
    

    printf("Main Server sent the result to the client.\n");
    send(new_client_sockfd, (const char*) final_result.c_str(), strlen(final_result.c_str()), 0);

    // send(new_client_sockfd, (const char*) namesToSend.c_str(), strlen(namesToSend.c_str()), 0);



    // create_UDP_socket_and_send_message(SERVER_B_UDP_PORT, servB_sublist_str);
    return 0;
}
