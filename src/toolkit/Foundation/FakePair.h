#ifndef TOOLKIT_FOUNDATION_GORGONPRIORITYQUEUE_H
#define TOOLKIT_FOUNDATION_GORGONPRIORITYQUEUE_H


#include <queue>

namespace Foundation {
    template <class T, class U>
    inline bool operator<(const pair<T,U>& lhs, const pair<T, U> &rhs) {
        return (lhs.first < rhs.first);
    }
}

#endif
