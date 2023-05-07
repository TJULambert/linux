#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<assert.h>

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
    fd_set newset,oldset;
    int maxfd = sfd;
    FD_ZERO(&oldset);
    FD_SET(sfd,&oldset);
    while(1){
        newset = oldset;
        /*参数：最大文件描述符+1,读，写，异常集合，null表示阻塞监听*/
        int n =  select(maxfd+1,&newset,nullptr,nullptr,nullptr);
        if(n<0){
            perror("select error");
        }
        //如果sfd在返回集合中，需要提取一个新的连接
        if(FD_ISSET(sfd,&newset)){
            struct sockaddr_in cliaddr;
            socklen_t len = sizeof cliaddr;
            int cfd = accept(sfd,(struct sockaddr*)&cliaddr,&len);
            assert(cfd!=-1);
            //修改参数
            FD_SET(cfd,&oldset);
            if(cfd>maxfd){
                maxfd = cfd;
            }
            //只有socket事件，只监听，本质就是优化了一下代码
            if(--n){
                continue;
            }
        }
        for(int i = sfd+1;i<=maxfd;i++){
            //有数据
            if(FD_ISSET(i,&oldset)){
                char buf[1024];
                int num;
                if((num = read(i,buf,sizeof buf)) == 0){
                    printf("client exit...\n");
                    close(i);
                    FD_CLR(i,&oldset);
                }
                write(STDOUT_FILENO,buf,num);
            }
        }
    }
    return 0;
}