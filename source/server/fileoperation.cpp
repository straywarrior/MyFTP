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

static int get_fileinfo(const char * cur_dir, const char * filename, char * fileinfo){
    struct stat stat_buf;
    const int date_bufsize = 32;
    char name_buf[MAX_PATH_LEN];
    sprintf(name_buf, "%s/%s", cur_dir, filename);
    if (stat(name_buf, &stat_buf) != 0){
        int err = errno;
        switch (err){
            case EACCES:
                server_log(SERVER_LOG_ERROR, "Search permission is denied.\n");
                break;
            case EBADF:
                server_log(SERVER_LOG_ERROR, "fd is bad.\n");
                break;
            case EFAULT:
                server_log(SERVER_LOG_ERROR, "Bad address.\n");
                break;
            case ELOOP:
                server_log(SERVER_LOG_ERROR, "Too many symbolic links encountered while traversing the path.\n");
                break;
            case ENAMETOOLONG:
                server_log(SERVER_LOG_ERROR, "pathname is too long.\n");
                break;
            case ENOENT:
                server_log(SERVER_LOG_ERROR, "A component of pathname does not exist, or pathname is an empty string.\n");
                break;
            case ENOMEM:
                server_log(SERVER_LOG_ERROR, "Out of memory.\n");
                break;
            case ENOTDIR:
                server_log(SERVER_LOG_ERROR, "A component of the path prefix of pathname is not a directory.\n");
                break;
            case EOVERFLOW:
                server_log(SERVER_LOG_ERROR, "pathname or fd refers to a file whose size, inode number, or number of blocks cannot be represented in.\n");
                break;
            case EINVAL:
                server_log(SERVER_LOG_ERROR, "Invalid flag specified in flags.\n");
                break;
        }
        server_log(SERVER_LOG_ERROR, "Failed to get file information: %s\n", filename);
        return -1;
    }else{
        char date[date_bufsize];
        char mode[11] = "----------";
        int bit_mask = stat_buf.st_mode & S_IFMT;
        if (bit_mask == S_IFDIR){
            mode[0] = 'd';
        }else if (bit_mask == S_IFREG){
            mode[0] = '-';
        }else{
            server_log(SERVER_LOG_WARNING, "%s is not a regular file.\n", filename);
            return -2;
        }
        strftime(date, date_bufsize, "%D %R", localtime(&(stat_buf.st_mtime)));
        sprintf(fileinfo, "%s %d %s %s %lld %s %s\r\n", mode, stat_buf.st_nlink, "-", "-", stat_buf.st_size, date, filename);
        return 0;
    }
}

int change_dir(myftpserver_worker_t * worker_t, const char * pathname){
    char dir_buf[MAX_PATH_LEN];
    sprintf(dir_buf, "%s/%s", worker_t->reladir, pathname);
    clean_path(dir_buf);
    char absolute_dir[MAX_PATH_LEN];
    sprintf(absolute_dir, "%s/%s", worker_t->rootdir, dir_buf);
    DIR * dir = open_dir(absolute_dir);
    if (!dir){
        return -1;
    }
    // Success: change the relative path in worker_t.
    strcpy(worker_t->reladir, dir_buf);
    closedir(dir);
    return 0;
}
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
    if (!dir){
        send_reply(worker_t->connection, REPCODE_450, strlen(REPCODE_450));
        closedir(dir);
        return -1;
    }

    send_reply(worker_t->connection, REPCODE_150, strlen(REPCODE_150));
    
    char fileinfo[MAX_PATH_LEN * 2];
    while (true){
        struct dirent * cur_dirent = readdir(dir);
        if (!cur_dirent)
            break;
        int status = get_fileinfo(cur_path, cur_dirent->d_name, fileinfo);
        if (status == 0){
            server_log(SERVER_LOG_DEBUG, "Sending file information: %s\n", cur_dirent->d_name);
            send_reply(data_conn, fileinfo, strlen(fileinfo));
        }
    }

    closedir(dir);
    return 0;
}

