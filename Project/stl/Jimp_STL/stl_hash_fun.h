// Created by Jimp on 2021/8/3.

#ifndef JIMP_STL__STL_HASH_FUN_H
#define JIMP_STL__STL_HASH_FUN_H

#include <cstddef>
#include "stl_config.h"

__STL_BEGIN_NAMESPACE

template <class _Key> struct hash { };

inline size_t __stl_hash_string(const char* __s)
{
	unsigned long __h = 0;
	for ( ; *__s; ++__s)
		__h = 5 * __h + *__s;

	return size_t(__h);
}

__STL_TEMPLATE_NULL struct hash<char*>
{
	size_t operator()(const char* __s) const { return __stl_hash_string(__s); }
};

__STL_TEMPLATE_NULL struct hash<const char*>
{
	size_t operator()(const char* __s) const { return __stl_hash_string(__s); }
};

__STL_TEMPLATE_NULL struct hash<char>
{
	size_t operator()(char __x) const { return __x; }
};

__STL_TEMPLATE_NULL struct hash<unsigned char>
{
	size_t operator()(unsigned char __x) const { return __x; }
};

__STL_TEMPLATE_NULL struct hash<signed char>
{
	size_t operator()(unsigned char __x) const { return __x; }
};

__STL_TEMPLATE_NULL struct hash<short>
{
	size_t operator()(short __x) const { return __x; }
};

__STL_TEMPLATE_NULL struct hash<unsigned short>
{
	size_t operator()(unsigned short __x) const { return __x; }
};

__STL_TEMPLATE_NULL struct hash<int>
{
	size_t operator()(int __x) const { return __x; }
};

__STL_TEMPLATE_NULL struct hash<unsigned int>
{
	size_t operator()(unsigned int __x) const { return __x; }
};

__STL_TEMPLATE_NULL struct hash<long>
{
	size_t operator()(long __x) const { return __x; }
};

__STL_TEMPLATE_NULL struct hash<unsigned long>
{
	size_t operator()(unsigned long __x) const { return __x; }
};

__STL_END_NAMESPACE

#endif//JIMP_STL__STL_HASH_FUN_H