#include "headers.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <map>
#include <string>
#include <vector>
#include <typeinfo>
using namespace std;

#define HOSTNAME "127.0.0.1"
#define SERVER_B_UDP_PORT 22786
#define MAIN_SERVER_UDP_PORT 23786

#define BUFFER_SIZE 1024

string user_list;

map<string, string> get_user_info() {
    map<string, string> m;
    fstream file;
    file.open("./b.txt");

    if (file.is_open()) {
        string s;
        while(getline(file, s)) {
            stringstream ss(s);
            string t;
            string username;
            string avail_list;
            while(getline(ss, t, ';')) {
                if (isalpha(t[0])) {
                    username = t;
                }
                else {
                    avail_list = t;
                    m[username] = avail_list;
                }
            }
        }
    }

    file.close();
    return m;
}

string get_user_list(map<string, string> m) {
    string res;
    for(map<string, string>::iterator it = m.begin();
        it != m.end(); ++it) {
            res += it->first + " ";
        }
    string ret = res.substr(0, res.size() - 1);
    return ret;
}

void create_UDP_socket_and_send_message(const int port, string msg) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Server A: failed to create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(HOSTNAME);

    int len = sendto(sockfd, (const char *) msg.c_str(), strlen(msg.c_str()),
    0, (const struct sockaddr*) &servaddr, sizeof(servaddr));

    if (len == -1) {
        perror ("Server A: failed to send message");
        exit(EXIT_FAILURE);
    }
    close(sockfd);
}

void create_UDP_socket_and_receive_message(const int port) {
    char buffer[BUFFER_SIZE];
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Server A: failed to create socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
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
    serv_addr.sin_port = htons(SERVER_B_UDP_PORT);
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

map<string, vector<vector<int > > > extract_availabilities(map<string,string> user_map, const string& requested_users) {
    vector<vector<int > > v;
    map<string, vector<vector<int > > > m;
    stringstream ss(requested_users);
    string name;
    while (ss >> name) {
        if (user_map.find(name) != user_map.end()) {
            string avail = user_map[name];
            m[name] = extract(avail);
        }
    }
    return m;
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

vector<vector<int> > find_mutual_availability(map<string, vector<vector<int> > > user_map) {
    vector<vector<int> > res;
    if (user_map.empty()) {
        vector<int> v;
        res.push_back(v);
        return res;
    }
    map<string, vector<vector<int> > >::iterator it = user_map.begin();
    if (user_map.size() == 1) {
        return it->second;
    }

    vector<vector<int> > a = it->second;
    ++it;
    vector<vector<int> > b = it->second;
    res = find_intersection(a, b);
    ++it;

    for (; it != user_map.end(); ++it) {
        res = find_intersection(res, it->second);
    }
    return res;
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

    printf("Server B is up and running using UDP on port %d.\n", SERVER_B_UDP_PORT);

    // create_UDP_socket_and_send_message(MAIN_SERVER_UDP_PORT, "Hello from Server B");
    map<string, string> m = get_user_info();
    string users = get_user_list(m);
    create_UDP_socket_and_send_message(MAIN_SERVER_UDP_PORT, users);
    printf("Server B finished sending a list of usernames to Main Server.\n");
    // create_UDP_socket_and_receive_message(SERVER_B_UDP_PORT);
    string requested_users = create_UDP_socket_and_receive_list();
    cout << requested_users << endl;
    printf("Server B received the usernames from Main Server using UDP over port %d.\n", SERVER_B_UDP_PORT);

    map<string, vector<vector<int> > > user_map = extract_availabilities(m, requested_users);
    vector<vector<int> > res = find_mutual_availability(user_map);
    string result = stringify(res);
    cout << "Found the intersection result: " << result << " for " << requested_users << endl;
    create_UDP_socket_and_send_message(MAIN_SERVER_UDP_PORT, result);
    printf("Server B finished sending the response to Main Server.\n");

    return 0;
}