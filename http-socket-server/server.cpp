#include "server.h"

void check_error(char * s) {
    if (state == -1) {
        fprintf(stderr, "error at %s %s\n", s, gai_strerror(state));
        exit(0);
    }
}

int main(void) {
    struct addrinfo hint;
    struct addrinfo *list;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC; //setting the IP version to unspecified, so it can be automatically set.
    hint.ai_flags = AI_PASSIVE; //setting the IP address to the local host.
    hint.ai_socktype = SOCK_STREAM;
    state = getaddrinfo(NULL, PORT, &hint, &list); //creating addrinfo uses HTTP and is set at local host
    check_error("creating addrinfo at line 37");
    int s_fd, new_fd; //socket file descriptors

    //getting valid socket
    struct addrinfo * itr;
    for (itr = list; itr != NULL ; itr = itr->ai_next) {
        s_fd = state = socket(itr->ai_family, itr->ai_socktype,
                itr->ai_protocol);
        check_error("getting socket at line 44");
        state = bind(s_fd, itr->ai_addr, itr->ai_addrlen);
        check_error("binding socket at line 47");
        if (state == 0)
            break;
    }
    state = listen(s_fd, BACKLOG);
    check_error("listening at line 46");
    freeaddrinfo(list); //no need anymore.
    /*
     while true: do
         Listen for connections
         Accept new connection from incoming client and delegate it to worker thread/process.
         Parse HTTP/1.0 request
         Ensure well-formed request (return error otherwise)
         Determine if target file exists and if permissions are set properly(return error otherwise)
         Transmit contents of file to connect (reads from the file and writes on the socket)
         Close the connection
     end while
     */
    struct sockaddr_storage connection_addr;
    socklen_t addr_size;
    addr_size = sizeof connection_addr;
    char * msg;
    while (1) {
        //Accept new connection from incoming client and delegate it to worker thread/process.
        state = new_fd = accept(s_fd, (struct sockaddr *) &connection_addr,
                &addr_size);
        check_error("accepting connection at line 72");
        if (!fork()) { //The child process with pid = 0
            close(s_fd); //Child process don't need to listen
            recv(new_fd, msg, 1000, 0);
            printf("%s\n", msg);
            send_response(new_fd, msg);
            close(new_fd);
            exit(0); //Backing to the parent process
        }
        close(new_fd);//Finished sending and receiving
    }
    return 0;
}
