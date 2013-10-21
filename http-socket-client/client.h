#ifndef CLIENT_H_
#define CLIENT_H_

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

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

int state;

void check_error(char * s);

void receive_response(int fd, std::string filename);

void http_request(std::string req, std::string filename, std::string hostname, const char *PORT);

std::string build_request(std::string filename, std::string hostname, std::string port);

#endif
