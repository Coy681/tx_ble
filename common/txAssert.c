#include"txAssert.h"
#include"platform/debug/log.h"

volatile int AAA_ASSERT_LINE;
//__func__,__FILE__,__LINE__,
void tx_assert_handler(const char* file,int line,const char* expr)
{
	AAA_ASSERT_LINE = line;
    #if(TX_DEBUG_LOG_ENABLE)
    LOG_TRACE(1,"assert enter",0,0);
    LOG_TRACE(1,"file",0,0);
    LOG_TRACE(1,file,0,0);
    LOG_TRACE(1,"line",&line,4);
    LOG_TRACE(1,"expression",0,0);
    LOG_TRACE(1,expr,0,0);
    #endif
//#if(1)
//    while(1);
//#endif
}
