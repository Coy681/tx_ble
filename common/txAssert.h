#include"config.h"


void tx_assert_handler(const char* file,int line,const char* expr);

#if(TX_DEBUG_ENABLE)
#define ASSERT(expr)   \
        do{\
            if(!(expr))\
            {\
                tx_assert_handler(__FILE__,__LINE__,#expr);\
            }\
        }while(0)     

#else   
#define ASSERT(expr)            ((void)0)
#endif
   