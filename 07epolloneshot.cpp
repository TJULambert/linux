#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<assert.h>
#include<unistd.h>
#include<errno.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<pthread.h>
#include<string.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024

struct fds{
    int epollfd;
    int sockfd;
};

int setnonblocking(int fd){
    int old_option=fcntl(fd,F_GETFL);
    int new_option=old_option|O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}

//将fd注册到epollfd上
void addfd(int epollfd,int fd,bool oneshot){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN|EPOLLET;
    if(oneshot){
        event.events|=EPOLLONESHOT;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);

    setnonblocking(fd);

}

//保证后续可以触发事件
void reset_oneshot(int epollfd,int fd){
    epoll_event event;
    event.data.fd  = fd;
    event.events = EPOLLIN|EPOLLET|EPOLLONESHOT;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);
}

void* worker(void*arg){
    int cfd = ((fds*)arg)->sockfd;
    int epollfd = ((fds*)arg)->epollfd;
    printf("a new thread to get data... %d\n ",cfd);
    char buf[BUFFER_SIZE];
    memset(buf,0,sizeof buf);
    while(1){
        int ret = read(cfd,buf,sizeof buf);
        if(ret == 0){
            close(cfd);
            printf("client close the connection...\n");
            break;
        }
        else if(ret<0){
            if(errno == EAGAIN){
                reset_oneshot(epollfd,cfd);
                printf("read finish...");
                break;
            }
        }else{
            printf("get content:%s\n",buf);
            sleep(5);
            //处理数据过程
        }
        
    }
    printf("%d  end...",cfd);

}

int main(void){
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sfd,(struct sockaddr*)&servaddr,sizeof servaddr);
    listen(sfd,5);
    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    addfd(epollfd,sfd,false);   
    //开始监听事件
    while(1){
        int ret = epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);
        assert(ret!=-1);
        for(int i = 0;i<ret;i++){
            int sockfd = events[i].data.fd;
            if(sockfd == sfd){
                //提取一个连接
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof clientaddr;
                int cfd = accept(sfd,(struct sockaddr*)&clientaddr,&len);
                
                addfd(epollfd,cfd,true);

            }else if(events[i].events&EPOLLIN){
                pthread_t thread;
                struct fds fds_for_new_worker;
                fds_for_new_worker.epollfd =epollfd;
                fds_for_new_worker.sockfd = events[i].data.fd;
                pthread_create(&thread,nullptr,worker,(void*)&fds_for_new_worker);
                pthread_detach(thread);
            }else{
                printf("something slse happened...\n");
            }
        }
    }
    close(sfd);
    return 0;
}








