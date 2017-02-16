#ifndef ARCHY_ANY_OBJECT_CONAINER
#define ARCHY_ANY_OBJECT_CONAINER

#ifdef _MBCS
#pragma warning( disable : 4786)
#endif

#include <list>
#include <vector>
#include <map>
#include "archy_config.hpp"

// 指针列表和指针数组, 会在析构时自动删除指针指向的对象

USING_STD_NAMESPACE;

namespace archy {

template< class T>
class ptr_list:public list<T*>
{
public:
	void clear(){
#if defined(__hpux) || defined(__sun__)
		typename
#endif
	 	list<T*>::iterator it;
#ifdef __hpux
		for( it = list<T*>::begin(); it != list<T*>::end(); ++it ) 
#else
		for( it = begin(); it != end(); ++it ) 
#endif
		{
	 		if( *it != 0 )
	 			delete (*it);
	 	}
		list<T*>::clear();
	}
	~ptr_list() {
		ptr_list<T>::clear();
	 }

};

template< class T>
class ptr_vector:public vector<T*>
{
public:
	void clear() {
#if defined(__hpux) || defined(__sun__)
		typename
#endif
	 	vector<T*>::iterator it;
#ifdef __hpux
		for( it = vector<T*>::begin(); it != vector<T*>::end(); ++it ) 
#else
		for( it = begin(); it != end(); ++it ) 
#endif
		{
	 		if( *it != 0 )
	 			delete (*it);
	 	}
		vector<T*>::clear();
	}
		
	~ptr_vector() {
		ptr_vector<T>::clear();
	 }
};

template< class T, class V>
class ptr_map:public map<T, V*>
{
public:
	void clear() {
#if defined(__hpux) || defined(__sun__)
		typename
#endif
	 	map<T, V*>::iterator it;
#ifdef __hpux
		for( it = map<T, V*>::begin(); it != map<T, V*>::end(); ++it ) 
#else
			for( it = begin(); it != end(); ++it ) 
#endif
		{
			
	 		if( it->second != 0 )
	 			delete (it->second);
	 	}
		map<T,V*>::clear();
	}

	~ptr_map() {
		ptr_map<T,V>::clear();
	 }
};

/*
对于模板中参数定义为模板，一些编译器支持的很差
所以改用上面两个实现
template<template<class> class ContainerType>
class ptr_container:public ContainerType<any_object*>
{
public:
	void clear() {
	 	typename ContainerType<any_object*>::iterator it;
	 	for( it = begin(); it != end(); ++it ) {
	 		if( *it != 0 )
	 			delete (*it);
	 	}
		ContainerType<any_object*>::clear();
	}

	~ptr_container() {
		ptr_container<ContainerType>::clear();
	 }
};
*/
}


#endif // ARCHY_ANY_OBJECT_CONAINER

