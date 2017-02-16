#include "ParamHelper.h"

#include <string>

using std::string;


namespace {
	/**
	 *  二分法查找
	 */
	
	// 1 值
	inline bool _ymequal1(const TParRecord& lhs, const TParRecord& rhs) 
	{
		if (strcmp(lhs.value1, rhs.value1) == 0)
			return true;
		else
			return false;
	}
	inline bool _ymless1(const TParRecord& lhs, const TParRecord& rhs) 
	{
		if (strcmp(lhs.value1, rhs.value1) < 0)
			return true;
		else 
			return false;
	}
	
	// 2 值
	inline bool _ymequal2(const TParRecord& lhs, const TParRecord& rhs) 
	{
		if ( (strcmp(lhs.value1, rhs.value1) == 0)
			&& (strcmp(lhs.value2, rhs.value2) == 0) )
			return true;
		else
			return false;
	}
	inline bool _ymless2(const TParRecord& lhs, const TParRecord& rhs) 
	{
		if (strcmp(lhs.value1, rhs.value1) < 0)
			return true;
		else if ( (strcmp(lhs.value1, rhs.value1) == 0)
				 && (strcmp(lhs.value2, rhs.value2) < 0) )
			return true;
		else 
			return false;
	}
	
	// 3 值
	inline bool _ymequal3(const TParRecord& lhs, const TParRecord& rhs) 
	{
		if ( (strcmp(lhs.value1, rhs.value1) == 0)
			&& (strcmp(lhs.value2, rhs.value2) == 0)
			&& (strcmp(lhs.value3, rhs.value3) == 0) )
			return true;
		else
			return false;
	}
	inline bool _ymless3(const TParRecord& lhs, const TParRecord& rhs) 
	{
		if (strcmp(lhs.value1, rhs.value1) < 0)
		{
			return true;
		}
		else if (strcmp(lhs.value1, rhs.value1) == 0)
		{
			if (strcmp(lhs.value2, rhs.value2) < 0)
			{
				return true;
			}
			else if ( (strcmp(lhs.value2, rhs.value2) == 0)
					&& (strcmp(lhs.value3, rhs.value3) < 0) )
			{
					return true;
			}
			else
			{
				return false;
			}
		}
		else 
		{
			return false;
		}
	}
	
	/**
	 * 用于快速速排序(三值排序，因为三值都有值)
	 */
	inline bool _ymlessp(TParRecord* lhs, TParRecord* rhs) 
	{
		if (strcmp(lhs->value1, rhs->value1) < 0)
		{
			return true;
		}
		else if (strcmp(lhs->value1, rhs->value1) == 0)
		{
			if (strcmp(lhs->value2, rhs->value2) < 0)
			{
				return true;
			}
			else if ( (strcmp(lhs->value2, rhs->value2) == 0)
					&& (strcmp(lhs->value3, rhs->value3) < 0) )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else 
		{
			return false;
		}
	}

} // unnamed namespace


/**
 * 快速排序算法
 */
void _ParamHelper::quicksort(TParRecord* _hrec, int left, int right)
{
	int i = left, j = right;
	
	//求中间值
	TParRecord _middle, _tempRecord;
	memset(&_middle, 0, sizeof(TParRecord));
	memcpy(&_middle, _hrec+(left+right)/2, sizeof(TParRecord));
	
	do {
		//从左扫描大于等于中值的数
		while((_ymlessp(_hrec+i, &_middle)) && (i<right))
			i++;     

		//从右扫描小于等于中值的数
		while((_ymlessp(&_middle, _hrec+j)) && (j>left))
			j--;

		if (i==j)
		{
			i++;
			j--;
			break;
		}
        if (i<j) //找到了一对值
		{
			//交换
			memset(&_tempRecord, 0, sizeof(TParRecord));
			
			memcpy(&_tempRecord, _hrec+i, sizeof(TParRecord));
			memcpy(_hrec+i, _hrec+j, sizeof(TParRecord));
			memcpy(_hrec+j, &_tempRecord, sizeof(TParRecord));
			
			i++;
			j--;
		}
	} while(i<=j);//如果两边扫描的下标交错，就停止（完成一次）

	//当左边部分有值(left<j)，递归左半边
	if(left<j)
		quicksort(_hrec,left,j);

	//当右边部分有值(right>i)，递归右半边
	if(right>i)
		quicksort(_hrec,i,right);
}

/**
 * 二分法查找算法
 */
int _ParamHelper::binsrch(TParRecord* _hrec, int size, TParRecord& parrec)
{
	typedef bool (*COMPFPTR)(const TParRecord&, const TParRecord&);

	COMPFPTR fequal=_ymequal3, fless=_ymless3;
	if (strcmp(parrec.value2, NULL_VALUE2)==0)
	{
		fequal = _ymequal1;
		fless = _ymless1;
	} else if (strcmp(parrec.value3, NULL_VALUE3) == 0)
	{
		fequal = _ymequal2;
		fless = _ymless2;
	}
	
	///
    int n = size;
    int low=0, high=n-1, found=0, mid=0;

    while( (low<=high) && (!found) ) 
    {
        mid = (low+high)/2;

        if ( fequal(*(_hrec+mid), parrec) ) 
        {
            strcpy(parrec.sresult, (_hrec+mid)->sresult);
            return mid;
        }
        else if ( fless(*(_hrec+mid), parrec) )
        {
            low = mid+1;   // 在后半区找
        }
        else
        {
            high = mid-1;  // 在前半区找
        }
    }

	return -1;
}

int _ParamHelper::trvsrch(TParRecord* _hrec, int size, TParRecord& parrec)
{
	typedef bool (*COMPFPTR)(const TParRecord&, const TParRecord&);

	COMPFPTR fequal=_ymequal3, fless=_ymless3;
	if (strcmp(parrec.value2, NULL_VALUE2)==0)
	{
		fequal = _ymequal1;
		fless = _ymless1;
	} else if (strcmp(parrec.value3, NULL_VALUE3) == 0)
	{
		fequal = _ymequal2;
		fless = _ymless2;
	}
	
	///
    for (int i=0; i<size; ++i)
    {
    	if ( fequal(*(_hrec+i), parrec) ) 
        {
            strcpy(parrec.sresult, (_hrec+i)->sresult);
            return i;
        }
    }
    
    return -1;
}

