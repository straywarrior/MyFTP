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

/*
 * Read command from client and split it to CMD and ARGUMENT FIELD
 */
FTPCMD read_command(int connection, char * arg_buf){
    char read_buf[MAX_READ_BUF + 1] = {0};
    int recv_len = recv(connection, &read_buf, MAX_READ_BUF, 0);
    if (recv_len <= 0){
        myftp_log(MYFTP_LOG_WARNING, "Failed to receive from client. Terminating...\n");
        return FTPCMD::ERROR;
    }
    if (recv_len < 3){
        myftp_log(MYFTP_LOG_WARNING, "Failed to read command from client...\n");
        return FTPCMD::UNKNOWN;
    }
    read_buf[recv_len] = '\0';
    myftp_log(MYFTP_LOG_DEBUG, "Read %d bytes from client: %s", recv_len, read_buf);
    FTPCMD result = parse_command(read_buf, arg_buf);
    return result;
}

FTPCMD parse_command(const char * read_buf, char * arg_buf){
    if (read_buf == nullptr || strlen(read_buf) < 3){
        myftp_log(MYFTP_LOG_ERROR, "Error in parsing command buffer.\n");
        return FTPCMD::ERROR;
    }
    // Find the first space and convert command character to UPPERCASE
    const char * c = read_buf;
    for (; *c != '\0' && *c != ' ' && *c != '\r' && *c != '\n'; c++);
    int cmd_len = c - read_buf;

    if (cmd_len < 3 || cmd_len > 4){
        myftp_log(MYFTP_LOG_WARNING, "Invalid command.\n");
        return FTPCMD::UNKNOWN;
    }
    split_arg(c, arg_buf);
    if (cmd_len == 3){
        if (strncasecmp(read_buf, "PWD", 3) == 0){
            return FTPCMD::PWD;
        }
        if (strncasecmp(read_buf, "CWD", 3) == 0){
            return FTPCMD::CWD;
        }
    }
    if (cmd_len == 4){
        if (strncasecmp(read_buf, "USER", 4) == 0){
            return FTPCMD::USER;
        }
        if (strncasecmp(read_buf, "PASS", 4) == 0){
            return FTPCMD::PASS;
        }
        if (strncasecmp(read_buf, "CDUP", 4) == 0){
            return FTPCMD::CDUP;
        }
        if (strncasecmp(read_buf, "QUIT", 4) == 0){
            return FTPCMD::QUIT;
        }
        if (strncasecmp(read_buf, "PORT", 4) == 0){
            return FTPCMD::PORT;
        }
        if (strncasecmp(read_buf, "PASV", 4) == 0){
            // FIXME: Needed by Safari/Chrome
            return FTPCMD::PASV;
        }
        if (strncasecmp(read_buf, "TYPE", 4) == 0){
            return FTPCMD::TYPE;
        }
        if (strncasecmp(read_buf, "MODE", 4) == 0){
            return FTPCMD::MODE;
        }
        if (strncasecmp(read_buf, "STRU", 4) == 0){
            return FTPCMD::STRU;
        }
        if (strncasecmp(read_buf, "RETR", 4) == 0){
            return FTPCMD::RETR;
        }
        if (strncasecmp(read_buf, "STOR", 4) == 0){
            return FTPCMD::STOR;
        }
        if (strncasecmp(read_buf, "SYST", 4) == 0){
            return FTPCMD::SYST;
        }
        if (strncasecmp(read_buf, "NOOP", 4) == 0){
            return FTPCMD::NOOP;
        }
        if (strncasecmp(read_buf, "SIZE", 4) == 0){
            return FTPCMD::SIZE;
        }
        if (strncasecmp(read_buf, "LIST", 4) == 0){
            return FTPCMD::LIST;
        }
        if (strncasecmp(read_buf, "NLST", 4) == 0){
            return FTPCMD::NLST;
        }
        if (strncasecmp(read_buf, "HELP", 4) == 0){
            return FTPCMD::HELP;
        }
    }
    return FTPCMD::UNKNOWN;
}

int split_arg(const char * cmd_buf, char * arg_buf){
    // Copy the string from the first not-space char to <CRLF>
    // TODO: catch the exception of no-<CRLF>
    const char * c = cmd_buf;
    char * d = arg_buf;
    while(*c != '\0' && *c == ' ') c++;
    while(*c != '\0'){
        if (*c == '\r' || *c == '\n')
            break;
        *d = *c;
        c++;
        d++;
    }
    *d = '\0';
    return 0; 
}

int send_reply(int connection, const char * send_buf, int len){
    if (send(connection, send_buf, len, 0) < 0){
        myftp_log(MYFTP_LOG_ERROR, "Failed to send reply to client. Terminating...\n");
        return -1;
    }else{
        return 0;
    }
}

int send_reply(int connection, const char * send_buf){
    return send_reply(connection, send_buf, strlen(send_buf));
}

int recv_reply(int connection, char * recv_buf){
    int len = recv(connection, recv_buf, MAX_SEND_BUF, 0);
    if (len > 0){
        recv_buf[len] = '\0';
        myftp_log(MYFTP_LOG_DEBUG, "Reply from server received. Length: %d bytes\n", len);
    }else{
        return -1;
    }
    return 0;
}

int recv_and_show_reply(int connection){
    char recv_buf[MAX_READ_BUF];
    if (recv_reply(connection, recv_buf) < 0){
        myftp_log(MYFTP_LOG_ERROR, "Failed to receive reply from server.\n");
        return -1;
    }else{
        myftp_log(MYFTP_LOG_INFO, "%s", recv_buf);
    }
    return 0;
}

int parse_reply(int connection, char * recv_buf);


/*
 * Data Connection
 */
int open_data_connection(int connection, struct sockaddr_in * data_sock_addr, unsigned int * v4addr, unsigned int * port){
    myftp_log(MYFTP_LOG_DEBUG, "Opening data connection for connection %d.\n", connection);
    int data_conn;
    //struct sockaddr_in data_sock_addr;
    data_sock_addr->sin_family = AF_INET;
    data_sock_addr->sin_addr.s_addr = *v4addr;
    data_sock_addr->sin_port = 0;
    unsigned int data_sock_addrlen = sizeof(*data_sock_addr);
    if ((data_conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        myftp_log(MYFTP_LOG_FATAL, "Cannot open data sockect for connection %d.\n", connection);
        return -1;
    }
    if ((bind(data_conn, (struct sockaddr*)data_sock_addr, data_sock_addrlen)) < 0){
        myftp_log(MYFTP_LOG_FATAL, "Cannot bind data socket for connection %d.\n", connection);
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
#if 0
    char v4addr_str[16];
    convert_ip_ntostr(*v4addr, v4addr_str, ',');
    char port_str[10];
    convert_port_htostr(*port, port_str, ',');
    char send_buf[64];
    sprintf(send_buf, "PORT %s,%s\r\n", v4addr_str, port_str);
    myftp_log(MYFTP_LOG_DEBUG, "Sending port parameter: %s", send_buf);
    send_reply(connection, send_buf);
    recv_and_show_reply(connection);

    if (listen(data_conn, 10) < 0){
        myftp_log(MYFTP_LOG_ERROR, "Failed to listen on the port. Exit.\n");
        return -1;
    }

    int data_recv_conn = accept(data_conn, (struct sockaddr *)data_sock_addr, &data_sock_addrlen);
    if (data_recv_conn < 0){
        myftp_log(MYFTP_LOG_ERROR, "Failed to accept connection.\n");
        return -1;
    }
    recv_and_show_data(data_recv_conn);
#endif
    return data_conn;
}

int close_data_connection(int connection, int data_conn){
    close(data_conn);
    myftp_log(MYFTP_LOG_DEBUG, "Data connection for connection %d closed.\n", connection);
    return 0;
}

int open_and_send_data_port(int connection, struct sockaddr_in * data_sock_addr, unsigned int v4addr){
    unsigned int port;
    int data_conn = open_data_connection(connection, data_sock_addr, &v4addr, &port);
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
    send_reply(connection, send_buf);
    recv_and_show_reply(connection);
#endif
    return data_conn;
}

int recv_and_show_data(int data_conn, struct sockaddr_in * data_sock_addr){
    if (listen(data_conn, 10) < 0){
        myftp_log(MYFTP_LOG_ERROR, "Failed to listen on the port. Exit.\n");
        return -1;
    }
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
