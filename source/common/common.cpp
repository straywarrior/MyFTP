/*
 * common.cpp
 * @Author:     StrayWarrior
 * @Date:       2015-12-16
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 * Distributed under terms of the MIT license.
 */

#include "common.h"

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

int send_msg(int ctl_conn, const char * send_buf, int len){
    if (send(ctl_conn, send_buf, len, 0) < 0){
        myftp_log(MYFTP_LOG_ERROR, "Failed to send message to remote.\n");
        return -1;
    }else{
        return 0;
    }
}

int send_msg(int ctl_conn, const char * send_buf){
    return send_msg(ctl_conn, send_buf, strlen(send_buf));
}


