// Created by Jimp on 2021/8/3.

#ifndef JIMP_STL__STL_HAS_SET_H
#define JIMP_STL__STL_HAS_SET_H

#include "stl_config.h"
#include "stl_hash_fun.h"
#include "hashtable.h"
#include "stl_alloc.h"
#include "stl_function.h"
#include "stl_pair.h"

__STL_BEGIN_NAMESPACE

template <class _Value,
		  class _HashFcn = hash<_Value>,
		  class _EqualKey = equal_to<_Value>,
		  class _Alloc = alloc>
class hash_set;

template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
inline bool
operator==(const hash_set<_Value, _HashFcn, _EqualKey, _Alloc>& __hs1,
		   const hash_set<_Value, _HashFcn, _EqualKey, _Alloc>& __hs2);

template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
class hash_set
{
private:
	typedef hashtable<_Value, _Value, _HashFcn, _Identity<_Value>,
					  _EqualKey, _ALloc> _Ht;
	_Ht _M_ht;

public:
	typedef typename _Ht::key_type key_type;
	typedef typename _Ht::value_type value_type;
	typedef typename _Ht::hasher hasher;
	typedef typename _Ht::key_equal key_equal;

	typedef typename _Ht::size_type size_type;
	typedef typename _Ht::difference_type difference_type;
	typedef typename _Ht::const_pointer pointer;
	typedef typename _Ht::const_pointer const_pointer;
	typedef typename _Ht::const_reference reference;
	typedef typename _Ht::const_reference const_reference;

	typedef typename _Ht::const_iterator iterator;
	typedef typename _Ht::const_iterator const_iterator;

	typedef typename _Ht::allocate_type allocator_type;

	hasher hash_funct() const { return _M_ht.hash_funct(); }
	key_equal key_eq() const { return _M_ht.key_eq(); }
	allocator_type get_allocator() const { return _M_ht.get_allocator(); }

public:
	hash_set()
		: _M_ht(100, hasher(), key_equal(), allocator_type()) {}
	explicit hash_set(size_type __n)
		: _M_ht(__n, hasher(), key_equal(), allocator_type()) {}
	hash_set(size_type __n, const hasher& __hf)
		: _M_ht(__n, __hf, key_equal(), allocator_type()) {}
	hash_set(size_type __n, const hasher& __hf, const key_equal& __eql,
			 const allocator_type& __a = allocator_type())
		: _M_ht(__n, __hf, __eql, __a) {}

	hash_set(const value_type* __f, const value_type* __l)
		: _M_ht(100, hasher(), key_equal(), allocator_type())
	{
		_M_ht.insert_unique(__f, __l);
	}
	hash_set(const value_type* __f, const value_type* __l, size_type __n)
		: _M_ht(__n, hasher(), key_equal(), allocator_type())
	{
		_M_ht.insert_unique(__f, __l);
	}
	hash_set(const value_type* __f, const value_type* __l, size_type __n,
			 const hasher& __hf)
		: _M_ht(__n, __hf, key_equal(), allocator_type())
	{
		_M_ht.insert_unique(__f, __l);
	}
	hash_set(const value_type* __f, const value_type* __l, size_type __n,
			 const hasher& __hf, const key_equal& __eql,
			 const allocator_type& __a = allocator_type())
		: _M_ht(__n, __hf, __eql, __a)
	{
		_M_ht.insert_unique(__f, __l);
	}

	hash_set(const_iterator __f, const_iterator __l)
		: _M_ht(100, hasher(), key_equal(), allocator_type())
	{
		_M_ht.insert_unique(__f, __l);
	}
	hash_set(const_iterator __f, const_iterator __l, size_type __n)
		: _M_ht(__n, hasher(), key_equal(), allocator_type())
	{
		_M_ht.insert_unique(__f, __l);
	}
	hash_set(const_iterator __f, const_iterator __l, size_type __n,
			 const hasher& __hf)
		: _M_ht(__n, __hf, key_equal(), allocator_type())
	{
		_M_ht.insert_unique(__f, __l);
	}
	hash_set(const_iterator __f, const_iterator __l, size_type __n,
			 const hasher& __hf, const key_equal& __eql,
			 const allocator_type& __a = allocator_type())
		: _M_ht(__n, __hf, __eql, __a)
	{
		_M_ht.insert_unique(__f, __l);
	}

public:
	size_type size() const { return _M_ht.size(); }
	size_type max_size() const { return _M_ht.max_size(); }
	bool empty() const { return _M_ht.empty(); }
	void swap(hash_set& __hs) { _M_ht.swap(__hs._M_ht); }

	friend bool
	operator==(const hash_set&, const hash_set&>);

	iterator begin() const { return _M_ht.begin(); }
	iterator end() const { return _M_ht.end(); }

public:
	pair<iterator, bool> insert(const value_type& __obj)
	{
		pair<typename _Ht::iterator, bool> __p = _M_ht.insert_unique(__obj);
		return pair<iterator, bool>(__p.first, __p.second);
	}

	void insert(const value_type* __f, const value_type* __l)
	{
		_M_ht.insert_unique(__f, __l);
	}
	void insert(const_iterator __f, const_iterator __l)
	{
		_M_ht.insert_unique(__f, __l);
	}

	pair<iterator, bool> insert_noresize(const value_type& __obj)
	{
		pair<typename _Ht::iterator, bool> __p =
			_M_ht.insert_unique_noresize(__obj);
		return pair<iterator, bool>(__p.first, __p.second);
	}

	iterator find(const key_type& __key) const { return _M_ht.find(__key); }

	size_type count(const key_type& __key) const { return _M_ht.count(__key); }

	pair<iterator, iterator> equal_range(const key_type& __key) const
	{
		return _M_ht.equal_range(__key);
	}

	size_type erase(const key_type& __key) { return _M_ht.erase(__key); }
	void erase(iterator __it) { _M_ht.erase(__it); }
	void erase(iterator __f, iterator __l) { _M_ht.erase(__f, __l); }
	void clear() { _M_ht.clear(); }

public:
	void resize(size_type __hint) { _M_ht.resize(__hint); }
	size_type bucket_count() const { return _M_ht.bucket_count(); }
	size_type max_bucket_count() const { return _M_ht.max_bucket_count(); }
	size_type elems_in_bucket(size_type __n) const { return _M_ht.elems_in_bucket(__n); }

};

template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
inline bool
operator==(const hash_set<_Value, _HashFcn, _EqualKey, _Alloc>& __hs1,
		   const hash_set<_Value, _HashFcn, _EqualKey, _Alloc>& __hs2)
{
	return __hs1._M_ht == __hs2._M_ht;
}

template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
inline bool
operator!=(const hash_set<_Value, _HashFcn, _EqualKey, _Alloc>& __hs1,
		   const hash_set<_Value, _HashFcn, _EqualKey, _Alloc>& __hs2)
{
	return !(__hs1 == __hs2);
}

template <class _Val, class _HashFcn, class _EqualKey, class _Alloc>
inline void
swap(hash_set<_Val,_HashFcn,_EqualKey,_Alloc>& __hs1,
	 hash_set<_Val,_HashFcn,_EqualKey,_Alloc>& __hs2)

{
	__hs1.swap(__hs2);
}

__STL_END_NAMESPACE

#endif//JIMP_STL__STL_HAS_SET_H