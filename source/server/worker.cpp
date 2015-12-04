/*
 * worker.cpp
 * @Author:     StrayWarrior
 * @Date:       2015-12-02
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#include "worker.h"

int worker_run(int connection, myftpserver_t * server_t) {
    send_reply(connection, REPCODE_220, strlen(REPCODE_220));
    return 0;
}

#define prepare_reply(send_buf, reply_fmt, ...) sprintf(send_buf, reply_fmt, __VA_ARGS__)

int send_reply(int connection, const char * send_buf, int len){
    if (send(connection, send_buf, len, 0) < 0){
        server_log(SERVER_LOG_ERROR, "Failed to send reply to client. Terminating...\n");
        return -1;
    }else{
        return 0;
    }
}
