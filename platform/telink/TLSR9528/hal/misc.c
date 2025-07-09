#include"../../hal/rf.h"
#include"../../hal/stimer.h"
#include"common/txCommon.h"

//todo,generate random number

_u8  random_byte(void)
{
    return 0x55;
}

_u16 random_hword(void)
{
    return 0x5555;
}

_u32 random_word(void)
{
    return 0x55555555;
}