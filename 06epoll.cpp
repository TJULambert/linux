#include<poll.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<assert.h>
#include<sys/epoll.h>
/**
 * epoll优势：返回的就是准备好的事件数组，不需要全部遍历
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
    
    //epoll逻辑
    // epoll_event内 events,data
    struct epoll_event tep;
    int efd = epoll_create(0);

    epoll_ctl(efd,EPOLL_CTL_ADD,sfd,&tep);

    return 0;
}



