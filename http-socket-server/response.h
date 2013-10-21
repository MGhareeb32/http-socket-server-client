#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

std::string get_req_file_name(char *req, int *failed);

std::string get_mimetype(std::string name, int *failed);

void open_req_file(std::string file_req_name, std::ifstream &file_req, int *failed);

void send(int fd, std::string str);

void send(int fd, char c);

void send_response(int fd, char *req);

#endif
