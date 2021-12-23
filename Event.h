//
// Created by 刘洋 on 2021/10/15.
//

#ifndef LINUXCODE_EVENT_H
#define LINUXCODE_EVENT_H

#include "Mutex.h"
#include "ConditionVariable.h"
#include "CriticalSection.h"

class Event {
public:
    Event(int i = 1);

    virtual ~Event();

    bool wait();

    bool wakeup();

private:
    bool flag;
    int resource;
    Mutex mutex;
    ConditionVariable conditionVariable;
};


#endif //LINUXCODE_EVENT_H
