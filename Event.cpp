//
// Created by 刘洋 on 2021/10/15.
//

#include "Event.h"

Event::Event(int i) {
    resource = i;
    flag = false;
}

Event::~Event() = default;

bool Event::wakeup() {
    try {
        {
            CriticalSection cs(&mutex);
            flag = true;
            resource++;
        }
        while (resource > 0 and flag) {
            bool s = conditionVariable.wakeup();
            if (!s) {
                CacheManager::writeLog("In CLEvent::Set(), m_Cond.Wakeup error\n");
                cout << "In CLEvent::Set(), m_Cond.Wakeup error" << endl;
                return false;
            }
        }
    }
    catch (const char *str) {
        CacheManager::writeLog("In CLEvent::Set(), exception arise\n");
        return false;
    }

    return true;
}

bool Event::wait() {
    try {
        {
            CriticalSection cs(&mutex);
            resource--;
        }
        while (!flag && resource <= 0) {
            bool s = conditionVariable.wait(&mutex);
            if (!s) {
                CacheManager::writeLog("In CLEvent::Wait(), m_Cond.Wait error\n");
                return false;
            }
        }
        flag = false;
    }
    catch (const char *str) {
        cout << str << endl;
        CacheManager::writeLog("In CLEvent::Wait(), exception arise\n");
        return false;
    }
    return true;
}