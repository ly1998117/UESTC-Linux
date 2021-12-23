//
// Created by 刘洋 on 2021/10/15.
//

#ifndef LINUXCODE_CRITICALSECTION_H
#define LINUXCODE_CRITICALSECTION_H

#include "Mutex.h"

class CriticalSection {
public:
    explicit CriticalSection(Mutex *);
    virtual ~CriticalSection();

private:
    CriticalSection(const CriticalSection &);
    CriticalSection &operator=(const CriticalSection &);

    Mutex *mutex;
};


#endif //LINUXCODE_CRITICALSECTION_H
