#include"ll.h"

static _u64 ll_host_support_feature;


controller_error_code_e ll_reset(void)
{
    //ll reset process
    return SUCCESS;
}

_u64 ll_get_feature(void)
{
	return (LL_FEATURE_SUPPORT|ll_host_support_feature);
}

