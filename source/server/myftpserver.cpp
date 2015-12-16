/*
 * myftpserver.cpp
 * @Author:     StrayWarrior
 * @Date:       2015-12-02
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#include "myftpserver.h"
#include "../common/OptionParser.h"
#include "worker.h"
#include <map>

using optparse::OptionParser;

int start_server(myftpserver_t * server_t){
    // Create a socket
    int server_sock;
    struct sockaddr_in server_sock_addr;
    int conn_cnt = 0;
    std::map<pid_t, myftpserver_worker_t *> worker_map;
    
    if ((server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        server_log(SERVER_LOG_FATAL, "Failed to create socket. Exit.\n");
        return(-1);
    }else{
        server_log(SERVER_LOG_DEBUG, "Socket created.\n");
    }

    // Bind a socket
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_port = htons(server_t->port);
    server_sock_addr.sin_addr.s_addr = server_t->ipv4addr;
    unsigned int server_sock_addrlen = sizeof(server_sock_addr);

    if (bind(server_sock, (struct sockaddr *)&server_sock_addr, server_sock_addrlen) < 0){
        server_log(SERVER_LOG_FATAL, "Failed to bind to the port. Exit. \n");
        return(-1);
    }else{
        server_log(SERVER_LOG_INFO, "Port binded. Start listening... \n");
    }

    // Listen to a socket
    if (listen(server_sock, server_t->max_conns) < 0){
        server_log(SERVER_LOG_FATAL, "Failed to listen on the port. Exit. \n");
        return (-1);
    }else{
        server_log(SERVER_LOG_INFO, "Listening started. Ready to accept connections... \n");
    }

    // Start the main loop
    int connection;
    while (true){
        if ((connection = accept(server_sock, (struct sockaddr *) &server_sock_addr, &server_sock_addrlen)) < 0){
            server_log(SERVER_LOG_ERROR, "Failed to accept connection. \n");
            break;
        }else{
            server_log(SERVER_LOG_DEBUG, "Connection accepted.\n");
            // We are in Linux so use multi-process is really easy.
            // TODO: How to do it in Windows? Using thread?
            myftpserver_worker_t * worker_t = new myftpserver_worker_t;
            worker_t->server = server_t;
            worker_t->connection = connection;
            pid_t pid = fork();
            if (pid < 0){
                server_log(SERVER_LOG_FATAL, "Failed to create new process.\n");
                close(connection);
                break;
            }
            if (pid == 0){
                // I'm child
                server_log(SERVER_LOG_DEBUG, "New process created.\n");
                // FIXME: Memory leak here.
                worker_run(worker_t);
                close(connection);
                server_log(SERVER_LOG_INFO, "Connection %d closed.\n", connection);
                exit(0);
            }else{
                // I'm father
                conn_cnt++;
                server_log(SERVER_LOG_INFO, "New connection established. Current connections: %d\n", conn_cnt);
            }    
        }
    }

    close(server_sock);

    return 0;
}

int main(int argc, char * argv[]){
    server_log(SERVER_LOG_INFO, "Starting MyFTP Server... \n");

    OptionParser parser = OptionParser().description("MyFTP Server.");
    parser.add_option("-v", "--verbose").dest("verbose").type("int").set_default("4")
        .help("Set log level. Default: 3 - INFO");
    parser.add_option("-p", "--port").dest("port").type("int").set_default("21")
        .help("Set listening port. Default: 21");
    parser.add_option("-i", "--IP").dest("ip").set_default("127.0.0.1")
        .help("Set listening IP. Default: 127.0.0.1");
    parser.add_option("-m", "--max-conns").dest("max_conns").type("int").set_default("10")
        .help("Set max connections limit. Default: 10");
    parser.add_option("-a", "--allow-anonymous").dest("allow_anony").type("int").set_default("0")
        .help("Allow anonymous connections. Default: 0");
    parser.add_option("-d", "--default-dir").dest("default_dir").set_default("")
        .help("Default dir for anonymous users");
    parser.add_option("-c", "--config").dest("config_file").set_default("")
        .help("Configuration file");
    parser.add_help_option();

    optparse::Values & options = parser.parse_args(argc, argv);

    myftpserver_t server_t;

    server_t.port = (unsigned int)options.get("port");
    server_log(SERVER_LOG_INFO, "Server port parsed: %d \n", server_t.port);

    server_t.ipv4addr = (unsigned int)inet_addr(options["ip"].c_str());
    server_log(SERVER_LOG_INFO, "Server IP parsed: %s Hex: %08x \n", options["ip"].c_str(), server_t.ipv4addr);

    server_t.max_conns = (unsigned int)options.get("max_conns");
    server_log(SERVER_LOG_INFO, "Server max connections: %d\n", server_t.max_conns);

    server_t.allow_anonymous = (bool)options.get("allow_anony");
    server_log(SERVER_LOG_INFO, "Server allows anonymous connections: %d\n", server_t.allow_anonymous);

    strcpy(server_t.default_dir, options["default_dir"].c_str());
    server_log(SERVER_LOG_INFO, "Server default dir: %s\n", server_t.default_dir);

    start_server(&server_t);

    return 0;
}

