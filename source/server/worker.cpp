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
#include "fileoperation.h"

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
            sec[p_second - p_first] = '\0';
            strcat(addr_buf, sec);
            sec_count++;
            server_log(SERVER_LOG_DEBUG, "IP Section: %s ", sec);
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
    server_log(SERVER_LOG_DEBUG, "\nData Connection IP parsed: %s \n", addr_buf);
    
    int port = 0;
    // The High-8bit
    p_second = strstr(p_first, ",");
    strncpy(sec, p_first, p_second - p_first);
    sec[p_second - p_first] = '\0';
    server_log(SERVER_LOG_DEBUG, "Port Section: %s ", sec);
    port = atoi(sec) * 256;
    // The Low-8bit
    p_first = ++p_second;
    strcpy(sec, p_first);
    server_log(SERVER_LOG_DEBUG, "Port Section: %s ", sec);
    port += atoi(sec);
    
    *data_v4addr = (unsigned int)inet_addr(addr_buf);
    *data_port = port;
    return 0;
}

static void get_system_str(char * buf){
#if defined( __APPLE__)
    strcpy(buf, "OSX");
#elif defined(__linux__)
    strcpy(buf, "LINUX");
#endif
}

myftpserver_worker_t::~myftpserver_worker_t(){
    this->server = nullptr;    
}

int worker_run(myftpserver_worker_t * worker_t) {
    int conn_handle = worker_t->connection;
    send_reply(conn_handle, REPCODE_220, strlen(REPCODE_220));

    // TODO: Read username and user-dir from config dir
    
    strcpy(worker_t->rootdir, worker_t->server->default_dir);
    strcpy(worker_t->reladir, "/");
    
    bool conn_close = false;
    bool user_login = false;
    while(!conn_close){
        char arg_buf[MAX_READ_BUF + 1] = {0};
        FTPCMD cur_cmd = read_command(conn_handle, arg_buf);
        server_log(SERVER_LOG_DEBUG, "Command %d from connection %d.\n", static_cast<int>(cur_cmd), conn_handle);;

        if (cur_cmd == FTPCMD::ERROR || cur_cmd == FTPCMD::QUIT){
            send_reply(conn_handle, REPCODE_221, strlen(REPCODE_221));
            conn_close = true;
            continue;
        }
        if (cur_cmd == FTPCMD::UNKNOWN){
            send_reply(conn_handle, REPCODE_503, strlen(REPCODE_503));
            continue;
        }

        if (!user_login){
            switch (cur_cmd){
               case FTPCMD::USER:
                    if (strlen(arg_buf) == 0){
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
                    if (strlen(worker_t->username) > 0){
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
                    if (get_data_conn_parm(arg_buf, &(worker_t->data_v4addr), &(worker_t->data_port)) < 0){
                        send_reply(conn_handle, REPCODE_501, strlen(REPCODE_501));
                    }else{
                        send_reply(conn_handle, REPCODE_200, strlen(REPCODE_200));
                    }
                    break;
                case FTPCMD::TYPE:
                    // TODO: RFC 959 minimum
                    send_reply(conn_handle, REPCODE_200, strlen(REPCODE_200));
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
                    char cur_dir[MAX_PATH_LEN];
                    get_cur_path(worker_t, cur_dir);
                    //strcat(cur_dir, EOL);
                    char send_buf_pwd[MAX_PATH_LEN];
                    prepare_reply(send_buf_pwd, REPCODE_257, cur_dir);
                    send_reply(conn_handle, send_buf_pwd, strlen(send_buf_pwd));
                    break;
                case FTPCMD::LIST:
                    int data_conn;
                    data_conn = open_data_connection(conn_handle, worker_t->data_v4addr, worker_t->data_port);
                    worker_t->data_conn = data_conn;
                    list_dir(worker_t);
                    close_data_connection(conn_handle, data_conn);
                    break;
                case FTPCMD::HELP:
                    send_help(conn_handle);

                    break;
                case FTPCMD::NOOP:
                    break;
                case FTPCMD::SYST:
                    // Sent by FTP built-in Linux
                    char buf[10];
                    get_system_str(buf);
                    char send_buf_syst[100];
                    prepare_reply(send_buf_syst, REPCODE_215, buf);
                    send_reply(conn_handle, send_buf_syst, strlen(send_buf_syst));
                    break;
                default:
                    break;
            }
        }
    }
    delete worker_t;
    return 0;
}

