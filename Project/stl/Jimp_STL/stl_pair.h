// Created by Jimp on 2021/8/3.

#ifndef JIMP_STL__STL_PAIR_H
#define JIMP_STL__STL_PAIR_H

#include "stl_config.h"

__STL_BEGIN_NAMESPACE

template <class _T1, class _T2>
struct pair {
	typedef _T1 first_type;
	typedef _T2 second_type;

	_T1 first;
	_T2 second;
	pair() : first(_T1()), second(_T2()) {}
	pair(const _T1& __a, const _T2& __b) : first(__a), second(__b) {}
};

template <class _T1, class _T2>
inline bool operator==(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
	return __x.first == __y.first && __x.second == __y.second;
}

template <class _T1, class _T2>
inline bool operator<(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
	return __x.first < __y.first ||
		(!(__y.first < __x.first) && __x.second < __y.second);
}

template <class _T1, class _T2>
inline bool operator!=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
	return !(__x == __y);
}

template <class _T1, class _T2>
inline bool operator>(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
	return __y < __x;
}

template <class _T1, class _T2>
inline bool operator<=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
	return !(__y < __x);
}

template <class _T1, class _T2>
inline bool operator>=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
	return !(__x < __y);
}

template <class _T1, class _T2>
inline pair<_T1, _T2> make_pair(const _T1& __x, const _T2& __y)
{
	return pair<_T1, _T2>(__x, __y);
}

__STL_END_NAMESPACE

#endif//JIMP_STL__STL_PAIR_H
