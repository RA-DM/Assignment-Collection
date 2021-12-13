#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <stdlib.h>
#include<sys/types.h>
#include <sys/wait.h>
#include<sys/stat.h>
#include<arpa/inet.h>
#include <fcntl.h>
#include <vector>
#include <sstream>
 
#define MAXLINE 10240
#define SERVER_PORT 3600

using namespace std;

vector<string> commandVector;


void dealWithCommand(char* buffer){
    vector<string>().swap(commandVector);
    string tempString=buffer;
    istringstream tempStream(tempString);
    while(tempStream>>tempString){
        commandVector.push_back(tempString);
    }
}

void handleFunc(int accpetfd, char* buffer){
    if(commandVector[0]=="ls"){
        FILE* file;
        file=popen("ls","r");
        if(file){
            fread(buffer,1,1024,file);
            send(accpetfd,buffer,MAXLINE,0);
            pclose(file);
        }
        else{
            strcpy(buffer, "Error: Can not exec ls");
            send(accpetfd,buffer,MAXLINE,0);
            cerr<<"Can not exec ls"<<endl;
        }
        return;
    }
    if(commandVector[0]=="cd"){
        try
        {
            if(chdir(commandVector[1].c_str())==0){
                snprintf(buffer,MAXLINE,"%s","1");
                send(accpetfd, buffer, MAXLINE, 0);
            }
            else{
                strcpy(buffer, "Error: Invalid path");
                send(accpetfd,buffer,MAXLINE,0);
                cerr << "Error: Invalid path" << endl;
            }
        }
        catch(const std::exception& e)
        {
            strcpy(buffer, e.what());
            send(accpetfd,buffer,MAXLINE,0);
            cerr << e.what() << '\n';
        }
        return;
        
    }
    if(commandVector[0]=="put"){
        //create directory
        FILE* file;
        try
        {
            if(access("./ReceivedFiles",F_OK)!=0){
                file=popen("mkdir ./ReceivedFiles","r");
                pclose(file);
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        //write files
        try
        {
            string tempString=commandVector[1];
            int i=tempString.size()-1;
            for( ;i!=-1 && tempString[i]!='/';i--){
                continue;
            }
            snprintf(buffer,MAXLINE,"%s%s","./ReceivedFiles/",tempString.substr(i+1).c_str());
            int openfd=open(buffer,O_RDWR|O_CREAT,0660);
            write(openfd,(void*)commandVector[2].c_str(),strlen(commandVector[2].c_str()));
            close(openfd);
            memset(buffer,0,MAXLINE);
            snprintf(buffer,MAXLINE,"%s","1");
            send(accpetfd, buffer, MAXLINE, 0);
        }
        catch(const std::exception& e)
        {
            strcpy(buffer, "Error: File can not writes");
            send(accpetfd,buffer,MAXLINE,0);
            cerr << e.what() << '\n';
        }
        return;
        
    }
    if(commandVector[0]=="get"){
        if(access(commandVector[1].c_str(),F_OK)==0){
            int openfd=open(commandVector[1].c_str(),O_RDWR);
            int size=lseek(openfd,0,SEEK_END);
            lseek(openfd,0,SEEK_SET);
            read(openfd,buffer,size*sizeof(char));
            send(accpetfd,buffer,MAXLINE,0);
            close(openfd);
        }
        else{
            strcpy(buffer, "Error: No such a file");
            send(accpetfd,buffer,MAXLINE,0);
            cerr<<"Error: No such a file"<<endl;
        }
        return;
    }
    if(commandVector[0]=="del"){
        if(access(commandVector[1].c_str(),F_OK)!=0){
            strcpy(buffer, "Error: File may not exist");
            send(accpetfd,buffer,MAXLINE,0);
            cerr<<"Error: File may not exist"<<endl;
            return;
        }
        FILE* cmd;
        snprintf(buffer,MAXLINE,"%s%s","rm ",commandVector[1].c_str());
        try
        {
            cmd=popen(buffer,"r");
            if(access(commandVector[1].c_str(),F_OK)!=0){
                strcpy(buffer,"Error: File del failed");
                cerr<<"Error: File del failed"<<endl;
                send(accpetfd, buffer, MAXLINE, 0);
            }
            else if(cmd){
                snprintf(buffer,MAXLINE,"%s","1");
                send(accpetfd, buffer, MAXLINE, 0);
            }
            pclose(cmd);
        }
        catch(const std::exception& e)
        {
            memset(buffer,0,MAXLINE);
            strcpy(buffer, e.what());
            send(accpetfd,buffer,MAXLINE,0);
            cerr<<e.what()<<endl;
        }
        return;
    }
}

int main()
{
    int listenfd,accpetfd;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    char buffer[MAXLINE] = {};
    int len;
 
    if ((listenfd = socket(PF_INET,SOCK_STREAM,0))==-1)
    {
        cout<<"Create socket failed!"<<endl;
        return 0;    
    }
    else
    {
       cout<<"Create socket success!"<<endl;
    }
    
    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERVER_PORT);
 
    if (bind(listenfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr))==-1)
    {
        cout<<"Bind socket failed!"<<endl;
        return 0;
    }
    else
    {
       cout<<"Bind socket success!"<<endl;
    }
    
    if (listen(listenfd,10)==-1)
    {
        cout<<"Listen socket failed!"<<endl;
        return 0;
    }
    else
    {
       cout<<"Listen socket success!"<<endl;
       cout<<"Server start..."<<endl;
    }
    while (true)
    {
        socklen_t clilen = sizeof(clientAddr);
        if ((accpetfd = accept(listenfd,(struct sockaddr*)&clientAddr,&clilen))==-1)
        {
            cout<<"Accept socket failed!"<<endl;
            continue;
        }
        int pid=fork();
        if(pid==0){
            close(listenfd);
            strcpy(buffer, "You have connected to the beta_ftp server");
            send(accpetfd,buffer,MAXLINE,0);
            memset(buffer,0,MAXLINE);
            cout<<"Accept socket success from: "<<inet_ntoa(clientAddr.sin_addr)<<endl;
            while ((len = recv(accpetfd,buffer,MAXLINE,0)>0))
            {
                if (strcmp(buffer,"quit")==0)
                {
                    strcpy(buffer, "Quited Successfully");
                    send(accpetfd,buffer,MAXLINE,0);
                    memset(buffer,0,MAXLINE);
                    break;
                }
                dealWithCommand(buffer);
                memset(buffer,0,MAXLINE);
                handleFunc(accpetfd,buffer);
                memset(buffer,0,MAXLINE);
            }
            close(accpetfd);
            exit(0);
        }
        else{
            close(accpetfd);
        }
    }
    return 0;
}
