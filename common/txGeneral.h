
#include "txAttribute.h"
#include "txType.h"

void txMemcpy(void *pO, void *pI, _u32 length);

int  txMemcmp(void * str1, void * str2, _u32 length);

void txMemsetByte(_u8 * pO, _u8 pValue, _u32 length);

void txMemsetWord(_u32 * pO, _u32 pValue, _u32 length);

int  txStringLength(_s8* string);
