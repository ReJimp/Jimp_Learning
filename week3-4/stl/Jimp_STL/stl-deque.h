// Created by Jimp on 2021/7/25.

#ifndef JIMP_STL__STL_DEQUE_H
#define JIMP_STL__STL_DEQUE_H

#include "stl-config.h"

__STL_BEGIN_NAMESPACE

inline size_t __deque_buf_size(size_t __size) {
	return __size < 512 ? size_t(512 / __size) : size_t(1);
}

template<class _Tp, class _Ref, class _Ptr>
struct _Deque_iterator {
	typedef _Deque_iterator<_Tp, _Tp&, _Tp*>             iterator;
	typedef _Deque_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;
	static size_t _S_buffer_size() { return __deque_buf_size(sizeof(_Tp)); }

	typedef random_access_iterator_tag iterator_category;
	typedef _Tp value_type;
	typedef _Ptr pointer;
	typedef _Ref reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef _Tp** _Map_pointer;

	typedef _Deque_iterator _Self;

	_Tp* _M_cur;
	_Tp* _M_first;
	_Tp* _M_last;
	_Map_pointer _M_node;

	_Deque_iterator(_Tp* __x, _Map_pointer __y)
		:_M_cur(__x), _M_first(*__y),
		  _M_last(*__y + _S_buffer_size()), _M_node(__y) {}
	_Deque_iterator() : _M_cur(0), _M_first(0), _M_last(0), _M_node(0) {}
	_Deque_iterator(const iterator& __x)
		: _M_cur(__x._M_cur), _M_first(__x._M_first),
		  _M_last(__x._M_last), _M_node(__x._M_node) {}

	reference operator*() const { return *_M_cur; }
	pointer operator->() const { return _M_cur; }

	difference_type operator-(const _Self& __x) const {
		return difference_type(_S_buffer_size()) * (_M_node - __x._M_node - 1) +
			(_M_cur - _M_first) + (__x._M_first - __x._M_cur);
	}

	_Self operator++() {
		++_M_cur;
		if(_M_cur == _M_last) {
			_M_set_node(_M_node + 1);
			_M_cur = _M_first;
		}
		return *this;
	}
	_Self operator++(int) {
		_Self __tmp = *this;
		++*this;
		return __tmp;
	}

	_Self& operator--() {
		if(_M_cur == _M_first) {
			_M_set_node(_M_node - 1);
			_M_cur = _M_last;
		}
		--_M_cur;
		return *this;
	}
	_Self operator--(int) {
		_Self __tmp = *this;
		--*this;
		return __tmp;
	}

	_Self& operator+=(difference_type __n)
	{
		difference_type __offset = __n + (_M_cur - _M_first);
		if(__offset >= 0 && __offset < difference_type(_S_buffer_size()))
			_M_cur += __n;
		else {
			difference_type __node_offset =
				__offset > 0 ? __offset / difference_type(_S_buffer_size())
						 : -difference_type((-__offset - 1) / _S_buffer_size()) - 1;
			_M_set_node(_M_node + __node_offset);
			_M_cur = _M_first +
				(__offset - __node_offset * difference_type(_S_buffer_size()));
		}
		return *this;
	}

	_Self operator+(difference_type __n) const
	{
		_Self __tmp = *this;
		return __tmp += __n;
	}

	_Self operator-=(difference_type __n) { return *this += -__n; }

	_Self operator-(difference_type __n) const {
		_Self __tmp = *this;
		return __tmp -= __n;
	}

	reference operator[](difference_type __n) const {return *(*this + __n); }

	bool operator==(const _Self& __x) const { return _M_cur == __x._M_cur; }
	bool operator!=(const _Self& __x) const { return !(*this == __x); }
	bool operator<(const _Self& __x) const {
		return (_M_node == __x._M_node) ?
			   (_M_cur < __x._M_cur) : (_M_node < __x._M_node);
	}
	bool operator>(const _Self& __x) const { return __x < *this; }
	bool operator<=(const _Self& __x) const { return !(__x < *this); }
	bool operator>=(const _Self& __x) const { return !(*this < __x); }

	void _M_set_node(_Map_pointer __new_node) {
		_M_node = __new_node;
		_M_first = *__new_node;
		_M_last = _M_first + difference_type(_S_buffer_size());
	}

};

template <class _Tp, class _Ref, class _Ptr>
inline _Deque_iterator<_Tp, _Ref, _Ptr>
operator+(ptrdiff_t __n, const _Deque_iterator<_Tp, _Ref, _Ptr>& _-x)
{
	return __x + __n;
}

template <class _Tp, class _Alloc>
class _Deque_base {
public:
	typedef _Deque_iterator<_Tp, _Tp&, _Tp*>             iterator;
	typedef _Deque_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;

	typedef _Alloc allocator_type;
	allocator_type get_allocator() const { return allocator_type(); }

	_Deque_base(const allocator_type&, size_t __num_elements)
		: _M_map(0), _M_map_size(0), _M_start(), _M_finish() {
		_M_initialize_map(__num_elements);
	}
	_Deque_base(const allocator_type&)
		: _M_map(0), _M_map_size(0), _M_start(), _M_finish() {}
	~_Deque_base();

protected:
	void _M_initialize_map(size_t);
	void _M_create_nodes(_Tp** __nstart, _Tp** __nfinish);
	void _M_destroy_nodes(_Tp** __nstart, _Tp** __nfinish);
	enum { _S_initial_map_size = 8; };

protected:
	_Tp** _M_map;
	size_t _M_map_size;
	iterator _M_start;
	iterator _M_finish;

	typedef simple_alloc<_Tp, _Alloc> _Node_alloc_type;
	typedef simple_alloc<_Tp*, _Alloc> _Map_alloc_type;

	_Tp* _M_allocate_node()
	{
		return _Node_alloc_type::allocate(__deque_buf_size(sizeof(_Tp)));
	}
	void _M_deallocate_node(_Tp* __p)
	{
		_Node_alloc_type::deallocate(__p, __deque_buf_size(sizeof(_Tp)));
	}
	_Tp** _M_allocate_map(size_t __n)
	{
		return _Map_alloc_type::allocate(__n);
	}
	void _M_deallocate_map(_Tp** __p, size_t __n)
	{
		_Map_alloc_type::deallocate(__p, __n);
	}

};

template <class _Tp, class _Alloc>
_Deque_base<_Tp, _Alloc>::~_Deque_base()
{
	if(_M_map) {
		_M_destroy_nodes(_M_start._M_node, _M_finish._M_node + 1);
		_M_deallocate_map(_M_map, _M_map_size);
	}
}

template<class _Tp, class _Alloc>
void
_Deque_base<_Tp, _Alloc>::_M_initialize_map(size_t __num_elements)
{
	size_t __num_nodes =
		__num_elements / __deque_buf_size(sizeof(_Tp)) + 1;

	_M_map_size = max((size_t) _S_initial_map_size, __num_nodes + 2);
	_M_map = _M_allocate_map(_M_map_size);

	_Tp** __nstart = _M_map + (_M_map_size - __num_nodes) / 2;
	_Tp** __nfinish = __nstart + __num_nodes;

	__STL_TRY {
		_M_create_nodes(__nstart, __nfinish);
	}
	__STL_UNWIND((_M_deallocate_map(_M_map, _M_map_size),
				        _M_map = 0, _M_map_size = 0));
	_M_start._M_set_node(__nstart);
	_M_finish._M_set_node(__nfinish - 1);
	_M_start._M_cur = _M_start._M_first;
	_M_finish._M_cur = _M_start._M_first +
		__num_elements % __deque_buf_size(sizeof(_Tp));
}

template <class _Tp, class _Alloc>
void _Deque_base<_Tp, _Alloc>::_M_create_nodes(_Tp** __nstart, _Tp** __nfinish)
{
	_Tp** __cur;
	__STL_TRY {
		for(__cur = __nstart; __cur < __nfinish; ++__cur)
			*__cur = _M_allocate_node();
	}
	__STL_UNWIND(_M_destroy_nodes(__nstart, __cur));
}

template<class _Tp, class _Alloc>
void
_Deque_base<_Tp, _Alloc>::_M_destroy_nodes(_Tp** __nstart, _Tp** __nfinish)
{
	for(_Tp** __n = __nstart; __n < __nfinish; ++__n)
		_M_deallocate_node(*__n);
}



__STL_END_NAMESPACE

#endif//JIMP_STL__STL_DEQUE_H