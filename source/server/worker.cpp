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

static int get_data_conn_parm(char * arg_buf, unsigned int * data_v4addr, unsigned int * data_port){
    char addr_buf[16];
    addr_buf[0] = '\0';
    int sec_count = 0;
    char * p_first = arg_buf;
    char * p_second = arg_buf;
    char sec[4];
    while (sec_count < 4 && p_second && *p_second != '\0'){
        if (*p_second == ','){
            strncpy(sec, p_first, p_second - p_first);
            strcat(addr_buf, sec);
            sec_count++;
            if (sec_count != 4){
                strcat(addr_buf, ".");
            }
            p_first = ++p_second;
        }else{
            ++p_second;
        }
    }
    if (sec_count != 4){
        return -1;
    }
    
    int port = 0;
    // The High-8bit
    p_second = strstr(p_first, ",");
    strncpy(sec, p_first, p_second - p_first);
    port = atoi(sec) * 256;
    // The Low-8bit
    p_first = ++p_second;
    strcpy(sec, p_first);
    port += atoi(sec);
    
    *data_v4addr = (unsigned int)inet_addr(addr_buf);
    *data_port = port;
    return 0;
}

myftpserver_worker_t::~myftpserver_worker_t(){
    this->server = nullptr;    
}

int worker_run(myftpserver_worker_t * worker_t) {
    int conn_handle = worker_t->connection;
    send_reply(conn_handle, REPCODE_220, strlen(REPCODE_220));

    // TODO: Read username and user-dir from config dir
    
    strcpy(worker_t->rootdir, worker_t->server->default_dir);
    strcpy(worker_t->reladir, ".");
    
    bool conn_close = false;
    bool user_login = false;
    while(!conn_close){
        char arg_buf[MAX_READ_BUF + 1] = {0};
        FTPCMD cur_cmd = read_command(conn_handle, arg_buf);
        server_log(SERVER_LOG_DEBUG, "Command %d from connection %d.\n", static_cast<int>(cur_cmd), conn_handle);;

        if (cur_cmd == FTPCMD::ERROR || cur_cmd == FTPCMD::QUIT){
            conn_close = true;
        }
        if (cur_cmd == FTPCMD::UNKNOWN){
            send_reply(conn_handle, REPCODE_503, strlen(REPCODE_503));
            continue;
        }

        if (!user_login){
            switch (cur_cmd){
               case FTPCMD::USER:
                    if (arg_buf == nullptr || strlen(arg_buf) == 0){
                        send_reply(conn_handle, REPCODE_501, strlen(REPCODE_501));
                        break;
                    }
                    if (strcmp(arg_buf, "anonymous") == 0){
                        // Not in RFC 959. Suggested by RFC 1635
                    }
                    strcpy(worker_t->username, arg_buf);
                    send_reply(conn_handle, REPCODE_331, strlen(REPCODE_331));
                    break;
                case FTPCMD::PASS:
                    // TODO: Add Password-check after adding config-read
                    if (worker_t->username != nullptr){
                        user_login = true;
                        send_reply(conn_handle, REPCODE_230, strlen(REPCODE_230));
                    }
                    break;
                default:
                    send_reply(conn_handle, REPCODE_530, strlen(REPCODE_530));
                    break;
            }
        }else{
            switch (cur_cmd){
                case FTPCMD::CWD:
                    // TODO: Add change working dir command.
                    break;
                case FTPCMD::PORT:
                    // TODO: RFC 959 minimum
                    get_data_conn_parm(arg_buf, &(worker_t->data_v4addr), &(worker_t->data_port));
                    break;
                case FTPCMD::TYPE:
                    // TODO: RFC 959 minimum
                    break;
                case FTPCMD::STRU:
                    // TODO: RFC 959 minimum
                    break;
                case FTPCMD::MODE:
                    // TODO: RFC 959 minimum
                    break;
                case FTPCMD::RETR:
                    
                    break;
                case FTPCMD::STOR:

                    break;
                case FTPCMD::PWD:

                    break;
                case FTPCMD::LIST:
                    break;
                case FTPCMD::HELP:

                    break;
                case FTPCMD::NOOP:
                    break;

                default:
                    break;
            }
        }
    }
    delete worker_t;
    return 0;
}

