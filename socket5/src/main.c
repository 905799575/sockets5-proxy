#include "../include/main.h"
#include "../include/common.h"
int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage:%s <server-port>\n", argv[0]);
        exit(1);
    }
    struct sockaddr_in serv_addr = {};
    int serv_sock = Socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //inet_pton(AF_INET, "172.16.2.21", &serv_addr.sin_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    int bind_res = Bind(serv_sock, (struct sockaddr* )&serv_addr, sizeof(serv_addr));
    if(bind_res < 0)
        exit(1);
    Listen(serv_sock, BACK_LOG);

    struct sockaddr_in client_addr = {};
    int client_len = sizeof(client_addr);
    
    signal(SIGCHLD, sigchild_handle);
    for(;;)
    {
        int client_sock = accept(serv_sock, (struct sockaddr*)&client_addr, &client_len);
        if(client_sock == -1)
        {
            printf("accept error");
            close(client_sock);
            continue;
        }
        if(fork() == 0) //子进程
        {
            close(serv_sock);
            int remote_fd = handle_client_connect(client_sock);
            if((forward_data(client_sock, remote_fd)) < 0)
            {
                printf("错误2\n");
                exit(2);
            }
            exit(0);
        }
        close(client_sock);
    }


    // //设置监听套接字为非阻塞
    // Setnoblocking(serv_sock);
    // //设置地址重用
    // if(Setreuseaddr(serv_sock))
    // {
    //     return 1;
    // }
    // //创建epoll红黑树
    // int epoll_fd = Epoll_create(EPOLL_SIZE);
    // struct epoll_event event = {};
    // //水平触发的监听套接字
    // event.events = EPOLLIN;
    // event.data.fd = serv_sock;
    // epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serv_sock, &event);

    // int event_count = 0;
    // struct epoll_event *occur_events = (struct epoll_event*)malloc(sizeof(struct epoll_event));
    // struct sockaddr_in client_addr = {};
    // int client_sock = 0;
    // socklen_t client_len = 0;
    // pthread_t thread_id = 0;
    // for( ; ; )
    // {
    //     printf("epoll_wait ...\n");
    //     //阻塞至文件描述符发生变化
    //     event_count = Epoll_wait(epoll_fd, occur_events, EPOLL_SIZE, -1);
    //     printf("epoll_wait success...\n");
    //     int i = 0;
    //     for(; i < event_count; i++)
    //     {
    //         //新的客户端连接
    //         if(occur_events[i].data.fd == serv_sock)
    //         {
    //             client_len = sizeof(client_addr);
    //             client_sock = accept(serv_sock, (struct sockaddr*)&client_addr, &client_len);
    //             if(client_sock == -1)
    //             {
    //                 printf("client sock == -1, errno:%d err:%s\n", errno, strerror(errno));
    //                 continue;
    //             }
    //             //Setnoblocking(client_sock);
    //             // event.events = EPOLLIN | EPOLLET; //设置已连接套接字为边缘触发模式
    //             // event.data.fd = serv_sock;
    //             // epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event);
    //             // printf("connected client:%d\n", client_sock);   
    //             // struct epfd_fd arg = {epoll_fd, client_sock};
    //             //pthread_create(&thread_id, NULL, handle_client_connect, &arg); 
    //             printf("before fd:%d\n", client_sock);
    //             pthread_create(&thread_id, NULL, handle_client_connect, (void*)client_sock); 
    //             pthread_join(thread_id,NULL);
    //             printf("wait success\n");
    //         }
    //         else //处理已连接套接字
    //         {
    //             printf("已连接套接字\n");
    //         }
    //     }
    //     printf("main thread 2\n");
    // }
    // close(epoll_fd);
    // close(serv_sock);
    // close(client_sock);
    return 0;
}