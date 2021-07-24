// Created by Jimp on 2021/7/24.

#ifndef JIMP_STL__STL_VECTOR_H
#define JIMP_STL__STL_VECTOR_H

#include "stl-config.h"

__STL_BEGIN_NAMESPACE

template <class _Tp, class _Alloc>
class _Vector_base {
public:
	typedef _Alloc allocator_type;
	allocator_type get_allocator() const { return allocator_type(); }

	_Vector_base(const _Alloc&)
		: _M_start(0), _M_finish(0), _M_end_of_storage(0) {}
	_Vector_base(size_t __n, const _Alloc&)
		: _M_start(0), _M_finish(0), _M_end_of_storage(0)
	{
		_M_start = _M_allocate(__n);
		_M_finish = _M_start;
		_M_end_of_storage = _M_start + __n;
	}

	~_Vector_base() { _M_deallocate(_M_start, _M_end_of_storage - _M_start); }

protected:
	_Tp* _M_start;
	_Tp* _M_finish;
	_Tp* _M_end_of_storage;

	typedef simple_alloc<_Tp, _Alloc> _M_data_allocator;
	_Tp* _M_allocate(size_t __n) {
		return _M_data_allocator::allocate(__n);
	}
	void _M_deallocate(_Tp* __p, size_t __n) {
		_M_data_allocator::deallocate(__p, __n);
	}
};

template <class _Tp, class _Alloc = alloc>
class vector : protected _Vector_base<_Tp, _Alloc>
{
private:
	typedef _Vector_base<_Tp, _Alloc> _Base;

public:
	typedef _Tp value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef typename _Base::allocator_type allocator_type;
	allocator_type get_allocator() const { return _Base::get_allocator(); }

	typedef reverse_iterator<const_iterator> const_reverse_iterator;
	typedef reverse_iterator<iterator> reverse_iterator;

protected:
	using _Base::_M_allocate;
	using _Base::_M_deallocate;
	using _Base::_M_start;
	using _Base::_M_finish;
	using _Base::_M_end_of_storage;

protected:
	void _M_insert_aux(iterator __position, const _Tp& __x);
	void _M_insert_aux(iterator __position);

public:
	iterator begin() {
		return _M_start;
	}
	const_iterator begin() {
		return _M_start;
	}
	iterator end() {
		return _M_finish;
	}
	const_iterator end() {
		return _M_finish;
	}

	reverse_iterator rbegin() {
		return reverse_iterator(end());
	}
	const_reverse_iterator rbegin() const {
		return const_reverse_iterator(end());
	}
	reverse_iterator rend() {
		return reverse_iterator(begin());
	}
	const_reverse_iterator rend() const {
		return const_reverse_iterator(begin());
	}

	size_type size() const {
		return size_type(end() - begin());
	}
	size_type max_size() const {
		return size_type(-1) / sizeof(_Tp);
	}
	size_type capacity() const {
		return size_type(_M_end_of_storage - begin());
	}
	bool empty() const {
		return begin() == end();
	}

	reference operator[](size_type __n) {
		return *(begin() + __n);
	}
	const_reference operator[](size_type __n) const {
		return *(begin() + __n);
	}

	explicit vector(const allocator_type& __a = allocator_type())
		: _Base(__a) {}

	vector(size_type __n, const _Tp& __value,
		   const allocator_type& __a = allocator_type())
		: _Base(__n, __a)
	{
		_M_finish = uninitialized_fill_n(_M_start, __n, __value);
	}

	explicit vector(size_type __n)
		: _Base(__n, allocator_type())
	{
		_M_finish = uninitialized_fill_n(_M_start, __n, _Tp());
	}

	vector(const vector<_Tp, _Alloc>& __x)
		: _Base(__x.size(), __x.get_allocator())
	{
		_M_finish = uninitialized_copy(__x.begin(), __x.end(), _M_start);
	}

	vector(const _Tp* __first, const _Tp* __last,
		   const allocator_type& __a = allocator_type())
		: _Base(__last - __first, __a)
	{
		_M_finish = uninitialized_copy(__first, __last, _M_start);
	}

	~vector() { destroy(_M_start, _M_finish); }

	vector<_Tp, _Alloc>& operator=(const vector<_Tp, _Alloc>& __x);
	void reserve(size_type __n) {
		if(capacity() < __n) {
			const size_type __old_size = size();
			iterator __tmp = _M_allocate_and_copy(__n, _M_start, _M_finish);
			destroy(_M_start, _M_finish);
			_M_deallocate(_M_start, _M_end_of_storage - _M_start);
			_M_start = __tmp;
			_M_finish = __tmp + __old_size;
			_M_end_of_storage = _M_start + __n;
		}
	}

	void assign(size_type __n, const _Tp& __val) { _M_fill_assign(__n, __val); }
	void _M_fill_assign(size_type __n, const _Tp& __val);

	reference front() { return *begin(); }
	const_reference front() const { return *begin(); }
	reference back() { return *(end() - 1); }
	const_reference back() const { return *(end() - 1); }

	void push_back(const _Tp& __x) {
		if(_M_finish != _M_end_of_storage) {
			construct(_M_finish, __x);
			++_M_finish;
		}
		else
			_M_insert_aux(end(), __x);
	}
	void push_back() {
		if(_M_finish !=_M_end_of_storage) {
			construct(_M_finish);
			++_M_finish;
		}
		else
			_M_insert_aux(end());
	}
	void swap
};


__STL_END_NAMESPACE

#endif//JIMP_STL__STL_VECTOR_H
