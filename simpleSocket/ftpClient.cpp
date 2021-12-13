#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <string>
#include<sstream>
#include <vector>
#include <map>


#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;


#define LS 0
#define GET 2
#define PUT 3
#define CD 4
#define DEL 5
#define HELP 6
#define QUIT 7
#define OUT 8



#define SERVER_PORT 3600
#define MAX_LENGTH 10240

char buf[MAX_LENGTH] = {};


string get_input(){
    string s;
    getline(cin, s);
    return s;
}
vector<string> split(string word){
    vector<string> res;
    string result;
    stringstream input(word);
    while(input>>result)
        res.push_back(result);
    return res;
}
int validate(vector<string> v){
    if (v.size() > 0){
        if (v[0] == "ls"){
            if(v.size() == 1)
                return LS;
            else return -1;
        }
        else if (v[0] == "get"){
            if(v.size() == 2)
                return GET;
            else return -1;
        }
        else if (v[0] == "put"){
            if(v.size() == 2)
                return PUT;
            else return -1;
        }
        else if (v[0] == "cd"){
            if(v.size() == 2)
                return CD;
            else return -1;
        }
        else if (v[0] == "del"){
            if(v.size() == 2)
                return DEL;
            else return -1;
        }
        else if (v[0] == "help"){
            if(v.size() == 1)
                return HELP;
            else return -1;
        }
        else if (v[0] == "quit"){
            if(v.size() == 1)
                return QUIT;
            else return -1;
        }
        return -2;
    }
    else{
        return -3;
    }
}

void error_handler(int num){
    if(num == -3)
        cout << "Please enter commands!" << endl;
    else if (num == -2)
        cout << "Command does not exist!" << endl;
    else if (num == -1)
        cout << "The number of arguments is wrong!" << endl;
}

void communicate(int client, char* input, vector<string> cmd, int num){
    if (num == PUT){
        try{
            memset(buf, 0, sizeof(buf));
            if(access(cmd[1].c_str(), F_OK)==0){
                int openfd = open(cmd[1].c_str(), O_RDWR);
                int size = lseek(openfd, 0, SEEK_END);
                lseek(openfd, 0, SEEK_SET);
                read(openfd, buf, size * sizeof(char));

                string st1 = buf;
                string st2 = input;
                string all = st2 + " " + st1;
                char *strmsg = (char*)all.c_str();

                send(client, strmsg, MAX_LENGTH, 0);
                close(openfd);
            }
            else{
                cout << "Error: No such a file" << endl;
                return;
            }
        }
        catch(const std::exception& e){
            cout << "Error" << endl;
            cerr << e.what() << '\n';
        }
    }
    else
        send(client, input, strlen(input), 0);
    
    if(num == GET){
        while(1){
            memset(buf, 0, sizeof(buf));
            int len = recv(client, buf, sizeof(buf), 0);
            char* bufp = buf;
            if(len > 0){
                string bufs = buf;
                if (strcmp(split(bufs)[0].c_str(), "Error:") == 0)
                    cout << bufs << endl;
                else{
                    FILE* file;
                    try{
                        if(access("./ReceivedFiles",F_OK)!=0){
                            file=popen("mkdir ./ReceivedFiles","r");
                            pclose(file);
                        }
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }

                    try{
                        string tempString=input;
                        int i=tempString.size()-1;
                        for( ;tempString[i]!=' ' && tempString[i]!='/';i--){
                            continue;
                        }
                        char tempPath[1024];
                        snprintf(tempPath,sizeof(tempPath),"%s%s","./ReceivedFiles/",tempString.substr(i+1).c_str());
                        int openfd = open(tempPath, O_RDWR | O_CREAT, 0660);
                        write(openfd, (void*)bufp, strlen(bufp));
                        memset(buf, 0, sizeof(buf));
                        close(openfd);
                    }
                    catch(const std::exception& e){
                        cout << "Error: File can not writes" << endl;
                        cerr << e.what() << '\n';
                    }
                }
                
                break;
            }
        }
    }
    else{
        memset(buf, 0, sizeof(buf));
        int len = recv(client, buf, sizeof(buf), 0);
        string bufs = buf;
        //cout << "rtn: " << bufs << endl;
        if (strcmp(split(bufs)[0].c_str(), "1") != 0)
            cout << bufs << endl;
        //cout << buf << endl;
    }
}
void help(){}
int main() {
    cout << "This is client" << endl;
    // socket
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1) {
        cout << "Error: socket" << endl;
        return 0;
    }
    //connect
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    // serverAddr.sin_port = htons(8000);
    // serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERVER_PORT);
    
    if (connect(client, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cout << "Error: connect" << endl;
        return 0;
    }
    cout << "...connect" << endl;
    

    memset(buf, 0, sizeof(buf));
    int len = recv(client, buf, sizeof(buf), 0);
    buf[len] = '\0';
    cout << buf << endl;
    cout << "# ";
    while (true) {


        string input = get_input();
        vector<string> cmd = split(input);
        int num = validate(cmd);
        if(num < 0)
            error_handler(num);
        else if (num == 6) help();
        else if (num == 7){
            send(client, "quit", MAX_LENGTH, 0);
            memset(buf, 0, sizeof(buf));
            int len = recv(client, buf, sizeof(buf), 0);
            cout << buf << endl;
            break;
        }
        else
            communicate(client, (char*)input.c_str(), cmd, num);
        cout << "# ";
    }
    close(client);
    return 0;
}