#include"txAssert.h"
#include"debug/log/log.h"

volatile int AAA_ASSERT_LINE;
//__func__,__FILE__,__LINE__,
void tx_assert_handler(const char* file,int line,const char* expr)
{
	AAA_ASSERT_LINE = line;
    #if(TX_DEBUG_LOG_ENABLE)
    LOG_STR(1,"assert enter");
    LOG_STR(1,"file");
    LOG_HEX(1,file,0,0);
    LOG_HEX(1,"line",&line,4);
    LOG_STR(1,"expression");
    LOG_HEX(1,expr,0,0);
    #endif
//#if(1)
//    while(1);
//#endif
}
