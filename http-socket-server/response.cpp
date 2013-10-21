#include "response.h"

std::string get_req_file_name(char *req, int *failed) {
    std::stringstream sin(req);
    for (std::string in = ""; sin >> in;)
        if (!in.compare("GET") && sin >> in && in.length() > 1)
            return "response/files" + in;
    *failed |= 1;
    return "bleh";
}

std::string get_mimetype(std::string name, int *failed) {
    // get extension
    std::string ext;
    int extIdx = name.find('.');
    if (extIdx != std::string::npos) {
        ext = name.substr(extIdx);
        // convert to mime
        std::ifstream file_ext_to_mime;
        file_ext_to_mime.open("response/ext_to_mime");
        for (std::string file_ext, file_mime; file_ext_to_mime >> file_ext >> file_mime;)
            if (!file_ext.compare(ext))
                return file_mime;
        file_ext_to_mime.close();
    }
    *failed |= 1;
    return "unk";
}

void open_req_file(std::string file_req_name, std::ifstream &file_req, int *failed) {
    file_req.open(file_req_name.c_str(), std::fstream::in | std::fstream::binary);
    *failed |= file_req.fail() || !file_req_name.compare("response/files/error.html");
}

void send(int fd, std::string str) {
    send(fd, str.c_str(), str.length(), 0);
}

void send(int fd, char c) {
    std::string str(1, c);
    send(fd, str);
}

void send_response(int fd, char *req) {
    int failed = 0;
    // get file name
    std::string file_req_name = get_req_file_name(req, &failed);
    std::string file_req_mimetype = get_mimetype(file_req_name, &failed);
    std::ifstream file_req;
    open_req_file(file_req_name, file_req, &failed);
    if (failed) {
        file_req.close();
        open_req_file("response/files/error.html", file_req, &failed);
    }
    // build response
    std::ifstream file_header;
    file_header.open(!failed ? "response/header_200" : "response/header_404", std::fstream::in);
    // concatenate
    const char *endl = "\r\n";
    send(fd, endl);
    // header
    for(std::string line; getline(file_header, line);)
        send(fd, line), send(fd, endl);
    file_header.close();
    // mimetype
    send(fd, "Content-Type: ");
    send(fd, file_req_mimetype);
    send(fd, endl);
    send(fd, endl);
    // file
    for(char c; file_req >> std::noskipws  >> c;)
        send(fd, c);
    send(fd, '\0');
    file_req.close();
}
