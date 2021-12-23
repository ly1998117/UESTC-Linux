#include "CLThread.h"
#include "CLExecutiveFunctionProvider.h"


bool CLThread::Run(void *pContext) {
    m_pContext = pContext;

    int r = pthread_create(&m_ThreadID, nullptr, StartFunctionOfThread, this);
    std::cout << "Thread ID [" << m_ThreadID << "] created" << std::endl;
    if (r != 0) {
        CacheManager::writeLog("In CLThread::Run(), pthread_create error\n");
        return false;
    }
    return true;
}

void *CLThread::StartFunctionOfThread(void *pThis) {
    auto *pThreadThis = (CLThread *) pThis;

    pThreadThis->m_pExecutiveFunctionProvider->RunExecutiveFunction(pThreadThis->m_pContext);

    return nullptr;
}

bool CLThread::WaitForDeath() {
    int r = pthread_join(m_ThreadID, nullptr);
    if (r != 0) {
        CacheManager::writeLog("In CLThread::WaitForDeath(), pthread_join error\n");
        return false;
    }
    return true;
}

CLThread::~CLThread(){
    int r = pthread_join(m_ThreadID, nullptr);
    if (r != 0) {
        CacheManager::writeLog("In CLThread::WaitForDeath(), pthread_join error\n");
    }
};