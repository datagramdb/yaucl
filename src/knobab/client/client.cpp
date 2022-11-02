//
// Created by giacomo on 16/04/2022.
//

#include <string>
#include <iostream>
#include <sstream>

#include <string_view>
static bool endsWith(std::string_view str, std::string_view suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

static bool startsWith(std::string_view str, std::string_view prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

std::string getLine() {
    std::cout << "> " << std::flush;
    std::stringstream s;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (endsWith(line, ";")) {
            s << line;
            std::string tmp = s.str();
            tmp.pop_back();
            return tmp;
        } else {
            s << line << std::endl;
        }
    }
    std::string tmp;
    if (endsWith(line, ";")) {
        tmp = s.str();
        tmp.pop_back();
    }
    return tmp;
}

#include <httplib.h>

int main(int argc, char** argv) {
    std::string host{"localhost"};
    int port = 8795;
    if (argc > 1) {
        std::string tmp{argv[1]};
        try {
            port = std::stoi(tmp);
            if (argc > 2) {
                std::string tmp2{argv[2]};
                std::swap(tmp2, host);
            }
        } catch (...) {
            std::swap(tmp, host);
        }
    }
    using namespace httplib;
    Client cli{host, port};
    std::string str;
    while ((str = getLine()) != "quit") {
        auto repl = cli.Post("/query", str, "text/plain");
        if (repl) {
            std::cout << repl.value().body << std::endl;
        }
    }
}