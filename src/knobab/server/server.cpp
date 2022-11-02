#include <iostream>
#include <knobab/server/query_manager/ServerQueryManager.h>

int main(int argc, char** argv) {
    ServerQueryManager mgr;
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
    mgr.run(host, port);
    std::cout << "Quitting server..." << std::endl;
    return 0;
}
