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

int worker_run(myftpserver_worker_t * worker_t) {
    int conn_handle = worker_t->connection;
    send_reply(conn_handle, REPCODE_220, strlen(REPCODE_220));

    // TODO: Read user-dir from config dir

    return 0;
}

