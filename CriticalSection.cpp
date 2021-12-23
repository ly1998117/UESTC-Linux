//
// Created by 刘洋 on 2021/10/15.
//

#include "CriticalSection.h"

CriticalSection::CriticalSection(Mutex *mutex) {
    if (mutex == nullptr) {
        CacheManager::writeLog("In CriticalSection::CriticalSection(), Mutex is NULL\n");
        throw "In CriticalSection::CriticalSection(), Mutex is NULL";
    }
    this->mutex = mutex;
    bool s = mutex->lock();
    while (!s){
        CacheManager::writeLog("In CriticalSection::CriticalSection(), Mutex lock error\n");
        throw "In CriticalSection::CriticalSection(), Mutex lock error";
    }
}

CriticalSection::~CriticalSection() {
    bool s = mutex->unlock();
    if (!s){
        CacheManager::writeLog("In CriticalSection::~CriticalSection(), Mutex unlock error\n");
        throw "In CriticalSection::~CriticalSection(), Mutex unlock error";
    }
}
