/*
 * worker.cpp
 * @Author:     StrayWarrior
 * @Date:       2015-12-02
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#include "worker.h"
#include "connection.h"

myftpserver_worker_t::~myftpserver_worker_t(){
    this->server = nullptr;    
}

int worker_run(myftpserver_worker_t * worker_t) {
    int conn_handle = worker_t->connection;
    send_reply(conn_handle, REPCODE_220, strlen(REPCODE_220));

    // TODO: Read username and user-dir from config dir
    
    while(true){
        char arg_buf[MAX_READ_BUF + 1] = {0};
        FTPCMD cur_cmd = read_command(conn_handle, arg_buf);
        server_log(SERVER_LOG_DEBUG, "Command %d from connection %d.\n", static_cast<int>(cur_cmd), conn_handle);;
        switch (cur_cmd){
            case FTPCMD::ERROR:
                break;
            default:
                break;
        }

    }

    delete worker_t;
    return 0;
}

