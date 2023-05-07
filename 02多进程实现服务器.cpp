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
void sig_catch(int sig){
    pid_t pid;
    while((pid = waitpid(0,nullptr,WNOHANG))!=-1){
        printf("recycling...%d",getpid());
    }
}

int main(void){
    //1.先创建sfd
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    //2.绑定监听等等
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
    bind(sfd,(struct sockaddr*)&servaddr,sizeof servaddr);
    listen(sfd,5);
    //3.fork一些子进程
    while(1){
        struct sockaddr_in clientaddr;
        socklen_t len = sizeof clientaddr;
        int cfd = accept(sfd,(struct sockaddr*)&clientaddr,&len);
        if(cfd<0){
            perror("accept error");
            exit(1);
        }
        pid_t pid= fork();
        //父进程负责监听sfd,子进程负责处理
        assert(pid!=-1);
        if(pid==0){
            while(1){
                char buf[1024];
                close(sfd);
                int ret = read(cfd,buf,sizeof buf);
                if(ret == 0){
                    printf("client is exit...\n");
                    close(cfd);
                    exit(1);
                }
                //子进程负责读，且打印在
                printf("%d is reading...\n",getpid());
                write(STDOUT_FILENO,buf,ret);
            }
        }else if(pid>0){
            struct sigaction sigact;
            sigact.sa_flags = 0;
            sigemptyset(&sigact.sa_mask);
            sigact.sa_handler = sig_catch;
            sigaction(SIGCHLD,&sigact,nullptr);
            close(cfd);
            //信号回收子进程
            continue;
        }
    }

    //4.子进程accept
    //5.子进程进行读取写操作
    return 0;

}