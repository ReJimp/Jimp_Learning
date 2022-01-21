// Created by Jimp on 2021/8/3.

#ifndef JIMP_STL__STL_ITERATOR_H
#define JIMP_STL__STL_ITERATOR_H

#include "stl_config.h"
#include "stl_iterator_base.h"

__STL_BEGIN_NAMESPACE

template <class _Container>
class back_insert_iterator {
protected:
	_Container* container;

public:
	typedef _Container          container_type;
	typedef output_iterator_tag iterator_category;
	typedef void                value_type;
	typedef void                difference_type;
	typedef void                pointer;
	typedef void                reference;

	explicit back_insert_iterator(_Container& __x) : container(&__x) {}
	back_insert_iterator<_Container>&
	operator=(const typename _Container::value_type& __value)
	{
		container->push_back(__value);
		return *this;
	}
	back_insert_iterator<_Container>& operator*() { return *this; }
	back_insert_iterator<_Container>& operator++() { return *this; }
	back_insert_iterator<_Container>& operator++(int) { return *this; }
};

template <class _Container>
inline back_insert_iterator<_Container> back_inserter(_Container& __x)
{
	return back_insert_iterator<_Container>(__x);
}

template <class _Container>
class front_insert_iterator {
protected:
	_Container* container;

public:
	typedef _Container          container_type;
	typedef output_iterator_tag iterator_category;
	typedef void                value_type;
	typedef void                difference_type;
	typedef void                pointer;
	typedef void                reference;

	explicit front_insert_iterator(_Container& __x) : container(&__x) {}
	front_insert_iterator<_Container>&
	operator=(const typename _Container::value_type& __value)
	{
		container->push_front(__value);
		return *this;
	}
	front_insert_iterator<_Container>& operator*() { return *this; }
	front_insert_iterator<_Container>& operator++() { return *this; }
	front_insert_iterator<_Container>& operator++(int) { return *this; }
};

template <class _Container>
inline front_insert_iterator<_Container> front_insert(_Container& __x)
{
	return front_insert_iterator<_Container>(__x);
}

template <class _Container>
class insert_iterator {
protected:
	_Container* container;
	typename _Container::iterator iter;

public:
	typedef _Container          container_type;
	typedef output_iterator_tag iterator_category;
	typedef void                value_type;
	typedef void                difference_type;
	typedef void                pointer;
	typedef void                reference;

	insert_iterator(_Container& __x, typename _Container::iterator __i)
		: container(&__x), iter(__i) {}
	insert_iterator<_Container>&
	operator=(const typename _Container::value_type& __value)
	{
		iter = container->insert(iter, __value);
		++iter;
		return *this;
	}
	insert_iterator<_Container>& operator*() { return *this; }
	insert_iterator<_Container>& operator++() { return *this; }
	insert_iterator<_Container>& operator++(int) { return *this; }
};

template <class _Container, class _Iterator>
inline
insert_iterator<_Container> inserter(_Container& __x, _Iterator __i)
{
	typedef typename _Container::iterator __iter;
	return insert_iterator<_Container>(__x, __iter(__i));
}

template <class _BidirectionalIterator, class _Tp, class _Reference = _Tp&,
          class _Distance = ptrdiff_t>
class reverse_bidirectional_iterator {
	typedef reverse_bidirectional_iterator<_BidirectionalIterator, _Tp,
										   _Reference, _Distance> _Self;

protected:
	_BidirectionalIterator current;

public:
	typedef bidirectional_iterator_tag iterator_category;
	typedef _Tp                        value_type;
	typedef _Distance                  difference_type;
	typedef _Tp*                       pointer;
	typedef _Reference                 reference;

	reverse_bidirectional_iterator() {}
	explicit reverse_bidirectional_iterator(_BidirectionalIterator __x)
		: current(__x) {}
	_BidirectionalIterator base() const { return current; }
	_Reference operator*() const {
		_BidirectionalIterator __tmp = current;
		return *--__tmp;
	}
	pointer operator->() const { return &(operator*()); }

	_Self& operator++() {
		--current;
		return *this;
	}
	_Self operator++(int) {
		_Self __tmp = *this;
		--current;
		return __tmp;
	}
	_Self& operator--() {
		++current;
		return *this;
	}
	_Self operator--(int) {
		_Self __tmp = *this;
		++current;
		return __tmp;
	}
};

template <class _BiIter, class _Tp, class _Ref, class _Distance>
inline bool operator==(
	const reverse_bidirectional_iterator<_BiIter, _Tp, _Ref, _Distance>& __x,
	const reverse_bidirectional_iterator<_BiIter, _Tp, _Ref, _Distance>& __y)
{
	return __x.base() == __y.base();
}

template <class _BiIter, class _Tp, class _Ref, class _Distance>
inline bool operator!=(
	const reverse_bidirectional_iterator<_BiIter, _Tp, _Ref, _Distance>& __x,
	const reverse_bidirectional_iterator<_BiIter, _Tp, _Ref, _Distance>& __y)
{
	return !(__x == __y);
}

template <class _Iterator>
class reverse_iterator
{
protected:
	_Iterator current;

public:
	typedef typename iterator_traits<_Iterator>::iterator_category
		    iterator_category;
	typedef typename iterator_traits<_Iterator>::value_type
		    value_type;
	typedef typename iterator_traits<_Iterator>::difference_type
		    difference_type;
	typedef typename iterator_traits<_Iterator>::pointer
	        pointer;
	typedef typename iterator_traits<_Iterator>::reference
		    reference;

	typedef _Iterator iterator_type;
	typedef reverse_iterator<_Iterator> _Self;

public:
	reverse_iterator() {}
	explicit reverse_iterator(iterator_type __x) : current(__x) {}

	reverse_iterator(const _Self& __x) : current(__x.current) {}

	iterator_type base() const { return current; }
	reference operator*() const {
		_Iterator __tmp = current;
		return *--__tmp;
	}
	pointer operator->() const { return &(operator*()); }

	_Self& operator++() {
		--current;
		return *this;
	}
	_Self operator++(int) {
		_Self __tmp = *this;
		--current;
		return __tmp;
	}
	_Self& operator--() {
		++current;
		return *this;
	}
	_Self operator--(int) {
		_Self __tmp = *this;
		++current;
		return __tmp;
	}

	_Self operator+(difference_type __n) const {
		return _Self(current - __n);
	}
	_Self& operator+=(difference_type __n) {
		current -= __n;
		return *this;
	}
	_Self operator-(difference_type __n) const {
		return _Self(current + __n);
	}
	_Self& operator-=(difference_type __n) {
		current += __n;
		return *this;
	}
	reference operator[](difference_type __n) const { return *(*this + __n); }
};

template <class _Iterator>
inline bool operator==(const reverse_iterator<_Iterator>& __x,
		               const reverse_iterator<_Iterator>& __y)
{
	return __x.base() == __y.base();
}

template <class _Iterator>
inline bool operator<(const reverse_iterator<_Iterator>& __x,
		              const reverse_iterator<_Iterator>& __y)
{
	return __y.base() < __x.base();
}

template <class _Iterator>
inline bool operator!=(const reverse_iterator<_Iterator>& __x,
		               const reverse_iterator<_Iterator>& __y)
{
	return !(__x == __y);
}

template <class _Iterator>
inline bool operator>(const reverse_iterator<_Iterator>& __x,
	                  const reverse_iterator<_Iterator>& __y)
{
	return __y < __x;
}

template <class _Iterator>
inline bool operator<=(const reverse_iterator<_Iterator>& __x,
	                   const reverse_iterator<_Iterator>& __y)
{
	return !(__y < __x);
}

template <class _Iterator>
inline bool operator>=(const reverse_iterator<_Iterator>& __x,
	                   const reverse_iterator<_Iterator>& __y)
{
	return !(__x < __y);
}

template <class _Iterator>
inline typename reverse_iterator<_Iterator>::difference_type
operator-(const reverse_iterator<_Iterator>& __x,
		  const reverse_iterator<_Iterator>& __y)
{
	return __y.base() - __x.base();
}

template <class _Iterator>
inline reverse_iterator<_Iterator>
operator+(typename reverse_iterator<_Iterator>::difference_type __n,
		  const reverse_iterator<_Iterator>& __x)
{
	return reverse_iterator<_Iterator>(__x.base() - __n);
}

__STL_END_NAMESPACE

#endif//JIMP_STL__STL_ITERATOR_H