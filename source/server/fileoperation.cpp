/*
 * fileoperation.cpp
 * @Author:     StrayWarrior
 * @Date:       2015-12-14
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#include "fileoperation.h"
#include "connection.h"
#include <string>
#include <vector>

static void clean_path(char * path){
    std::string path_str = std::string(path);
    std::vector<std::string> dir_vec;
    std::string sec;

    path_str += "/";
    for (int i = 0; i < path_str.size(); ++i){
        if (path[i] == '/'){
            if (sec.empty()) continue;
            if (sec == "."){
                // Do Nothing
            }else if (sec == ".."){
                if (dir_vec.size())
                    dir_vec.pop_back();
            }else{
                dir_vec.push_back(sec);
            }
            sec = "";
        }else{
            sec += path[i];
        }
    }

    std::string result;
    if (dir_vec.empty()){
        strcpy(path, "/");
    }else{
        for (int i = 0; i < dir_vec.size(); ++i){
            result += ("/" + dir_vec[i]);
        }
        strcpy(path, result.c_str());
    }
    server_log(SERVER_LOG_DEBUG, "Path clean done: %s\n", path);
}

static DIR * open_dir(const char * path){
    DIR * dir = opendir(path);
    if (!dir){
        server_log(SERVER_LOG_ERROR, "Cannot open directory %s \n", path);
        return nullptr;
        closedir(dir);
    }
    return dir;
}

static int get_fileinfo(const char * filename, char * fileinfo){
    struct stat stat_buf;
    const int date_bufsize = 32;
    if (stat(filename, &stat_buf) != 0){
        return -1;
    }else{
        char date[date_bufsize];
        strftime(date, date_bufsize, "%F %R", localtime(&(stat_buf.st_mtime)));
        sprintf(fileinfo, "%s %lld %s\r\n", filename, stat_buf.st_size, date);
        return 0;
    }
}

int init_working_dir(myftpserver_worker_t * worker_t){
    return 0;
}

int change_dir(myftpserver_worker_t * worker_t, const char * pathname);
int get_cur_path(myftpserver_worker_t * worker_t, char * result){
    result[0] = '\0';
    strcat(result, worker_t->reladir);
    clean_path(result);
    return 0;
}
int get_absolute_path(myftpserver_worker_t * worker_t, char * result){
    result[0] = '\0';
    strcat(result, worker_t->rootdir);
    strcat(result, worker_t->reladir);
    clean_path(result);
    return 0;
}
int list_dir(myftpserver_worker_t * worker_t){
    int data_conn = worker_t->data_conn;

    char cur_path[MAX_PATH_LEN];
    get_absolute_path(worker_t, cur_path);
    DIR * dir = open_dir(cur_path); 
    
    char fileinfo[MAX_PATH_LEN * 2];
    while (true){
        struct dirent * cur_dirent = readdir(dir);
        if (!cur_dirent)
            break;
        if (get_fileinfo(cur_dirent->d_name, fileinfo) != 0){
            server_log(SERVER_LOG_ERROR, "Failed to get file information: %s\n", cur_dirent->d_name);
            closedir(dir);
            return -1;
        }else{
            server_log(SERVER_LOG_DEBUG, "Sending file information: %s\n", cur_dirent->d_name);
            send_reply(data_conn, fileinfo, strlen(fileinfo));
        }
    }

    closedir(dir);
    return 0;
}

int close_working_dir(myftpserver_worker_t * worker_t);

