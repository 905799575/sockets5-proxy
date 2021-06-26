#ifndef __COMMONH__
#define __COMMONH__

struct epfd_fd
{
    int epfd;
    int fd;
};

struct client_hello
{
    unsigned char ver;
    unsigned char nmethods;
    unsigned char methods[255];
};

struct server_hello
{
    unsigned char ver;
    unsigned char method;
};

struct client_connect_requst
{
    unsigned char ver;
    unsigned char cmd;
    unsigned char rsv;
    unsigned char atyp;
    uint8_t addr_length; 
    unsigned char dst_addr[4]; //存储32位IP地址
    unsigned char dst_port[2];
};

struct server_connect_response
{
    unsigned char ver;
    unsigned char rep;
    unsigned char rsv;
    unsigned char atyp;
    unsigned char bnd_addr[4];
    unsigned char bnd_port[2];
};

#endif