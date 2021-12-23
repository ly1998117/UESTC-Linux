#ifndef LINUXCODE_CLEXECUTIVEFUNCTIONPROVIDER_H
#define LINUXCODE_CLEXECUTIVEFUNCTIONPROVIDER_H

#include "CacheManage.h"

class CLExecutiveFunctionProvider
{
public:
    CLExecutiveFunctionProvider(){};
	virtual ~CLExecutiveFunctionProvider(){};

public:
	virtual bool RunExecutiveFunction(void* pContext) = 0;

private:
     /* 复制构造函数*/
	CLExecutiveFunctionProvider(const CLExecutiveFunctionProvider&);
	CLExecutiveFunctionProvider& operator=(const CLExecutiveFunctionProvider&);
};


#endif
