/*
 * myftpclient.cpp
 * @Author:     StrayWarrior
 * @Date:       2015-12-16
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#include "myftpclient.h"
#include "../common/OptionParser.h"
#include "connection.h"
#include <string>
#include <iostream>

using optparse::OptionParser;

static int start_client(myftpclient_t * client_t){
    bool user_login = false;

    int client_sock;
    struct sockaddr_in client_sock_addr;

    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        myftp_log(MYFTP_LOG_FATAL, "Failed to create socket. Exit.\n");
        return -1;
    }else{
        myftp_log(MYFTP_LOG_DEBUG, "Socket created.\n");
    }

    client_sock_addr.sin_family = AF_INET;
    client_sock_addr.sin_port = htons(client_t->port);
    client_sock_addr.sin_addr.s_addr = client_t->server_ipv4addr;

    if ((connect(client_sock, (struct sockaddr *)&client_sock_addr, sizeof(client_sock_addr)) < 0)){
        myftp_log(MYFTP_LOG_ERROR, "Cannot connect to server. Exit.\n"); // TODO: Retry in some minutes?
        close(client_sock);
        return -1;
    }
    myftp_log(MYFTP_LOG_INFO, "Waiting for the welcome message...\n");
    recv_and_show_reply(client_sock);

    while (true){
        std::cout << ">" << std::flush;
        std::string cmd_buf;
        std::getline(std::cin, cmd_buf);
        cmd_buf += "\r\n";
        FTPCMD cur_cmd;
        char arg_buf[MAX_PATH_LEN];
        cur_cmd = parse_command((const char *)cmd_buf.c_str(), arg_buf);
        int data_conn;
        switch (cur_cmd) {
            case FTPCMD::USER:
                send_reply(client_sock, cmd_buf.c_str());
                recv_and_show_reply(client_sock);
                break;
            case FTPCMD::PASS:
                send_reply(client_sock, cmd_buf.c_str());
                recv_and_show_reply(client_sock);
                break;
            case FTPCMD::PWD:
                send_reply(client_sock, cmd_buf.c_str());
                recv_and_show_reply(client_sock);
                break;
            case FTPCMD::CWD:
                send_reply(client_sock, cmd_buf.c_str());
                recv_and_show_reply(client_sock);
                break;
            case FTPCMD::CDUP:
                send_reply(client_sock, cmd_buf.c_str());
                recv_and_show_reply(client_sock);
                break;
            case FTPCMD::LIST:
                {
                    struct sockaddr_in data_sock_addr;
                    data_conn = open_and_send_data_port(client_sock, &data_sock_addr, client_t->local_ipv4addr);
                    send_reply(client_sock, cmd_buf.c_str());
                    recv_and_show_reply(client_sock);
                    recv_and_show_data(data_conn, &data_sock_addr);
                    recv_and_show_reply(client_sock);
                    close_data_connection(client_sock, data_conn);
                }
                break;
            case FTPCMD::PORT:
                send_reply(client_sock, cmd_buf.c_str());
                recv_and_show_reply(client_sock);
                break;
            case FTPCMD::PASV:
                send_reply(client_sock, cmd_buf.c_str());
                recv_and_show_reply(client_sock);
                break;
            case FTPCMD::STOR:
                {
                    struct sockaddr_in data_sock_addr;
                    data_conn = open_and_send_data_port(client_sock, &data_sock_addr, client_t->local_ipv4addr);
                    send_reply(client_sock, cmd_buf.c_str());
                    store_file(client_sock, data_conn, &data_sock_addr, arg_buf);
                    close_data_connection(client_sock, data_conn);
                    recv_and_show_reply(client_sock);
                }
                break;
             case FTPCMD::RETR:
                {
                    struct sockaddr_in data_sock_addr;
                    data_conn = open_and_send_data_port(client_sock, &data_sock_addr, client_t->local_ipv4addr);
                    send_reply(client_sock, cmd_buf.c_str());
                    retrieve_file(client_sock, data_conn, &data_sock_addr, arg_buf);
                    close_data_connection(client_sock, data_conn);
                    recv_and_show_reply(client_sock);
                }
                break;
            case FTPCMD::HELP:
                send_reply(client_sock, cmd_buf.c_str());
                break;
            case FTPCMD::QUIT:
                close(client_sock);
                return 0;
                break;
            case FTPCMD::ERROR:
            case FTPCMD::UNKNOWN:
                myftp_log(MYFTP_LOG_ERROR, "Invalid command.\n");
                break;
            default:
                break;
        }
    }
 
    return 0;
}

int main(int argc, char * argv[]){
    myftp_log(MYFTP_LOG_INFO, "Starting MyFTP Client... \n");

    OptionParser parser = OptionParser().description("MyFTP Server.");
    parser.add_option("-v", "--verbose").dest("verbose").type("int").set_default("4")
        .help("Set log level. Default: 3 - INFO");
    parser.add_option("-p", "--port").dest("port").type("int").set_default("21")
        .help("Set server port. Default: 21");
    parser.add_option("-i", "--server").dest("server").set_default("127.0.0.1")
        .help("Set server IP. Default: 127.0.0.1");
    parser.add_option("-l", "--local").dest("local").set_default("127.0.0.1")
        .help("Set local IP. Default: 127.0.0.1");
    parser.add_option("-u", "--user").dest("user").set_default("anonymous")
        .help("User used to login. Default: anonymous");
    parser.add_option("-w", "--passwd").dest("passwd").set_default("anonymous")
        .help("Password used to login. Default: anonymous");
    parser.add_help_option();

    optparse::Values & options = parser.parse_args(argc, argv);

    myftpclient_t client_t;

    client_t.port = (unsigned int)options.get("port");
    myftp_log(MYFTP_LOG_INFO, "Server port parsed: %d \n", client_t.port);

    client_t.server_ipv4addr = (unsigned int)inet_addr(options["server"].c_str());
    myftp_log(MYFTP_LOG_INFO, "Server IP parsed: %s Hex: %08x \n", options["server"].c_str(), client_t.server_ipv4addr);

    client_t.local_ipv4addr = (unsigned int)inet_addr(options["local"].c_str());
    myftp_log(MYFTP_LOG_INFO, "Local IP parsed: %s Hex: %08x \n", options["local"].c_str(), client_t.local_ipv4addr);

    strcpy(client_t.user, options["user"].c_str());
    myftp_log(MYFTP_LOG_INFO, "User to use: %s\n", client_t.user);

    start_client(&client_t);

    return 0;
}

