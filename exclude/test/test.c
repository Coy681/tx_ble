

/****
 * example ,function pointer as return value
 * Used in object-oriented programming methods.
 * ****/
int add(int a,int b)
{
    return a+b;
}

int sub(int a,int b)
{
    return a-b;
}

int (*pTest(int flag))(int,int)
{
    if(flag == 0)
    {
    	return &add;
    }
    else if(flag == 1)
    {
    	return &sub;
    }
    else
    {
    	return 0;
    }
}

