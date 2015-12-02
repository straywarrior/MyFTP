/*
 * myftpserver.cpp
 * @Author:     StrayWarrior
 * @Date:       2015-12-02
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#include "myftpserver.h"

using optparse::OptionParser;

int main(int argc, char * argv[]){
    server_log(SERVER_LOG_INFO, "Starting MyFTP Server... \n");

    OptionParser parser = OptionParser().description("MyFTP Server.");
    parser.add_option("-v", "--verbose").dest("verbose").type("int").set_default("4")
        .help("Set log level. Default: INFO");
    parser.add_option("-p", "--port").dest("port").type("int").set_default("21")
        .help("Set listening port. Default: 21");
    parser.add_option("-i", "--IP").dest("ip").set_default("127.0.0.1")
        .help("Set listening IP. Default: 127.0.0.1");
    parser.add_help_option();

    optparse::Values & options = parser.parse_args(argc, argv);

    myftpserver_t;

    
    return 0;
}

