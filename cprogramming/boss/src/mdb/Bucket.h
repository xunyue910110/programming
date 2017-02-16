// ############################################
// Source file : Bucket.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-4-23
// Update      : 2008-4-23
// Copyright(C): chen min, Linkage.
// ############################################

#ifndef BUCKET_H_INCLUDED
#define BUCKET_H_INCLUDED

class ShmPosition;  //伪地址=共享内存标示+偏移量

//定义hash map 的一个node 结构
class Bucket 
{
	public:
		ShmPosition m_data;
		ShmPosition m_diffNext;//不同的key的链表 纵链表
		ShmPosition m_sameNext;//相同的key的链表 横链表
}; 

// 哈希函数
class HashCode
{
	public:
		static unsigned hash(const char *input,int iLenth)
		{
			unsigned seed=0;
			char *key; 
			key = (char *)input;
			for(int i=0;i<iLenth;++i)
			{
				seed ^= int(*(key+i)) + 0x9e3779b9 + (seed<<6) + (seed>>2);
			}
			
			return seed;
		}
};

#endif //BUCKET_H_INCLUDED
