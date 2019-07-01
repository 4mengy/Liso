//
// Created by Merlin on 2019-05-22.
//

#ifndef LISO_FIXEDBUF_H
#define LISO_FIXEDBUF_H

#include <array>
#include <mutex>
#include <memory>
#include <string.h>
#include "NoneCopyable.h"


namespace Liso {
    using Liso::NoneCopyable;

    template <int SIZE>
    class FixedBuf : NoneCopyable {
    public:
        FixedBuf() : curr_pos(buf) {}
        void put(const char* msg, int len) {
            memcpy(curr_pos, msg, len);
            curr_pos += len;
        }
        bool empty() {
            return curr_pos == buf;
        }
        bool avaible(int len) {
            if (curr_pos + len > buf + SIZE - 1) {
                *curr_pos = 0;
                return false;
            }
            return true;
        }
        inline const char* data() {
            return buf;
        }

        inline const int length() {
            return curr_pos - buf;
        }

    private:
        char* curr_pos;
        char buf[SIZE];
    };
}

#endif //LISO_FIXEDBUF_H
