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
FTPCMD read_command(int connection, char * arg_buf){
    char read_buf[MAX_READ_BUF + 1] = {0};
    int recv_len = recv(connection, &read_buf, MAX_READ_BUF, 0);
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

FTPCMD parse_command(char * read_buf, char * arg_buf){
    if (read_buf == nullptr || strlen(read_buf) < 3){
        server_log(SERVER_LOG_ERROR, "Error in parsing command buffer.\n");
        return FTPCMD::ERROR;
    }
    // Find the first space and convert command character to UPPERCASE
    char * c = read_buf;
    for (; *c != '\0' && *c != ' ' && *c != '\r' && *c != '\n'; c++){
        *c = (char)(toupper(*c));
    }
    int cmd_len = c - read_buf;
    if (cmd_len < 3 || cmd_len > 4){
        server_log(SERVER_LOG_WARNING, "Invalid command.\n");
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
        server_log(SERVER_LOG_ERROR, "Failed to send reply to client. Terminating...\n");
        return -1;
    }else{
        server_log(SERVER_LOG_DEBUG, "Sent %d bytes to client.\n", len);
        return 0;
    }
}

int send_reply(int connection, const char * send_buf){
    return send_reply(connection, send_buf, strlen(send_buf));
}

int send_help(int connection){
    char command_impled[] =
        "USER PASS PWD CWD CDUP PORT RETR STOR SYST QUIT";
    char help_msg[MAX_SEND_BUF];
    sprintf(help_msg, "All avaliable commands: %s\r\n", command_impled);
    send_reply(connection, help_msg, strlen(help_msg));

    return 0;
}


/*
 * Data Connection
 */
int open_data_connection(int connection, unsigned int v4addr, unsigned int port){
    server_log(SERVER_LOG_DEBUG, "Opening data connection at %08x:%d for connection %d.\n", v4addr, port, connection);
    int data_conn;
    struct sockaddr_in data_sock_addr;
    data_sock_addr.sin_family = AF_INET;
    data_sock_addr.sin_addr.s_addr = v4addr;
    data_sock_addr.sin_port = htons(port);
    if ((data_conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        server_log(SERVER_LOG_FATAL, "Cannot open data sock at %08x:%d for connection %d.\n", v4addr, port, connection);
        send_reply(connection, REPCODE_425, strlen(REPCODE_425));
        return -1;
    }
    if ((connect(data_conn, (struct sockaddr*)&data_sock_addr, sizeof(data_sock_addr))) < 0){
        server_log(SERVER_LOG_FATAL, "Cannot open data connection at %08x:%d for connection %d.\n", v4addr, port, connection);
        send_reply(connection, REPCODE_425, strlen(REPCODE_425));
        return -1;
    }
    server_log(SERVER_LOG_DEBUG, "Data connection at %08x:%d for connection %d opened.\n", v4addr, port, connection);
    return data_conn;
}

int close_data_connection(int connection, int data_conn){
    close(data_conn);
    send_reply(connection, REPCODE_226, strlen(REPCODE_226));
    server_log(SERVER_LOG_DEBUG, "Data connection for connection %d closed.\n", connection);
    return 0;
}
