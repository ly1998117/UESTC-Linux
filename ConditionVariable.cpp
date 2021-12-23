//
// Created by 刘洋 on 2021/10/15.
//

#include "ConditionVariable.h"

ConditionVariable::ConditionVariable() {
    int r = pthread_cond_init(&condition_variable, 0);
    if (r != 0) {
        CacheManager::writeLog("In ConditionVariable::ConditionVariable(), initial error\n");
        throw "In ConditionVariable::ConditionVariable(), initial error";
    }
}

ConditionVariable::~ConditionVariable() {
    int r = pthread_cond_destroy(&condition_variable);
    if (r != 0) {
        CacheManager::writeLog("In ConditionVariable::~ConditionVariable(), destroy error\n");
        throw "In ConditionVariable::~ConditionVariable(), destroy error";
    }
}

bool ConditionVariable::wait(Mutex *mutex) {
    if (mutex == nullptr)
        return false;
    int r = pthread_cond_wait(&condition_variable, &(mutex->mutex));
    if (r != 0) {
        CacheManager::writeLog("In ConditionVariable::lock(), Mutex lock error\n");
        return false;
    }
    return true;
}

bool ConditionVariable::wakeup() {
    int r = pthread_cond_signal(&condition_variable);
    if (r != 0) {
        CacheManager::writeLog("In ConditionVariable::wakeup(), wakeup error");
        return false;
    }
    return true;
}

bool ConditionVariable::wakeupAll() {
    int r = pthread_cond_broadcast(&condition_variable);
    if (r != 0) {
        CacheManager::writeLog("In ConditionVariable::wakeupAll(), wakeupAll error");
        return false;
    }
    return true;
}
