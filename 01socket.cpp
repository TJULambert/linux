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

    int sfd = socket(AF_INET,SOCK_STREAM,0);
    
    struct sockaddr_in servaddr,clientaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_aton("127.0.0.1",&(servaddr.sin_addr));
    bind(sfd,(struct sockaddr*)&servaddr,sizeof servaddr);
    listen(sfd,5);

    socklen_t len = sizeof clientaddr;

    int cfd = accept(sfd,(struct sockaddr*)&clientaddr,&len);
    
    char buf[1024];
    int ret = 0;
    while(1){
        if(cfd>0){
            ret = read(cfd,buf,sizeof buf);
            if(ret == 0){
                printf("client exit...");
                break;
            }
            write(STDOUT_FILENO,buf,ret);
        }
    }
    
    //直接在屏幕上打印
    
    return 0;
}