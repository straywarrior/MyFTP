/*
 * connection.cpp
 * @Author:     StrayWarrior
 * @Date:       2015-12-02
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#include "myftpclient.h"
#include "connection.h"
#include <ctype.h>

/*
 * Protocol Interpreter
 * Note: Need C++11
 */

static int convert_ip_ntostr(unsigned int v4addr, char * buf, char delm = '.'){
    uint8_t sec[4];
    for (int i = 0; i < 4; ++i){
        sec[i] = v4addr & 0xFF;
        v4addr >>= 8;
    }
    sprintf(buf, "%d%c%d%c%d%c%d", sec[0], delm, sec[1], delm, sec[2], delm, sec[3]);
    return 0;
}

static int convert_port_htostr(unsigned int port, char * buf, char delm = ','){
    uint8_t sec[2];
    uint16_t port_s = (uint16_t)port;
    sec[0] = port_s & 0xFF;
    sec[1] = (port_s >> 8) & 0xFF;
    sprintf(buf, "%d%c%d", sec[1], delm, sec[0]);
    return 0;
}

int recv_reply(int ctl_conn, char * recv_buf){
    int len = recv(ctl_conn, recv_buf, MAX_SEND_BUF, 0);
    if (len > 0){
        recv_buf[len] = '\0';
        myftp_log(MYFTP_LOG_DEBUG, "Reply from server received. Length: %d bytes\n", len);
    }else{
        return -1;
    }
    return 0;
}

int recv_and_show_reply(int ctl_conn){
    char recv_buf[MAX_READ_BUF];
    if (recv_reply(ctl_conn, recv_buf) < 0){
        myftp_log(MYFTP_LOG_ERROR, "Failed to receive reply from server.\n");
        return -1;
    }else{
        myftp_log(MYFTP_LOG_INFO, "%s", recv_buf);
    }
    return 0;
}

int parse_reply(int ctl_conn, char * recv_buf);


/*
 * Data Connection
 */
int open_data_connection(int ctl_conn, struct sockaddr_in * data_sock_addr, unsigned int * v4addr, unsigned int * port){
    myftp_log(MYFTP_LOG_DEBUG, "Opening data connection for connection %d.\n", ctl_conn);
    int data_conn;
    //struct sockaddr_in data_sock_addr;
    data_sock_addr->sin_family = AF_INET;
    data_sock_addr->sin_addr.s_addr = *v4addr;
    data_sock_addr->sin_port = 0;
    unsigned int data_sock_addrlen = sizeof(*data_sock_addr);
    if ((data_conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        myftp_log(MYFTP_LOG_FATAL, "Cannot open data sockect for connection %d.\n", ctl_conn);
        return -1;
    }
    if ((bind(data_conn, (struct sockaddr*)data_sock_addr, data_sock_addrlen)) < 0){
        myftp_log(MYFTP_LOG_FATAL, "Cannot bind data socket for connection %d.\n", ctl_conn);
        return -1;
    }
    // Get the IP and port.
    if (getsockname(data_conn, (struct sockaddr *)data_sock_addr, &data_sock_addrlen) < 0){
        myftp_log(MYFTP_LOG_FATAL, "Failed to get IP and port binded.\n");
    }else{
        *v4addr = data_sock_addr->sin_addr.s_addr;
        *port = (unsigned int)ntohs(data_sock_addr->sin_port);
        myftp_log(MYFTP_LOG_INFO, "Data connection listening at %08x:%d \n", *v4addr, *port);
    }

    // Start listening.
    if (listen(data_conn, 10) < 0){
        myftp_log(MYFTP_LOG_ERROR, "Failed to listen on the port. Exit.\n");
        return -1;
    }

    return data_conn;
}

int close_data_connection(int ctl_conn, int data_conn){
    close(data_conn);
    myftp_log(MYFTP_LOG_DEBUG, "Data connection for connection %d closed.\n", ctl_conn);
    return 0;
}

int open_and_send_data_port(int ctl_conn, struct sockaddr_in * data_sock_addr, unsigned int v4addr){
    unsigned int port;
    int data_conn = open_data_connection(ctl_conn, data_sock_addr, &v4addr, &port);
    if (data_conn < 0){
        return -1;
    }
#if 1
    char v4addr_str[16];
    convert_ip_ntostr(v4addr, v4addr_str, ',');
    char port_str[10];
    convert_port_htostr(port, port_str, ',');
    char send_buf[64];
    sprintf(send_buf, "PORT %s,%s\r\n", v4addr_str, port_str);
    myftp_log(MYFTP_LOG_DEBUG, "Sending port parameter: %s", send_buf);
    send_msg(ctl_conn, send_buf);
    recv_and_show_reply(ctl_conn);
#endif
    return data_conn;
}

int recv_and_show_data(int data_conn, struct sockaddr_in * data_sock_addr){
    unsigned int data_sock_addrlen = sizeof(*data_sock_addr);

    int data_recv_conn = accept(data_conn, (struct sockaddr *)data_sock_addr, &data_sock_addrlen);
    if (data_recv_conn < 0){
        myftp_log(MYFTP_LOG_ERROR, "Failed to accept connection.\n");
        return -1;
    }

    char recv_buf[MAX_READ_BUF];
    while(true){
        if (recv_reply(data_recv_conn, recv_buf) < 0){
            myftp_log(MYFTP_LOG_ERROR, "Failed to receive data from server.\n");
            return -1;
        }else{
            myftp_log(MYFTP_LOG_INFO, "%s", recv_buf); 
        }
    }
    close(data_recv_conn);
    return 0;
}

int store_file(int ctl_conn, int data_conn, struct sockaddr_in * data_sock_addr, const char * filename){
    unsigned int data_sock_addrlen = sizeof(*data_sock_addr);

    int data_send_conn = accept(data_conn, (struct sockaddr *)data_sock_addr, &data_sock_addrlen);
    if (data_send_conn < 0){
        myftp_log(MYFTP_LOG_ERROR, "Failed to accept connection.\n");
        return -1;
    }

    int frd_hanle = open(filename, O_RDONLY);
    if (frd_hanle < 0){
        myftp_log(MYFTP_LOG_ERROR, "Cannot open file %s\n", filename);
        return -1;
    }

    char fread_buf[MAX_READ_BUF];
    while(true){
        int len = read(frd_hanle, fread_buf, MAX_READ_BUF);
        if (len > 0){
            send_msg(data_send_conn, fread_buf, len);
        }else{
            break;
        }
    }
    myftp_log(MYFTP_LOG_DEBUG, "file %s stored.\n", filename);

    close(frd_hanle);
    close(data_send_conn);

    return 0;
}

int retrieve_file(int ctl_conn, int data_conn, struct sockaddr_in * data_sock_addr, const char * filename){
    unsigned int data_sock_addrlen = sizeof(*data_sock_addr);

    int data_recv_conn = accept(data_conn, (struct sockaddr *)data_sock_addr, &data_sock_addrlen);
    if (data_recv_conn < 0){
        myftp_log(MYFTP_LOG_ERROR, "Failed to accept connection.\n");
        return -1;
    }
    
    int fwr_handle = open(filename, O_WRONLY|O_CREAT, 0644);
    if (fwr_handle < 0){
        myftp_log(MYFTP_LOG_DEBUG, "Cannot open file %s\n", filename);
        return -1;
    }

    char fwrite_buf[MAX_SEND_BUF];
    while (true){
        int len = recv(data_recv_conn, fwrite_buf, MAX_SEND_BUF, 0);
        if (len > 0){
            write(fwr_handle, fwrite_buf, len);
        }else{
            break;
        }
    }
    myftp_log(MYFTP_LOG_DEBUG, "file %s retrived.\n", filename);

    close(fwr_handle);
    close(data_recv_conn);

    return 0;
}
