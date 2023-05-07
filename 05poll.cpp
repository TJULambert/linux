#include<poll.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<assert.h>
/**poll机制
 * 传入参数不是集合，改为数组
 * 
*/

int main(void){
    
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in servaddr;
    servaddr.sin_port = htons(8888);
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //绑定前设置端口服用
    int opt = 1;
    setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    bind(sfd,(struct sockaddr*)&servaddr,sizeof servaddr);
    listen(sfd,5);
    struct pollfd pfds[1024];
    pfds[0].events = POLLIN;
    pfds[0].fd = sfd;
    pfds[0].revents = 0;//传入为0表示，等待poll传出参数
    int max = 0;
    for(int i = 0;i<1024;i++){
        pfds[i].fd = -1;
    }
    while(1){
        int n = poll(pfds,max+1,-1);
        //如果sfd变化，提取cfd
        if(pfds[0].revents&POLLIN){
            struct sockaddr_in cliaddr;
            socklen_t len = sizeof cliaddr;
            int cfd = accept(sfd,(struct sockaddr*)&cliaddr,&len);
            printf("client connecting...");
            
            for(int i = 1;i<1024;i++){
                if(pfds[i].fd<0){
                    pfds[i].fd = cfd;
                    pfds[i].events = POLLIN; 
                }
            }
        }
        //处理cfd
        //for()
        
    }

    return 0;
}



