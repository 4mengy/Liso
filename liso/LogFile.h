//
// Created by Merlin on 2019-05-20.
//

#ifndef LISO_LOGFILE_H
#define LISO_LOGFILE_H

#include <string>
#include "LogHandler.h"


namespace Liso {
    class LogFile : public LogHandler {
    public:
        LogFile(const std::string& file_name);

        virtual ~LogFile();
        virtual void append(const char* log_line, int len);

    private:
        int fd;
    };
}
#endif //LISO_LOGFILE_H
