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
    return 0;
}
