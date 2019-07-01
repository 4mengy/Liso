#include <iostream>
#include <algorithm>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#include "AsyncLogging.h"
#include "LogFile.h"

const Liso::AsyncLogging* LOGGER = nullptr;

void daemonize() {
    if (::fork() > 0) ::exit(0);
    ::setsid();
    if (::fork() > 0) ::exit(0);
    ::chdir("/");
    ::umask(0);
}

// https://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
class InputParser {
public:
    InputParser(int argc, char** argv) {
        for (int i = 0; i < argc; i++) {
            tokens.push_back(std::string(argv[i]));
        }
    }

    bool option_exist(const std::string& option) const {
        return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
    }

    const std::string& get_option(const std::string& option) const {
        auto pos = std::find(tokens.begin(), tokens.end(), option);
        if (pos != tokens.end() && pos + 1 != tokens.end()) {
            return *(pos + 1);
        }
        else {
            static const std::string empty_str("");
            return empty_str;
        }
    }

private:
    std::vector<std::string> tokens;
};

int main(int argc, char** argv) {
    std::cout << "Hello, World!" << std::endl;

    InputParser cmd_parser(argc, argv);
    if (!cmd_parser.option_exist("-DFOREGROUND")) {
        //daemonize();
    }

    int port = 8000;
    if (cmd_parser.option_exist("-port")) {
        port = std::stoi(cmd_parser.get_option("-port"));
    }

    Liso::LogFile file_handler("/tmp/test.log");
    Liso::AsyncLogging logger(&file_handler);
    logger.start();

    LOGGER = &logger;

    return 0;
}
