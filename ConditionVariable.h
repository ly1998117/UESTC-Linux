//
// Created by 刘洋 on 2021/10/15.
//

#ifndef LINUXCODE_CONDITIONVARIABLE_H
#define LINUXCODE_CONDITIONVARIABLE_H

#include "pthread.h"
#include "Mutex.h"

class ConditionVariable {
public:
    ConditionVariable();
    virtual ~ConditionVariable();

    bool wait(Mutex *);
    bool wakeup();
    bool wakeupAll();


private:
    pthread_cond_t condition_variable;
};


#endif //LINUXCODE_CONDITIONVARIABLE_H
