/*
 * fileoperation.cpp
 * @Author:     StrayWarrior
 * @Date:       2015-12-14
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#include "fileoperation.h"
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

}

int init_working_dir(myftpserver_worker_t * worker_t){
    return 0;
}

int change_dir(myftpserver_worker_t * worker_t, const char * pathname);
int get_cur_path(myftpserver_worker_t * worker_t, char * result){
    result[0] = '\0';
    strcat(result, worker_t->reladir);
    return 0;
}
int get_absolute_path(myftpserver_worker_t * worker_t, char * result){
    result[0] = '\0';
    strcat(result, worker_t->rootdir);
    strcat(result, worker_t->reladir);
    return 0;
}
int list_dir(myftpserver_worker_t * worker_t);
DIR * open_dir(char * path);
int close_working_dir(myftpserver_worker_t * worker_t);

