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

template<class _Tp, class _Alloc = alloc>
class deque : protected _Deque_base<_Tp, _Alloc> {
	typedef _Deque_base<_Tp, _Alloc> _Base;

public:
	typedef _Tp value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef typename _Base::allocator_type allocator_type;
	allocator_type get_allocator() const { return _Base::get_allocator(); }

public:
	typedef typename _Base::iterator       iterator;
	typedef typename _Base::const_iterator const_iterator;

	typedef reverse_iterator<const_iterator> const_reverse_iterator;
	typedef reverse_iterator<iterator> reverse_iterator;

protected:
	typedef pointer* _Map_pointer;
	static size_t _S_buffer_size() { return __deque_buf_size(sizeof(_Tp)); }

protected:
	using _Base::_M_initialize_map;
	using _Base::_M_create_nodes;
	using _Base::_M_destroy_nodes;
	using _Base::_M_allocate_node;
	using _Base::_M_deallocate_node;
	using _Base::_M_allocate_map;
	using _Base::_M_deallocate_map;

	using _Base::_M_map;
	using _Base::_M_map_size;
	using _Base::_M_start;
	using _Base::_M_finish;

public:
	iterator begin() { return _M_start; }
	iterator end() { return _M_finish; }
	const_iterator begin() const { return _M_start; }
	const_iterator end() const { return _M_finish; }

	reverse_iterator rbegin() { return reverse_iterator(_M_finish); }
	reverse_iterator rend() { return reverse_iterator(_M_start); }
	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(_M_finish);
	}
	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(_M_start);
	}

	reference operator[](size_type __n)
	{
		return _M_start[difference_type(__n)];
	}
	const_reference operator[](size_type __n) const
	{
		return _M_start[difference_type(__n)];
	}

	reference front() { return *_M_start; }
	reference back() {
		iterator __tmp = _M_finish;
		--__tmp;
		return *__tmp;
	}
	const_reference front() const { return *_M_start; }
	const_reference back() const {
		const_iterator __tmp = _M_finish;
		--__tmp;
		return *__tmp;
	}

	size_type size() const { return _M_finish - _M_start;}
	size_type max_size() const { return size_type(-1); }
	bool empty() const { return _M_finish == _M_start; }

public:
	explicit deque(const allocator_type& __a = allocator_type())
		: _Base(__a, 0) {}
	deque(const deque& __x) : _Base(__x.get_allocator(), __x.size())
	{
		uninitialized_copy(__x.begin(), __x.end(), _M_start);
	}
	deque(size_type __n, const value_type& __value,
		  const allocator_type& __a = allocator_type()) : _Base(__a, __n)
	{
		_M_fill_initialize(__value);
	}
	explicit deque(size_type __n) : _Base(allocator_type(), __n)
	{
		_M_fill_initialize(value_type());
	}

	deque(const value_type* __first, const value_type* __last,
		  const allocator_type& __a = allocator_type())
		: _Base(__a, __last - __first)
	{
		uninitialized_copy(__first, __last, _M_start);
	}
	deque(const_iterator __first, const_iterator __last,
		  const allocator_type& __a = allocator_type())
		: _Base(__a, __last - __first)
	{
		uninitialized_copy(__first, __last, _M_start);
	}

	~deque() { destroy(_M_start, _M_finish); }

	deque& operator= (const deque& __x) {
		const size_type __len = size();
		if(&__x != this) {
			if(__len >= __x.size())
				erase(copy(__x.begin(), __x.end(), _M_start), _M_finish);
			else {
				const_iterator __mid = __x.begin() + difference_type(__len);
				copy(__x.begin(), __mid, _M_start);
				insert(_M_finish, __mid, __x.end());
			}
		}
		return *this;
	}

	void swap(deque& __x) {
		Jimp_STL::swap(_M_start, __x._M_start);
		Jimp_STL::swap(_M_finish, __x._M_finish);
		Jimp_STL::swap(_M_map, __x._M_map);
		Jimp_STL::swap(_M_map_size, __x._M_map_size);
	}

public:
	void _M_fill_assign(size_type __n, const _Tp& __val) {
		if(__n > size()) {
			fill(begin(), end(), __val);
			insert(end(), __n - size(), __val);
		}
		else {
			erase(begin() + __n, end());
			fill(begin(), end(), __val);
		}
	}

	void assign(size_type __n, const _Tp& __val) {
		_M_fill_assign(__n, __val);
	}

public:
	void push_back(const value_type& __t) {
		if(_M_finish._M_cur != _M_finish._M_last - 1) {
			construct(_M_finish._M_cur, __t);
			++_M_finish._M_cur;
		}
		else
			_M_push_back_aux(__t);
	}

	void push_back() {
		if(_M_finish._M_cur != _M_finish._M_last - 1) {
			construct(_M_finish._M_cur);
			++_M_finish._M_cur;
		}
		else
			_M_push_back_aux();
	}

	void push_front(const value_type& __t) {
		if(_M_start._M_cur != _M_start._M_first) {
			construct(_M_start._M_cur - 1, __t);
			--_M_start._M_cur;
		}
		else
			_M_push_front_aux(__t);
	}

	void push_front() {
		if(_M_start._M_cur != _M_start._M_first) {
			construct(_M_start._M_cur - 1);
			--_M_start._M_cur;
		}
		else
			_M_push_front_aux();
	}

	void pop_back() {
		if(_M_finish._M_cur != _M_finish._M_first) {
			--_M_finish._M_cur;
			destroy(_M_finish._M_cur);
		}
		else
			_M_pop_back_aux();
	}

	void pop_front() {
		if(_M_start._M_cur != _M_start._M_last - 1) {
			destroy(_M_start._M_cur);
			++_M_start._M_cur;
		}
		else
			_M_pop_front_aux();
	}

public:
	iterator insert(iterator position, const value_type& __x) {
		if(position._M_cur == _M_start._M_cur) {
			push_front(__x);
			return _M_start;
		}
		else if(position._M_cur == _M_finish._M_cur) {
			push_back(__x);
			iterator __tmp = _M_finish;
			--__tmp;
			return __tmp;
		}
		else {
			return _M_insert_aux(position, __x);
		}
	}

	iterator insert(iterator __position)
	{
		return insert(__position, value_type());
	}

	void insert(iterator __pos, size_type __n, const value_type& __x)
	{
		_M_fill_insert(__pos, __n, __x);
	}

	void _M_fill_insert(iterator __pos, size_type __n, const value_type& __x);

	void insert(iterator __pos,
		        const value_type* __first, const value_type* __last);
	void insert(iterator __pos,
		        const_iterator __first, const_iterator __last);

	void resize(size_type __new_size, const value_type& __x) {
		const size_type __len = size();
		if(__new_size < __len)
			erase(_M_start + __new_size, _M_finish);
		else
			insert(_M_finish, __new_size - __len, __x);
	}

	void resize(size_type new_size) { resize(new_size, value_type()); }

public:
	iterator erase(iterator __pos) {
		iterator __next = __pos;
		++__next;
		difference_type __index = __pos - _M_start;
		if(size_type(__index) < (this->size() >> 1)) {
			copy_backward(_M_start, __pos, __next);
			pop_front();
		}
		else {
			copy(__next, _M_finish, __pos);
			pop_back();
		}
		return _M_start + __index;
	}

	iterator erase(iterator __first, iterator __last);
	void clear();

protected:
	void _M_fill_initialize(const value_type& __value);

protected:
	void _M_push_back_aux(const value_type&);
	void _M_push_back_aux();
	void _M_push_front_aux(const value_type&);
	void _M_push_front_aux();
	void _M_pop_back_aux();
	void _M_pop_front_aux();

protected:
	iterator _M_insert_aux(iterator __pos, const value_type& __x);
	iterator _M_insert_aux(iterator __pos);
	void _M_insert_aux(iterator __pos, size_type __n, const value_type& __x);
	void _M_insert_aux(iterator __pos,
				       const value_type* __first, const value_type* __last,
				       size_type __n);
	void _M_insert_aux(iterator __pos,
				       const_iterator __first, const_iterator __last,
				       size_type __n);

	iterator _M_reserve_elements_at_front(size_type __n) {
		size_type __vacancies = _M_start._M_cur - _M_start._M_first;
		if(__n > __vacancies)
			_M_new_elements_at_front(__n - __vacancies);
		return _M_start - difference_type(__n);
	}

	iterator _M_reserve_elements_at_back(size_type __n) {
		size_type __vacancies = (_M_finish._M_last - _M_finish._M_cur) - 1;
		if(__n > __vacancies)
			_M_new_elements_at_back(__n - __vacancies);
		return _M_finish + difference_type(__n);
	}

	void _M_new_elements_at_front(size_type __new_elements);
	void _M_new_elements_at_back(size_type __new_elements);

protected:
	void _M_reserve_map_at_back(size_type __nodes_to_add = 1) {
		if(__nodes_to_add + 1 > _M_map_size - (_M_finish._M_node - _M_map))
			_M_reallocate_map(__nodes_to_add, false);
	}

	void _M_reserve_map_at_front(size_type __nodes_to_add = 1) {
		if(__nodes_to_add > size_type(_M_start._M_node - _M_map))
			_M_reallocate_map(__nodes_to_add, true);
	}

	void _M_reallocate_map(size_type __nodes_to_add, bool __add_at_front);

};

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_fill_insert(iterator __pos,
								        size_type __n, const value_type& __x)
{
	if(__pos._M_cur == _M_start._M_cur) {
		iterator __new_start = _M_reserve_elements_at_front(__n);
		__STL_TRY {
			uninitialized_fill(__new_start, _M_start, __x);
		}
		__STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
	}
	else if(__pos._M_cur == _M_finish._M_cur) {
		iterator __new_finish = _M_reserve_elements_at_back(__n);
		__STL_TRY {
			uninitialized_fill(_M_finish, __new_finish, __x);
			_M_finish = __new_finish;
		}
		__STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1,
									        __new_finish._M_node + 1));
	}
	else
		_M_insert_aux(__pos, __n, __x);
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::insert(iterator __pos,
						        const value_type* __first,
						        const value_type* __last)
{
	size_type __n = __last - __first;
	if(__pos._M_cur == _M_start._M_cur) {
		iterator __new_start = _M_reserve_elements_at_front(__n);
		__STL_TRY {
			uninitialized_copy(__first, __last, __new_start);
			_M_start = __new_start;
		}
		__STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
	}
	else if(__pos._M_cur == _M_finish._M_cur) {
		iterator __new_finish = _M_reserve_elements_at_back(__n);
		__STL_TRY {
			uninitialized_copy(__first, __last, _M_finish);
			_M_finish = __new_finish;
		}
		__STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1,
									        __new_finish._M_node + 1));
	}
	else
		_M_insert_aux(__pos, __first, __last, __n);
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::insert(iterator __pos,
						        const_iterator __first, const_iterator __last)
{
	size_type __n = __last - __first;
	if(__pos._M_cur == _M_start._M_cur) {
		iterator __new_start = _M_reserve_elements_at_front(__n);
		__STL_TRY {
			uninitialized_copy(__first, __last, __new_start);
			_M_start = __new_start;
		}
		__STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
	}
	else if(__pos._M_cur == _M_finish._M_cur)
	{
		iterator __new_finish = _M_reserve_elements_at_back(__n);
		__STL_TRY {
			uninitialized_copy(__first, __last, _M_finish);
			_M_finish = __new_finish;
		}
		__STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1,
									        __new_finish._M_node + 1));
	}
	else
		_M_insert_aux(__pos, __first, __last, __n);
}

template<class _Tp, class _Alloc>
typename deque<_Tp, _Alloc>::iterator
deque<_Tp, _Alloc>::erase(iterator __first, iterator __last)
{
	if(__first == _M_start && __last == _M_finish) {
		clear();
		return _M_finish;
	}
	else {
		difference_type __n = __last - __first;
		difference_type __elems_before = __first - _M_start;
		if(__elems_before < difference_type((this->size() - __n) / 2)) {
			copy_backward(_M_start, __first, __last);
			iterator __new_start = _M_start + __n;
			destroy(_M_start, __new_start);
			_M_destroy_nodes(__new_start._M_node, _M_start._M_node);
			_M_start = __new_start;
		}
		else {
			copy(__last, _M_finish, __first);
			iterator __new_finish = _M_finish - __n;
			destroy(__new_finish, _M_finish);
			_M_destroy_nodes(__new_finish._M_node + 1, _M_finish._M_node + 1);
			_M_finish = __new_finish;
		}
		return _M_start + __elems_before;
	}
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::clear()
{
	for(_Map_pointer __node = _M_start._M_node + 1;
	    __node < _M_finish._M_node;
	    ++__node) {
		destory(*__node, *__node + _S_buffer_size());
		_M_deallocate_node(*__node);
	}

	if(_M_start._M_node != _M_finish._M_node) {
		destroy(_M_start._M_cur, _M_start._M_last);
		destory(_M_finish._M_first, _M_finish._M_cur);
		_M_deallocate_node(_M_finish._M_first);
	}
	else
		destory(_M_start._M_cur, _M_finish._M_cur);

	_M_finish = _M_start;
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_fill_initialize(const value_type& __value)
{
	_Map_pointer  __cur;
	__STL_TRY {
		for(__cur = _M_start._M_node; __cur < _M_finish._M_node; ++__cur)
			uninitialized_fill(*__cur, *__cur + _S_buffer_size(), __value);
		uninitialized_fill(_M_finish._M_first, _M_finish._M_cur, __value);
	}
	__STL_UNWIND(destroy(_M_start, iterator(*__cur, __cur)));
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_push_back_aux(const value_type& __t)
{
	value_type __t_copy = __t;
	_M_reserve_map_at_back();
	*(_M_finish._M_node + 1) = _M_allocate_node();
	__STL_TRY {
		construct(_M_finish._M_cur, __t_copy);
		_M_finish._M_set_node(_M_finish._M_node + 1);
		_M_finish._M_cur = _M_finish._M_first;
	}
	__STL_UNWIND(_M_deallocate_node(*(_M_finish._M_node + 1)));
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_push_back_aux()
{
	_M_reserve_elements_at_back();
	*(_M_finish._M_node + 1) = _M_allocate_node();
	__STL_TRY {
		construct(_M_finish._M_cur);
		_M_finish._M_set_node(_M_finish._M_node + 1);
		_M_finish._M_cur = _M_finish._M_first;
	}
	__STL_UNWIND(_M_deallocate_node(*(_M_finish._M_node + 1)));
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_push_front_aux(const value_type& __t)
{
	value_type __t_copy = __t;
	_M_reserve_elements_at_front();
	*(_M_start._M_node - 1) = _M_allocate_node();
	__STL_TRY {
		_M_start._M_set_node(_M_start._M_node - 1);
		_M_start._M_cur = _M_start._M_last - 1;
		construct(_M_start._M_cur, __t_copy);
	}
	__STL_UNWIND((++_M_start, _M_deallocate_node(*(_M_start._M_node - 1))));
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_push_front_aux()
{
	_M_reserve_elements_at_front();
	*(_M_start._M_node - 1) = _M_allocate_node();
	__STL_TRY {
		_M_start._M_set_node(_M_start._M_node - 1);
		_M_start._M_cur = _M_start._M_last - 1;
		construct(_M_start._M_cur);
	}
	__STL_UNWIND((++_M_start, _M_deallocate_node(*(_M_start._M_node - 1))));
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_pop_back_aux()
{
	_M_deallocate_node(_M_finish._M_first);
	_M_finish._M_set_mode(_M_finish._M_node - 1);
	_M_finish._M_cur = _M_finish._M_last - 1;
	destroy(_M_finish._M_cur);
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_pop_front_aux()
{
	destroy(_M_start._M_cur);
	_M_deallocate_node(_M_start._M_first);
	_M_start._M_set_node(_M_start._M_node +1 );
	_M_start._M_cur = _M_start._M_first;
}

template<class _Tp, class _Alloc>
typename deque<_Tp, _Alloc>::iterator
deque<_Tp, _Alloc>::_M_insert_aux(iterator __pos, const value_type& __x)
{
	difference_type __index = __pos - _M_start;
	value_type __x_copy = __x;
	if(size_type(__index) < this->size() / 2) {
		push_front(front());
		iterator __front1 = _M_start;
		++__front1;
		iterator __front2 = __front1;
		++__front2;
		__pos = _M_start + __index;
		iterator __pos1 = __pos;
		++__pos1;
		copy(__front2, __pos1, __front1);
	}
	else {
		push_back(back());
		iterator __back1 = _M_finish;
		--__back1;
		iterator __back2 = __back1;
		--__back2;
		__pos = _M_start + __index;
		copy_backward(__pos, __back2, __back1);
	}
	*__pos = __x_copy;
	return __pos;
}

template<class _Tp, class _Alloc>
typename deque<_Tp, _Alloc>::iterator
deque<_Tp, _Alloc>::_M_insert_aux(iterator __pos)
{
	difference_type __index = __pos - _M_start;
	if(__index < size() / 2) {
		push_back(front());
		iterator __front1 = _M_start;
		++__front1;
		iterator __front2 = __front1;
		++__front2;
		__pos = _M_start + __index;
		iterator __pos1 = __pos;
		++__pos1;
		copy(__front2, __pos1, __front1);
	}
	else {
		push_back(back());
		iterator __back1 = _M_finish;
		--__back1;
		iterator __back2 = __back1;
		--__back2;
		__pos = _M_start + __index;
		copy_backward(__pos, __back2, __back1);
	}
	*__pos = value_type();
	return __pos;
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_insert_aux(iterator __pos,
								       size_type __n,
								       const value_type& __x)
{
	const difference_type __elems_before = __pos + _M_start;
	size_type __length = this->size();
	value_type __x_copy = __x;
	if(__elems_before < difference_type(__length / 2)) {
		iterator __new_start = _M_reserve_elements_at_front(__n);
		iterator __old_start = _M_start;
		__pos = _M_start + __elems_before;
		__STL_TRY {
			if(__elems_before >= difference_type(__n)) {
				iterator __start_n = _M_start + difference_type(__n);
				uninitialized_copy(_M_start, __start_n, __new_start);
				_M_start = __new_start;
				copy(__start_n, __pos, __old_start);
				fill(__pos - difference_type(__n), __pos, __x_copy);
			}
			else {
				__uninitialized_copy_fill(_M_start, __pos, __new_start,
										  _M_start, __x.copy);
				_M_start = __new_start;
				fill(__old_start, __pos, __x_copy);
			}
		}
		__STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
	}
	else {
		iterator __new_finish = _M_reserve_elements_at_back(__n);
		iterator __old_finish = _M_finish;
		const difference_type __elems_after =
			difference_type(__length) - __elems_before;
		__pos = _M_finish - __elems_after;
		__STL_TRY {
			if(__elems_after > difference_type(__n)) {
				iterator __finish_n = _M_finish - difference_type(__n);
				uninitialized_copy(__finish_n, _M_finish, _M_finish);
				_M_finish = __new_finish;
				copy_backward(__pos, __finish_n, __old_finish);
				fill(__pos, __pos + difference_type(__n), __x_copy);
			}
			else {
				__uninitialized_fill_copy(_M_finish, __pos + difference_type(__n),
										  __x_copy, __pos, _M_finish);
				_M_finish = __new_finish;
				fill(__pos, __old_finish, __x_copy);
			}
		}
		__STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1,
									        __new_finish._M_node + 1));
	}
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_insert_aux(iterator __pos,
								       const value_type* __first,
								       const value_type* __last,
								       size_type __n)
{
	const difference_type __elemsbefore = __pos - _M_start;
	size_type __length = size();
	if(__elemsbefore < __length / 2) {
		iterator __new_start = _M_reserve_elements_at_front(__n);
		iterator __old_start = _M_start;
		__pos = _M_start + __elemsbefore;
		__STL_TRY {
			if(__elemsbefore >= difference_type(__n)) {
				iterator __start_n = _M_start + difference_type(__n);
				uninitialized_copy(_M_start, __start_n, __new_start);
				_M_start = __new_start;
				copy(__start_n, __pos, __old_start);
				copy(__first, __last, __pos - difference_type(__n));
			}
			else {
				const value_type* __mid =
					__first + (difference_type(__n) - __elemsbefore);
				__uninitialized_copy_copy(_M_start, __pos, __first, __mid,
										  __new_start);
				copy(__mid, __last, __old_start);
			}
		}
		__STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
	}
	else {
		iterator __new_finish = _M_reserve_elements_at_back(__n);
		iterator __old_finish = _M_finish;
		const difference_type __elemsafter =
			difference_type(__length) - __elemsbefore;
		__pos = _M_finish - __elemsafter;
		__STL_TRY {
			if(__elemsafter > difference_type(__n)) {
				iterator __finish_n = _M_finish - difference_type(__n);
				uninitialized_copy(__finish_n, _M_finish, _M_finish);
				_M_finish = __new_finish;
				copy_backward(__pos, __finish_n, __old_finish);
				copy(__first, __last, __pos);
			}
			else {
				const value_type* __mid = __first + __elemsafter;
				__uninitialized_copy_copy(__mid, __last, __pos, _M_finish, _M_finish);
				_M_finish = __new_finish;
				copy(__first, __mid, __pos);
			}
		}
		__STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1,
									        __new_finish._M_node + 1));
	}
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_insert_aux(iterator __pos,
								       const_iterator __first,
								       const_iterator __last,
								       size_type __n)
{
	const difference_type __elemsbefore = __pos - _M_start;
	size_type __length = size();
	if(__elemsbefore < __length / 2) {
		iterator __new_start = _M_reserve_elements_at_front(__n);
		iterator __old_start = _M_start;
		__pos = _M_start + __elemsbefore;
		__STL_TRY {
			if(__elemsbefore >= __n) {
				iterator __start_n = _M_start + __n;
				uninitialized_copy(_M_start, __start_n, __new_start);
				_M_start = __new_start;
				copy(__start_n, __pos, __old_start);
				copy(__first, __last, __pos - difference_type(__n));
			}
			else {
				const_iterator __mid = __first + (__n - __elemsbefore);
				__uninitialized_copy_copy(_M_start, __pos, __first, __mid,
										  __new_start);
				_M_start = __new_start;
				copy(__mid, __last, __old_start);
			}
		}
		__STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
	}
	else {
		iterator __new_finish = _M_reserve_elements_at_back(__n);
		iterator __old_finish = _M_finish;
		const difference_type __elemsafter = __length - __elemsbefore;
		__pos = _M_finish - __elemsafter;
		__STL_TRY {
			if(__elemsafter > __n) {
				iterator __finish_n = _M_finish - difference_type(__n);
				uninitialized_copy(__finish_n, _M_finish, _M_finish);
				_M_finish = __new_finish;
				copy_backward(__pos, __finish_n, __old_finish);
				copy(__first, __last, __pos);
			}
			else {
				const_iterator __mid = __first + __elemsafter;
				__uninitialized_copy_copy(__mid, __last, __pos, _M_finish, _M_finish);
				_M_finish = __new_finish;
				copy(__first, __mid, __pos);
			}
		}
		__STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1, __new_finish._M_node + 1));
	}
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_new_elements_at_front(size_type __new_elems)
{
	size_type __new_nodes
		= (__new_elems + _S_buffer_size() - 1) / _S_buffer_size();
	_M_reserve_map_at_front(__new_nodes);
	size_type __i;
	__STL_TRY {
		for(__i = 1; __i <= __new_nodes; ++__i)
			*(_M_start._M_node - __i) = _M_allocate_node();
	}

	catch(...) {
		for(size_type __j = 1; __j < __i; ++__j)
			_M_deallocate_node(*(_M_start._M_node - __j));
		throw;
	}
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_new_elements_at_back(size_type __new_elems)
{
	size_type __new_nodes
		= (__new_elems + _S_buffer_size() - 1) / _S_buffer_size();
	_M_reserve_map_at_back(__new_nodes);
	size_type __i;
	__STL_TRY {
		for(__i = 1; __i <= __new_nodes; ++__i)
			*(_M_finish._M_node - __i) = _M_allocate_node();
	}
	catch(...) {
		for(size_type __j = 1; __j < __i; ++__j)
			_M_deallocate_node(*(_M_start._M_node + __j));
		throw;
	}
}

template<class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_reallocate_map(size_type __nodes_to_add,
									       bool __add_at_front)
{
	size_type __old_num_nodes = _M_finish._M_node - _M_start._M_node + 1;
	size_type __new_num_nodes = __old_num_nodes + __nodes_to_add;

	_Map_pointer  __new_nstart;
	if(_M_map_size > 2 * __new_num_nodes) {
		__new_nstart = _M_map + (_M_map_size - __new_num_nodes) / 2
		                 + (__add_at_front ? __nodes_to_add : 0);
		if(__new_nstart < _M_start._M_node)
			copy(_M_start._M_node, _M_finish._M_node + 1, __new_nstart);
		else
			copy_backward(_M_start._M_node, _M_finish._M_node + 1,
						  __new_nstart + __old_num_nodes);
	}
	else {
		size_type __new_map_size =
			_M_map_size + max(_M_map_size, __nodes_to_add) + 2;

		_Map_pointer  __new_map = _M_allocate_map(__new_map_size);
		__new_nstart = __new_map + (__new_map_size - __new_num_nodes) / 2
		                     + (__add_at_front ? __nodes_to_add : 0);
		copy(_M_start._M_node, _M_finish._M_node + 1, __new_nstart);
		_M_deallocate_map(_M_map, _M_map_size);

		_M_map = __new_map;
		_M_map_size = __new_map_size;
	}

	_M_start._M_set_node(__new_nstart);
	_M_finish._M_set_node(__new_nstart + __old_num_nodes - 1);
}

template<class _Tp, class _Alloc>
inline bool operator==(const deque<_Tp, _Alloc>& __x,
		               const deque<_Tp, _Alloc>& __y) {
	return __x.size() == __y.size() &&
	       equal(__x.begin(), __x.end(), __y.begin());
}

template<class _Tp, class _Alloc>
inline bool operator<(const deque<_Tp, _Alloc>& __x,
		              const deque<_Tp, _Alloc>& __y)
{
	return lexicographical_compare(__x.begin(), __x.end(),
								   __y.begin(), __y.end());
}

template<class _Tp, class _Alloc>
inline bool operator!=(const deque<_Tp, _Alloc>& __x,
		               const deque<_Tp, _Alloc>& __y)
{
	return  !(__x == __y);
}

template<class _Tp, class _Alloc>
inline bool operator>(const deque<_Tp, _Alloc>& __x,
	                  const deque<_Tp, _Alloc>& __y)
{
	return __y < __x;
}

template<class _Tp, class _Alloc>
inline bool operator<=(const deque<_Tp, _Alloc>& __x,
	                   const deque<_Tp, _Alloc>& __y)
{
	return !(__y < __x);
}

template<class _Tp, class _Alloc>
inline bool operator>=(const deque<_Tp, _Alloc>& __x,
		               const deque<_Tp, _Alloc>& __y)
{
	return !(__x < __y);
}

template<class _Tp, class _Alloc>
inline void swap(deque<_Tp, _Alloc>& __x, deque<_Tp, _Alloc>& __y)
{
	__x.swap(__y);
}

__STL_END_NAMESPACE

#endif//JIMP_STL__STL_DEQUE_H