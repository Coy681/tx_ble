
#include"system.h"
#include"system/ble/controller/controller.h"

void system_init()
{
	tx_malloc_init();//must init first
	ble_controller_init();
}

ARCH_INIT(system_init);


// #define TX_BIT_SET(val,bit)      ((val)|=(1U<<(bit)))

// #define TX_BIT_CLEAR(val,bit)    ((val)&=(~(1U<<bit)))

// #define TX_BIT_GET(val,bit)      (((val)>>(bit))&1U)

// #define TX_BIT_REVERSE(val,bit)  ((val)^=(1U<<(bit)))

// typedef struct tx_listNode_t
// {
// 	int                   data;
// 	struct tx_listNode_t* next;
// }tx_listNode_t;

// /**
//  *      ^   ^   ^   ^   ^
//  *      p   c   n
//  *          p   c   n
//  */
// tx_listNode_t* tx_list_node_reverse(tx_listNode_t* list)
// {
// 	ASSERT(list!=NULL);
// 	tx_listNode_t* pPrevious = NULL;
// 	tx_listNode_t* pCur      = list;
// 	tx_listNode_t* pNext     = NULL;
// 	while(pCur!=NULL)
// 	{
// 		pNext->next = pCur->next;
// 		pCur->next  = pPrevious;
// 		pPrevious   = pCur;
// 		pCur        = pNext;
// 	}
//     return pCur;
// }

// tx_listNode_t* tx_list_find_kTh_from_end(tx_listNode_t* list,_u32 k)
// {
// 	if(list!=NULL||k==0)
// 	{
// 		return NULL;
// 	}
// 	tx_listNode_t* pFast = list;
// 	tx_listNode_t* pSlow = list;
// 	_u32 step = 0;
// 	while(step<k)
// 	{
// 		pFast = pFast->next;
// 		if(pFast == NULL)
// 		{
// 			return NULL;
// 		}
// 		step++;
// 	}
// 	while(pFast!=NULL)
// 	{
// 		pSlow = pSlow->next;
// 		pFast = pFast->next;
// 	}
// 	return pSlow;
// }

// tx_listNode_t* tx_list_delete_node_by_val(tx_listNode_t* list,int value)
// {
// 	if(list == NULL)
// 	{
// 		return NULL;
// 	}

// 	tx_listNode_t* pDelete = NULL;
// 	tx_listNode_t* pTemp = (tx_listNode_t*)tx_malloc(sizeof(tx_listNode_t));
// 	ASSERT(pTemp!=NULL);
// 	pTemp->next = list;
// 	tx_listNode_t* pCur = pTemp;
// 	while(pCur->next!=NULL)
// 	{
// 		if(pCur->next->data == value)
// 		{
// 			pDelete = pCur->next;
// 			pCur->next = pCur->next->next;
// 			tx_free(pDelete);
// 		}
// 		else
// 		{
// 			pCur = pCur->next;
// 		}
// 	}
// 	tx_listNode_t* pNew = pTemp->next;
// 	tx_free(pTemp);
// 	return pNew;
// }
// /**
//  * return 1:cycle
//  * returb 0:no cycle
//  */
// int tx_list_is_cycle(tx_listNode_t* list)
// {
// 	if(list == NULL || list->next == NULL)
// 	{
// 		return 0;
// 	}	
// 	tx_listNode_t* fast = list->next;
// 	tx_listNode_t* slow = list;
// 	while(slow!=fast)
// 	{
// 		if(fast == NULL||fast->next == NULL)
// 		{
// 			return 0;
// 		}
// 		slow = slow->next;
// 		fast = fast->next->next;
// 	}
// 	return 1;
// }

// tx_listNode_t* tx_list_merge_sorted(tx_listNode_t* list1,tx_listNode_t* list2)
// {
// 	tx_listNode_t* pNew = (tx_listNode_t*)tx_malloc(sizeof(tx_listNode_t));
// 	ASSERT(pNew!=NULL);
// 	tx_listNode_t* pCur = pNew;
// 	while(list1!=NULL&&list2!=NULL)
// 	{
// 		if(list1->data >list2->data)
// 		{
// 			pCur->next = list1;
// 			list1 = list1->next;
// 		}
// 		else
// 		{
// 			pCur->next = list2;
// 			list2 = list2->next;
// 		}
// 		pCur = pCur->next;
// 	}
// 	pCur->next = (list1!=NULL)?list1:list2;
// 	tx_listNode_t* pRet = pNew->next;
// 	tx_free(pNew);
// 	return pRet;
// }

// void tx_memcpy(_u8* dest,_u8* src,_u32 len)
// {
// 	if(dest == NULL || src == NULL)
// 	{
// 		return;
// 	}
// 	_u8* pD = dest;
// 	_u8* pS = src;
// 	while(len--)
// 	{
// 		*dest++ = *src++;
// 	}
// }

// void tx_memcpy(_u8* dest,_u8* src,_u32 len)
// {
// 	if(dest == NULL || src == NULL||len == 0)
// 	{
// 		return;
// 	}
// 	_u8* pDest = dest;
// 	_u8* pSrc  = src;
// 	if(dest>src&&dest<(src+len))
// 	{
// 		dest+=(len-1);
// 		src +=(len-1);
// 		while(len--)
// 		{
// 			*pDest-- = *pSrc--;
// 		}
// 	}
// 	else
// 	{
// 		while(len--)
// 		{
// 			*pDest++ = *pSrc++;
// 		}
// 	}
// }

// void tx_sort(int data[],int len)
// {
// 	if(data == NULL || len == 0)
// 	{
// 		return;
// 	}
// 	for(int i=0;i<len-1;i++)
// 	{
// 		for(int j=0;j<len-i-1;j++)
// 		{
// 			if(data[j]>data[j+1])
// 			{
// 				data[j]^=data[j+1];
// 				data[j+1]^=data[j];
// 				data[j]^=data[j+1];
// 			}
// 		}
// 	}
// }
// void tx_fast_sort(int data[],int len)
// {
// 	if(data == NULL || len == 0)
// 	{
// 		return;
// 	}
	
// }
// void tx_string_reverse(_s8*s,_u32 size)
// {
// 	_u32 l = 0,r = size-1;
// 	while(l<r)
// 	{	
// 		_s8 temp = s[l];
// 		s[l++] = s[r];
// 		s[r--] = temp;
// 	}
// }