#include "client.h"

void check_error(char * s) {
    if (state == -1) {
        fprintf(stderr, "error at %s %s\n", s, gai_strerror(state));
        exit(0);
    }
}

void receive_response(int fd, std::string filename) {
    char buffer[1024];
    int bufferSize, newLine = -1;
    std::ofstream out;
    out.open(filename.c_str(), std::fstream::binary);
    std::stringstream header;
    bool headerDone = false;
    while (!headerDone && (bufferSize = recv(fd, buffer, 1024, 0)) > 0) {
        buffer[bufferSize] = '\0';
        header << std::noskipws << buffer;
        // header
        std::string str = header.str();
        newLine = str.find("\r\n\r\n", newLine + 1);
        if (newLine > 0) {
            std::string headerStr = str.substr(0, newLine);
            printf("%s\n", headerStr.c_str());
            out << str.substr(newLine + 4);
            headerDone = true;
        }
    }
    // file
    while ((bufferSize = recv(fd, buffer, 1024, 0)) > 0) {
        buffer[bufferSize] = '\0';
        out.write(buffer, bufferSize);
    }
    out.close();
}

void http_request(std::string req, std::string filename, std::string hostname, const char *PORT) {
    struct addrinfo hint;
    struct addrinfo *list;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC; //setting the IP version to unspecified, so it can be automatically set.
    hint.ai_socktype = SOCK_STREAM;

    state = getaddrinfo(NULL, PORT, &hint, &list); //creating addrinfo uses HTTP and is set at local host
    check_error("creating addrinfo at line 37");
    int s_fd; //socket file descriptor
    //getting valid socket
    struct addrinfo * itr;
    for (itr = list; itr != NULL; itr = itr->ai_next) {
        state = s_fd = socket(itr->ai_family, itr->ai_socktype,
                itr->ai_protocol);  //getting socket
        check_error("getting socket");
        state = connect(s_fd, itr->ai_addr, itr->ai_addrlen);
        check_error("connecting");
        break;
    }
    state = itr == NULL ? -1 : 0;
    check_error("connection establishing");
    freeaddrinfo(list);
    //Sending request
    send(s_fd, req.c_str(), req.length(), 0);
    receive_response(s_fd, filename);
    close(s_fd);
}

std::string build_request(std::string filename, std::string hostname, std::string port) {
    std::string out = "GET /";
    out += filename;
    out += " HTTP/1.0\r\n\r\n\0\0";
    return out;
}

int main(void) {
    for(std::string line; getline(std::cin, line);) {
        std::string token;
        std::stringstream tokenizer(line);
        std::string filename, hostname = "localhost", port = "80";
        // filename
        tokenizer >> token;
        filename = token;
        // hostname
        if (tokenizer >> token)
            hostname = token;
        // port number
        if (tokenizer >> token)
            port = token;
        // to request
        std::string request = build_request(filename, hostname, port);
        http_request(request, filename, hostname, port.c_str());
    }
    return 0;
}
