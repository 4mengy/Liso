//
// Created by Merlin on 2019-05-27.
//

#ifndef LISO_NONECOPYABLE_H
#define LISO_NONECOPYABLE_H

namespace Liso {
    class NoneCopyable {
    public:
        NoneCopyable(const NoneCopyable&) = delete;
        NoneCopyable& operator= (const NoneCopyable&) = delete;
    protected:
        NoneCopyable() = default;
        ~NoneCopyable() = default;
    };
}

#endif //LISO_NONECOPYABLE_H
