#include "../include/main.h"
#include "../include/common.h"

void sigchild_handle(int signum)
{
    pid_t pid;
    int stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}

int Socket(int domain, int type, int protocol)
{
    int sock = socket(domain, type, protocol);
    if(sock == -1)
    {
        perror("Socket error");
        return -1;
    }
    return sock;
}

int Bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
    int res = bind(socket, address, address_len);
    if(res == -1)
    {
        perror("Bind error");
        return -1;
    }
    return res;
}

int Listen(int socket, int backlog)
{
    int res = listen(socket, backlog);
    if(res == -1)
    {
        perror("Listen error");
        return -1;
    }
    return res;
}

int Epoll_create(int size)
{
    int epoll_fd = epoll_create(size);
    if(epoll_fd == -1)
    {
        perror("Epoll_create error");
        return -1;
    }
    return epoll_fd;
}

int Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
    int res = epoll_ctl(epfd, op, fd, event);
    if(res == -1)
    {
        perror("Epoll_ctl error");
        return -1;
    }
    return res;
}

int Setnoblocking(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    int res = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    return res;
}

int Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
    int event_count = epoll_wait(epfd, events, maxevents, timeout);
    if(event_count == -1)
    {
        perror("Epoll_wait error");
        return -1;
    }

    return event_count;
}

int Setreuseaddr(int fd)
{
    int on = 1;
    int res = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(res == -1)
    {
        perror("setsockopt error");
        return -1;
    }
    return res;
}

int get_client_hello_info(int fd, struct client_hello* client_hello_info)
{
    //printf("sss111\n");
    char buf[1024];
    if(read(fd, buf, 1024) < 0)
    {
        perror("get_client_hello_info read error");
        return -1;
    }
    //printf("sss112\n");
    client_hello_info->ver = buf[0];
    //printf("sss113\n");
    client_hello_info->nmethods = buf[1];
    //printf("sss114\n");
    memmove(client_hello_info->methods, &buf[2], 1);
    //printf("sss115\n");
}

int read_client_connect_info(int fd, struct client_connect_requst* client_connect_requst_info)
{
    uint8_t length = 0;
    unsigned char *tmp_addr = NULL;
    char buff[1024];
    if(read(fd, buff, 1024) < 0)
    {
        perror("read_client_connect_info read error");
        return -1;
    }
    client_connect_requst_info->ver = buff[0];
    client_connect_requst_info->cmd = buff[1];
    client_connect_requst_info->rsv = buff[2];
    client_connect_requst_info->atyp = buff[3];
    //printf("ver:%d  cmd:%d  rsv:%d  atyp:%d\n", client_connect_requst_info->ver, client_connect_requst_info->cmd, client_connect_requst_info->rsv, client_connect_requst_info->atyp);
    switch (client_connect_requst_info->atyp)
    {
        //IPV4
        case 0x01:
            client_connect_requst_info->addr_length = 4;
            memmove(client_connect_requst_info->dst_addr, &buff[4], 4);
            //端口
            memmove(client_connect_requst_info->dst_port, &buff[8], 2);
            break;
        //域名
        case 0x03:
            //printf("0x03 start1\n");
            length = buff[4];
            client_connect_requst_info->addr_length = length;
            //client_connect_requst_info->addr_length = client_connect_requst_info->addr_length;
            tmp_addr = (unsigned char *)malloc(length);
            memmove(tmp_addr, &buff[5], length);
            //printf("0x03 start2\n");
            //printf("dns name:%s\n", tmp_addr);
            //转为IPV4地址
            struct hostent* host_addr = gethostbyname(tmp_addr);
            //只使用第一个IPV4地址
            memmove(client_connect_requst_info->dst_addr, host_addr->h_addr_list[0], 4);
            //printf("0x03 start3\n");
            //端口
            memmove(client_connect_requst_info->dst_port, &buff[5+length], 2);
            //printf("0x03 start4\n");
            //printf("client_connect_requst_info->dst_port:%d %d\n", client_connect_requst_info->dst_port[0], client_connect_requst_info->dst_port[1]);
            free(tmp_addr);
            //printf("0x03 start5\n");
            break;
        //IPV6
        case 0x04:
            // client_connect_requst_info->addr_length = 16;
            // memmove(client_connect_requst_info->dst_addr, &buff[4], 16);
            // //端口
            // memmove(client_connect_requst_info->dst_port, &buff[20], 2);
            printf("IPV6\n");
            return -1;
            break;
        default:
            perror("无效的地址\n");
            return -1;
            break;
    }
    //printf("read_client_connect_info end\n");
}

int server_connect_dest_addr(const struct client_connect_requst* client_connect_requst_info)
{
    int remote_fd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in dst_server_addr = {};
    dst_server_addr.sin_family = AF_INET;
    memmove(&(dst_server_addr.sin_addr.s_addr), client_connect_requst_info->dst_addr, 4);
    memmove(&dst_server_addr.sin_port, client_connect_requst_info->dst_port, 2);
    if(connect(remote_fd, (struct sockaddr*)&dst_server_addr, sizeof(dst_server_addr)) < 0)
    {
        perror("connect to dst server error\n");
        return -1;
    }
    return remote_fd;
}

int getresponselength(const struct client_connect_requst* client_connect_requst_info)
{
    int length = 0;
    length += client_connect_requst_info->addr_length;
    if(client_connect_requst_info->atyp == 0x03)
        length += 7;
    else
        length += 6;
    return length;
}

int validate_socket5_connect(int fd)
{
    //printf("222\n");
    struct client_hello client_hello_info = {};
    if(get_client_hello_info(fd, &client_hello_info) < 0)
        return -1;
    //printf("3333\n");
    if(client_hello_info.ver != 0x05 || client_hello_info.methods[0] != 0x00)
    {
        perror("服务器当前仅支持0x05 socket5和无验证的方式\n");
        return -1;
    }
    struct server_hello server_hello_info = {0x05, 0x00};
    //返回服务端握手信息
    //printf("sizeof(server_hello_info):%d\n",sizeof(server_hello_info));
    if(write(fd, (void*)&server_hello_info, sizeof(server_hello_info)) < 0)
    {
        perror("server hello write error\n");
        return -1;
    }
    //读取客户端连接信息
    struct client_connect_requst client_connect_requst_info; //记得free
    if(read_client_connect_info(fd, &client_connect_requst_info) < 0)
        return -1;
    //printf("4444\n");
    //服务器与目标地址连接
    int remote_fd = server_connect_dest_addr(&client_connect_requst_info);
    //printf("remote_fd=%d\n",remote_fd);
    if(remote_fd < 0)
        return -1;
    struct server_connect_response server_connect_response_info = {};
    if(client_connect_requst_info.cmd != 0x01)
    {
        printf("当前服务器只支持connect\n");
        return -1;
    }
    if(client_connect_requst_info.atyp == 0x04)
    {
        printf("当前服务器只支持IPV4\n");
        return -1;
    }
    //返回客户端信息
    server_connect_response_info.ver = client_connect_requst_info.ver;
    server_connect_response_info.rep = 0x00; //代表成功
    server_connect_response_info.rsv = client_connect_requst_info.rsv;
    server_connect_response_info.atyp = 0x01; //IPV4
    memmove(server_connect_response_info.bnd_addr, client_connect_requst_info.dst_addr, 4);
    memmove(server_connect_response_info.bnd_port, client_connect_requst_info.dst_port, 2);
    printf("ver:%d  rep:%d  rsv:%d  atyp:%d  ip: %d:%d:%d:%d\n", server_connect_response_info.ver, server_connect_response_info.rep, server_connect_response_info.rsv, server_connect_response_info.atyp, server_connect_response_info.bnd_addr[0], server_connect_response_info.bnd_addr[1], server_connect_response_info.bnd_addr[2], server_connect_response_info.bnd_addr[3]);
    uint16_t tmp_port = 0;
    memmove(&tmp_port, &(server_connect_response_info.bnd_port), 2);
    //printf("tmp pp:%d\n", ntohs(tmp_port));
    //printf("sizeof(server_connect_response_info):%d\n",sizeof(server_connect_response_info));
    if(write(fd, (void *)&server_connect_response_info, sizeof(server_connect_response_info)) < 0)
    {
        perror("write server response error");
        return -1;
    }
    //printf("5555\n");
    return remote_fd;
}

int r_max(int a, int b)
{
    return a > b ? a : b;
}

int forward_data(int cli_fd, int remote_fd)
{
    char data_buf[MAXBUFF];
    int client_fd = cli_fd;
    int server_fd = remote_fd;
    fd_set all_set, copy_set;
    FD_ZERO(&all_set);
    FD_ZERO(&copy_set);
    FD_SET(client_fd, &all_set);
    FD_SET(server_fd, &all_set);
    int max_fd = r_max(client_fd, server_fd)+1;
    printf("forward_data start\n");
    printf("clientfd:%d serverfd:%d\n", client_fd, server_fd);
    copy_set = all_set;
    for( ; ; )
    {
        copy_set = all_set;
        //echo
        int select_res = select(max_fd, &copy_set, NULL, NULL, NULL);
        if(select_res < 0)
        {
            perror("select error");
            return -1;
        }
        if(FD_ISSET(client_fd, &copy_set)) //读取客户端数据，返回给服务端
        {
            int nbytes = read(client_fd, data_buf, MAXBUFF);
            if(nbytes < 0)
            {
                perror("select read client_fd error");
                return -1;
            }
            if(nbytes == 0) //客户端的数据读取完毕,关闭客户端的读，服务端的写
            {
                printf("read bytes1 end:%d\n",nbytes);
                shutdown(client_fd, SHUT_RD);
                shutdown(server_fd, SHUT_WR);
                FD_CLR(client_fd, &all_set);
                continue;
            }
            int write_res = write(server_fd, data_buf, nbytes);
            if(write_res < 0)
            {
                perror("select write client_fd error");
                return -1;
            }
        }
        if(FD_ISSET(server_fd, &copy_set))
        {
            printf("server_fd ss\n");
            int nbytes = read(server_fd, data_buf, MAXBUFF);
            if(nbytes < 0)
            {
                perror("select read server_fd error");
                return -1;
            }
            if(nbytes == 0) //客户端的数据读取完毕,关闭客户端的读，服务端的写
            {
                shutdown(server_fd, SHUT_RD);
                shutdown(client_fd, SHUT_WR);
                break;
            }
            if(write(client_fd, data_buf, nbytes) < 0)
            {
                perror("select write server_fd error");   
                return -1;
            }
        }
    }
    //将当前文件描述符删除
    //epoll_ctl(arg->epfd, EPOLL_CTL_DEL, client_fd, NULL);
    close(client_fd);
    close(remote_fd);
    //整个线程正常终止
    printf("正常终止\n");
    return 1;
}

int handle_client_connect(int client_sock)
{
    // struct epfd_fd args;
    // args.epfd = ((struct epfd_fd*)arg)->epfd;
    // args.fd = ((struct epfd_fd*)arg)->fd;
    //int client_fd = (int)arg;
    //printf("111\n");
    int client_fd = client_sock;
    int remote_fd = validate_socket5_connect(client_fd);
    //printf("remote_fd11:%d\n", remote_fd);
    if(remote_fd < 0)
    {
        printf("错误1\n");
        exit(1);
    }
    return remote_fd;
}



