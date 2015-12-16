/*
 * connection.cpp
 * @Author:     StrayWarrior
 * @Date:       2015-12-02
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#include "myftpserver.h"
#include "connection.h"
#include <ctype.h>

/*
 * Protocol Interpreter
 * Note: Need C++11
 */

/*
 * Read command from client and split it to CMD and ARGUMENT FIELD
 */
FTPCMD read_command(int ctl_conn, char * arg_buf){
    char read_buf[MAX_READ_BUF + 1] = {0};
    int recv_len = recv(ctl_conn, &read_buf, MAX_READ_BUF, 0);
    if (recv_len <= 0){
        server_log(SERVER_LOG_WARNING, "Failed to receive from client. Terminating...\n");
        return FTPCMD::ERROR;
    }
    if (recv_len < 3){
        server_log(SERVER_LOG_WARNING, "Failed to read command from client...\n");
        return FTPCMD::UNKNOWN;
    }
    read_buf[recv_len] = '\0';
    server_log(SERVER_LOG_DEBUG, "Read %d bytes from client: %s", recv_len, read_buf);
    FTPCMD result = parse_command(read_buf, arg_buf);
    return result;
}

int send_help(int ctl_conn){
    char command_impled[] =
        "USER PASS PWD CWD CDUP PORT RETR STOR SYST QUIT";
    char help_msg[MAX_SEND_BUF];
    sprintf(help_msg, "All avaliable commands: %s\r\n", command_impled);
    send_msg(ctl_conn, help_msg, strlen(help_msg));

    return 0;
}


/*
 * Data Connection
 */
int open_data_connection(int ctl_conn, unsigned int v4addr, unsigned int port){
    server_log(SERVER_LOG_DEBUG, "Opening data connection at %08x:%d for connection %d.\n", v4addr, port, ctl_conn);
    int data_conn;
    struct sockaddr_in data_sock_addr;
    data_sock_addr.sin_family = AF_INET;
    data_sock_addr.sin_addr.s_addr = v4addr;
    data_sock_addr.sin_port = htons(port);
    if ((data_conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        server_log(SERVER_LOG_FATAL, "Cannot open data sock at %08x:%d for connection %d.\n", v4addr, port, ctl_conn);
        send_msg(ctl_conn, REPCODE_425, strlen(REPCODE_425));
        return -1;
    }
    if ((connect(data_conn, (struct sockaddr*)&data_sock_addr, sizeof(data_sock_addr))) < 0){
        server_log(SERVER_LOG_FATAL, "Cannot open data connection at %08x:%d for connection %d.\n", v4addr, port, ctl_conn);
        send_msg(ctl_conn, REPCODE_425, strlen(REPCODE_425));
        return -1;
    }
    server_log(SERVER_LOG_DEBUG, "Data connection at %08x:%d for connection %d opened.\n", v4addr, port, ctl_conn);
    return data_conn;
}

int close_data_connection(int ctl_conn, int data_conn){
    close(data_conn);
    send_msg(ctl_conn, REPCODE_226, strlen(REPCODE_226));
    server_log(SERVER_LOG_DEBUG, "Data connection for connection %d closed.\n", ctl_conn);
    return 0;
}
