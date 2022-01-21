// Created by Jimp on 2021/8/3.

#ifndef JIMP_STL__STL_FUNCTION_H
#define JIMP_STL__STL_FUNCTION_H

#include "stl_config.h"

__STL_BEGIN_NAMESPACE

template <class _Arg, class _Result>
struct unary_function {
	typedef _Arg argument_type;
	typedef _Result result_type;
};

template <class _Arg1, class _Arg2, class _Result>
struct binary_function {
	typedef _Arg1 first_argument_type;
	typedef _Arg2 second_argument_type;
	typedef _Result result_type;
};

template <class _Tp>
struct plus : public binary_function<_Tp, _Tp, _Tp> {
	_Tp operator()(const _Tp& __x, const _Tp& __y) const { return __x + __y; }
};

template <class _Tp>
struct minus : public binary_function<_Tp, _Tp, _Tp> {
	_Tp operator()(const _Tp& __x, const _Tp& __y) const { return __x - __y; }
};

template <class _Tp>
struct multiplies : public binary_function<_Tp, _Tp, _Tp> {
	_Tp operator()(const _Tp& __x, const _Tp& __y) const { return __x * __y; }
};

template <class _Tp>
struct divides : public binary_function<_Tp, _Tp, _Tp> {
	_Tp operator()(const _Tp& __x, const _Tp& __y) const { return __x / __y; }
};

// identity_element (not part of the C++ standard)

template <class _Tp>
inline _Tp identity_element(plus<_Tp>)
{
	return _Tp(0);
}

template <class _Tp>
inline _Tp identity_element(multiplies<_Tp>)
{
	return _Tp(1);
}

template <class _Tp>
struct modulus : public binary_function<_Tp, _Tp, _Tp>
{
	_Tp operator()(const _Tp& __x, const _Tp& __y) const { return __x % __y; }
};

template <class _Tp>
struct negate : public unary_function<_Tp, _Tp>
{
	_Tp operator()(const _Tp& __x) const { return -__x; }
};

template <class _Tp>
struct equal_to : public binary_function<_Tp, _Tp, bool>
{
	bool operator()(const _Tp& __x, const _Tp& __y) const { return __x == __y; }
};

template <class _Tp>
struct np_equal_to : public binary_function<_Tp, _Tp, bool>
{
	bool operator()(const _Tp& __x, const _Tp& __y) const { return __x != __y; }
};

template <class _Tp>
struct greater : public binary_function<_Tp, _Tp, bool>
{
	bool operator()(const _Tp& __x, const _Tp& __y) const { return __x > __y; }
};

template <class _Tp>
struct less : public binary_function<_Tp, _Tp, bool>
{
	bool operator()(const _Tp& __x, const _Tp& __y) const { return __x < __y; }
};

template <class _Tp>
struct greater_equal : public binary_function<_Tp, _Tp, bool>
{
	bool operator()(const _Tp& __x, const _Tp& __y) const { return __x >= __y; }
};

template <class _Tp>
struct less_equal : public binary_function<_Tp, _Tp, bool>
{
	bool operator()(const _Tp& __x, const _Tp& __y) const { return __x <= __y; }
};

template <class _Tp>
struct logical_and : public binary_function<_Tp, _Tp, bool>
{
	bool operator()(const _Tp& __x, const _Tp& __y) const { return __x && __y; }
};

template <class _Tp>
struct logical_or : public binary_function<_Tp, _Tp, bool>
{
	bool operator()(const _Tp& __x, const _Tp& __y) const { return __x || __y; }
};

template <class _Tp>
struct logical_not : public unary_function<_Tp, bool>
{
	bool operator()(const _Tp& __x) const { return !__x; }
};

template <class _Predicate>
class unary_negate
	: public unary_function<typename _Predicate::argument_type, bool>
{
protected:
	_Predicate _M_pred;

public:
	explicit unary_negate(const _Predicate& __x) : _M_pred(__x) {}
	bool operator()(const typename _Predicate::argument_type& __x) const {
		return !_M_pred(__x);
	}
};

template <class _Predicate>
inline unary_negate<_Predicate>
not1(const _Predicate& __pred)
{
	return unary_negate<_Predicate>(__pred);
}

template <class _Predicate>
class binary_negate
	: public binary_function<typename _Predicate::first_argument_type,
							 typename _Predicate::second_argument_type,
							 bool>
{
protected:
	_Predicate _M_pred;

public:
	explicit binary_negate(const _Predicate& __x) : _M_pred(__x) {}
	bool operator()(const typename _Predicate::first_argument_type& __x,
			   		const typename _Predicate::second_argument_type& __y)
	{
		return !_M_pred(__x, __y);
	}
};

template <class _Predicate>
inline binary_negate<_Predicate>
not2(const _Predicate& __pred)
{
	return binary_negate<_Predicate>(__pred);
}

template <class _Operation>
class binder1st
	: public unary_function<typename _Operation::second_argument_type,
							typename _Operation::result_type>
{
protected:
	_Operation op;
	typename _Operation::first_argument_type value;

public:
	binder1st(const _Operation& __x,
			  const typename _Operation::first_argument_type& __y)
		: op(__x), value(__y) {}
	typename _Operation::result_type
	operator()(const typename _Operation::second_argument_type& __x) const
	{
		return op(value, __x);
	}
};

template <class _Operation, class _Tp>
inline binder1st<_Operation>
bind1st(const _Operation& __fn, const _Tp& __x)
{
	typedef typename _Operation::first_argument_type _Arg1_type;
	return binder1st<_Operation>(__fn, _Arg1_type(__x));
}

template <class _Operation>
class binder2nd
	: public unary_function<typename _Operation::first_argument_type,
							typename _Operation::result_type>
{
protected:
	_Operation op;
	typename _Operation::second_argument_type value;

public:
	binder2nd(const _Operation& __x,
			  const typename _Operation::second_argument_type& __y)
		: op(__x), value(__y) {}
	typename _Operation::result_type
	operator()(const typename _Operation::first_argument_type& __x) const
	{
		return op(__x, value);
	}
};

template <class _Operation, class _Tp>
inline binder2nd<_Operation>
bind2nd(const _Operation& __fn, const _Tp& __x)
{
	typedef typename _Operation::second_argument_type _Arg2_type;
	return binder2nd<_Operation>(__fn, _Arg2_type(__x));
}



__STL_END_NAMESPACE

#endif//JIMP_STL__STL_FUNCTION_H