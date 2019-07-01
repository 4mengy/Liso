//
// Created by Merlin on 2019-05-20.
//

#ifndef LISO_LOGHANDLER_H
#define LISO_LOGHANDLER_H

namespace Liso {
    class LogHandler {
    public:
        virtual void append(const char* msg, int len) = 0;
        virtual ~LogHandler() = default;
    };
}

#endif //LISO_LOGHANDLER_H
