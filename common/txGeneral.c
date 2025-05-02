#include "txGeneral.h"

_RAM_CODE _NO_INLINE
void txMemcpy(void *pO, void *pI, _u32 length)
{
	_u8 *pInput = (_u8*) pI;
	_u8 *pOuput = (_u8*) pO;
	while ((length--) > 0)
	{
		*pOuput++ = *pInput++;
	}
}

_RAM_CODE _NO_INLINE
int txMemcmp(void * str1, void * str2, _u32 length)
{
	_u8 *st1 = (_u8*)str1;
	_u8 *st2 = (_u8*)str2;

	while(length--)
    {
		if(*st1 != *st2)
        {
			return 1; 
		}
		st1++;
		st2++;
	}
	return 0;
}

_RAM_CODE _NO_INLINE
void txMemsetByte(_u8 * pO, _u8 pValue, _u32 length)
{
	_u8 *ptr = pO;
	while (length-- > 0)
    {
		*ptr++ = pValue;
    }
}

_RAM_CODE _NO_INLINE
void txMemsetWord(_u32 * pO, _u32 pValue, _u32 length)
{
	_u32 *ptr = pO;
	while (length-- > 0)
    {
		*ptr++ = pValue;
    }
}

_RAM_CODE _NO_INLINE
int txStringLength(_s8* string)
{
	_u32 length = 0;
	while(*string != '\0')
    {
		length ++;
		string ++;
	}
	return length;
}

int tick1_exceed_tick2(_u32 tick1,_u32 tick2)
{
	if((tick1-tick2) < BIT(30))
	{
		return 1;
	}
	else
	{
		return 0;
	}
} 