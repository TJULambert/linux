#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/wait.h>
#include<signal.h>
#include<pthread.h>

//
void* func(void* arg){
    int *cfd = (int*)arg;
    char buf[1024];
    while(1){
        int n = read(*cfd,buf,sizeof buf);
        if(n == 0){
            close(*cfd);
            break;
        }
        write(STDOUT_FILENO,buf,n);
    }
    return nullptr;
}
int main(void){
    //1.先创建sfd
    pthread_t pthid[5];
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    //2.绑定监听等等
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
    bind(sfd,(struct sockaddr*)&servaddr,sizeof servaddr);
    listen(sfd,5);
    int i = 0;
    int cfds[5];
    while(1){
        struct sockaddr_in clientaddr;
        socklen_t len = sizeof clientaddr;
        cfds[i] = accept(sfd,(struct sockaddr*)&clientaddr,&len);
        pthread_create(&pthid[i],nullptr,func,(void*)&cfds[i]);
        pthread_detach(pthid[i]);
        i++;
    }

    return 0;
}