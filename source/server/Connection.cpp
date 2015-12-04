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

int send_reply(int connection, const char * send_buf, int len){
    if (send(connection, send_buf, len, 0) < 0){
        server_log(SERVER_LOG_ERROR, "Failed to send reply to client. Terminating...\n");
        return -1;
    }else{
        return 0;
    }
}
