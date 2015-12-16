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

using optparse::OptionParser;

int main(int argc, char * argv[]){
    myftp_log(MYFTP_LOG_INFO, "Starting MyFTP Client... \n");

    OptionParser parser = OptionParser().description("MyFTP Server.");
    parser.add_option("-v", "--verbose").dest("verbose").type("int").set_default("4")
        .help("Set log level. Default: 3 - INFO");
    parser.add_option("-p", "--port").dest("port").type("int").set_default("21")
        .help("Set server port. Default: 21");
    parser.add_option("-i", "--IP").dest("ip").set_default("127.0.0.1")
        .help("Set server IP. Default: 127.0.0.1");
    parser.add_option("-u", "--user").dest("user").set_default("anonymous")
        .help("User used to login. Default: anonymous");
    parser.add_option("-p", "--passwd").dest("passwd").set_default("anonymous")
        .help("Password used to login. Default: anonymous");
    parser.add_help_option();

    optparse::Values & options = parser.parse_args(argc, argv);

    myftpclient_t client_t;

    client_t.port = (unsigned int)options.get("port");
    myftp_log(MYFTP_LOG_INFO, "Server port parsed: %d \n", client_t.port);

    client_t.ipv4addr = (unsigned int)inet_addr(options["ip"].c_str());
    myftp_log(MYFTP_LOG_INFO, "Server IP parsed: %s Hex: %08x \n", options["ip"].c_str(), client_t.ipv4addr);

    strcpy(client_t.user, options["user"].c_str());
    myftp_log(MYFTP_LOG_INFO, "User to use: %s\n", client_t.user);

    return 0;
}


