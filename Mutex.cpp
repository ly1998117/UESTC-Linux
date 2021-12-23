//
// Created by 刘洋 on 2021/10/15.
//

#include "Mutex.h"

Mutex::Mutex() {
    int r = pthread_mutex_init(&mutex, nullptr);
    if (r != 0) {
        CacheManager::writeLog("In Mutex::Mutex(), Mutex initial error\n");
        throw "In Mutex::Mutex(), Mutex initial error";
    }
}

Mutex::~Mutex() {
    int r = pthread_mutex_destroy(&mutex);
    if (r != 0) {
        CacheManager::writeLog("In Mutex::~Mutex(), Mutex destroy error\n");
        throw "In Mutex::~Mutex(), Mutex destroy error";
    }
}

bool Mutex::lock() {
    int r = pthread_mutex_lock(&mutex);
    if (r != 0) {
        CacheManager::writeLog("In Mutex::lock(), Mutex lock error\n");
        return false;
    }
    return true;
}

bool Mutex::unlock() {
    int r = pthread_mutex_unlock(&mutex);
    if (r != 0) {
        CacheManager::writeLog("In Mutex::unlock(), Mutex unlock error\n");
        return false;
    }
    return true;
}