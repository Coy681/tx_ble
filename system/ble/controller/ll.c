#include"ll.h"


/*********************************ll feature implementation**********************************/
static _u64 ll_host_support_feature;

_u64 ll_get_feature(void)
{
	return (LL_FEATURE_SUPPORT|ll_host_support_feature);
}

void ll_feature_reset(void)
{
	ll_host_support_feature = 0;
}
/******************************************ll reset*******************************************/
controller_error_code_e ll_reset(void)
{
	//ll feature reset
	ll_feature_reset();

    //ll reset process
    return SUCCESS;
}
