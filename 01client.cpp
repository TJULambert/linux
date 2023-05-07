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

int main(void){

    int cfd = socket(AF_INET,SOCK_STREAM,0);
    
    struct sockaddr_in servaddr,clientaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    inet_aton("127.0.0.1",&servaddr.sin_addr);
    connect(cfd,(struct sockaddr*)&servaddr,sizeof servaddr);
    char buf[1024];
    memset(buf,0,sizeof buf);
    while(1){
       int ret = read(STDIN_FILENO,buf,sizeof buf); 
       printf("ret:%d sizeof bu:%lu\n",ret,sizeof buf);
       write(cfd,buf,ret);
    }
    
    return 0;
}