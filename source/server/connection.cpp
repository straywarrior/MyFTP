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
    if (recv_len < 0){
        server_log(SERVER_LOG_WARNING, "Failed to receive from client. Terminating...\n");
        return FTPCMD::ERROR;
    }
    if (recv_len < 3){
        server_log(SERVER_LOG_WARNING, "Failed to read command from client...\n");
        return FTPCMD::UNKNOWN;
    }
    read_buf[recv_len] = '\0';
    server_log(SERVER_LOG_DEBUG, "Read from client... Done. Parse it.\n");
    return parse_command(read_buf, arg_buf);
}

FTPCMD parse_command(char * read_buf, char * arg_buf){
    if (read_buf == nullptr || strlen(read_buf) < 3){
        server_log(SERVER_LOG_ERROR, "Error in parsing command buffer.\n");
        return FTPCMD::ERROR;
    }
    // Find the first space and convert command character to UPPERCASE
    server_log(SERVER_LOG_DEBUG, "Parsing start.\n");
    char * c = read_buf;
    for (char * c; *c != '\0' && *c != ' '; c++){
        server_log(SERVER_LOG_DEBUG, "Parsing...\n");
        //FIXME: Core Dump Here.
        *c = (char)(toupper(*c));
        printf("%c ", toupper(*c));
    }
    int cmd_len = c - read_buf;
    server_log(SERVER_LOG_DEBUG, "Space found. Command length: %d", cmd_len);
    if (cmd_len < 3 || cmd_len > 4){
        server_log(SERVER_LOG_WARNING, "Invalid command.\n");
        return FTPCMD::UNKNOWN;
    }
    split_arg(c, arg_buf);
    if (cmd_len == 3){
        if (strncmp(read_buf, "PWD", 3) == 0){
            return FTPCMD::PWD;
        }
    }
    if (cmd_len == 4){
        if (strncmp(read_buf, "USER", 4) == 0){
            return FTPCMD::USER;
        }
        if (strncmp(read_buf, "PASS", 4) == 0){
            return FTPCMD::PASS;
        }
        if (strncmp(read_buf, "QUIT", 4) == 0){
            return FTPCMD::QUIT;
        }
        if (strncmp(read_buf, "PORT", 4) == 0){
            return FTPCMD::PORT;
        }
        if (strncmp(read_buf, "TYPE", 4) == 0){
            return FTPCMD::TYPE;
        }
        if (strncmp(read_buf, "MODE", 4) == 0){
            return FTPCMD::MODE;
        }
        if (strncmp(read_buf, "STRU", 4) == 0){
            return FTPCMD::STRU;
        }
        if (strncmp(read_buf, "RETR", 4) == 0){
            return FTPCMD::RETR;
        }
        if (strncmp(read_buf, "STOR", 4) == 0){
            return FTPCMD::STOR;
        }
        if (strncmp(read_buf, "NOOP", 4) == 0){
            return FTPCMD::NOOP;
        }
        if (strncmp(read_buf, "LIST", 4) == 0){
            return FTPCMD::LIST;
        }
        if (strncmp(read_buf, "NLST", 4) == 0){
            return FTPCMD::NLST;
        }
        if (strncmp(read_buf, "HELP", 4) == 0){
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
    *d = '\n';
    return 0; 
}

int send_reply(int connection, const char * send_buf, int len){
    if (send(connection, send_buf, len, 0) < 0){
        server_log(SERVER_LOG_ERROR, "Failed to send reply to client. Terminating...\n");
        return -1;
    }else{
        return 0;
    }
}
