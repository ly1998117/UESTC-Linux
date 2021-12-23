//
// Created by 刘洋 on 2021/10/15.
//

#ifndef LINUXCODE_MUTEX_H
#define LINUXCODE_MUTEX_H

#include "pthread.h"
#include "CacheManage.h"

class Mutex {
public:
    friend class ConditionVariable;
    explicit Mutex();
    virtual ~Mutex();

    bool lock();
    bool unlock();

private:
    Mutex(const Mutex &);

    Mutex operator=(const Mutex &);

private:
    pthread_mutex_t mutex;
};


#endif //LINUXCODE_MUTEX_H
