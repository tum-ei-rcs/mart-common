#ifndef LIB_MART_COMMON_GUARD_RANGES_H
#define LIB_MART_COMMON_GUARD_RANGES_H
/**
 * ranges.h (mart-common)
 *
 * Copyright (C) 2015-2017: Michael Balszun <michael.balszun@mytum.de>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See either the LICENSE file in the library's root
 * directory or http://opensource.org/licenses/MIT for details.
 *
 * @author: Michael Balszun <michael.balszun@mytum.de>
 * @brief: utilities for ranged for loop and combining iterator pairs into a single object
 *
 */

/* ######## INCLUDES ######### */
/* Standard Library Includes */
#include <iterator>

/* Proprietary Library Includes */
#include "./cpp_std/type_traits.h"

/* Project Includes */
/* ~~~~~~~~ INCLUDES ~~~~~~~~~ */

namespace mart {
// wrapper around a pointer, that allows deriving from it
template<class T>
class Pointer {
public:
	constexpr Pointer( T* p = nullptr )
		: ptr{p}
	{
	}
	// operator T* const &() const { return ptr; }
	// operator T*&() { return ptr; }
	constexpr T*& get() { return ptr; }
	constexpr T* const& get() const { return ptr; }

	constexpr T& operator*() const { return *ptr; }
	constexpr T* operator->() const { return ptr; }

	constexpr Pointer& operator++()
	{
		++ptr;
		return *this;
	}
	constexpr Pointer operator++( int ) { return ptr++; }

	constexpr Pointer& operator--()
	{
		--ptr;
		return *this;
	}
	constexpr Pointer operator--( int ) { return ptr--; }

	constexpr Pointer& operator+=( std::ptrdiff_t diff )
	{
		ptr += diff;
		return *this;
	}
	constexpr Pointer& operator-=( std::ptrdiff_t diff )
	{
		ptr -= diff;
		return *this;
	}

	constexpr T& operator[]( std::ptrdiff_t diff ) const { return ptr + diff; }

	friend constexpr bool operator!=( Pointer l, Pointer r ) { return l.ptr != r.ptr; }
	friend constexpr bool operator==( Pointer l, Pointer r ) { return l.ptr == r.ptr; }
	friend constexpr bool operator<( Pointer l, Pointer r ) { return l.ptr < r.ptr; }
	friend constexpr bool operator<=( Pointer l, Pointer r ) { return l.ptr <= r.ptr; }
	friend constexpr bool operator>( Pointer l, Pointer r ) { return l.ptr > r.ptr; }
	friend constexpr bool operator>=( Pointer l, Pointer r ) { return l.ptr >= r.ptr; }

	friend constexpr Pointer operator+( Pointer l, std::ptrdiff_t n ) { return l.ptr + n; }
	friend constexpr Pointer operator+( std::ptrdiff_t n, Pointer l ) { return n + l.ptr; }

	friend constexpr std::ptrdiff_t operator-( Pointer l, Pointer r ) { return l.ptr - r.ptr; }
	friend constexpr Pointer        operator-( Pointer l, std::ptrdiff_t n ) { return l.ptr - n; }

private:
	T* ptr;
};

template<class T>
class IIterator {
	static_assert( std::is_integral<T>::value, "IIterator can only be used for integral values" );

public:
	using difference_type   = std::make_signed_t<T>;
	using value_type        = T;
	using pointer           = T*;
	using reference         = const T&;
	using iterator_category = std::random_access_iterator_tag;

	constexpr IIterator( T value = T() )
		: i{value} {};

	constexpr reference operator*() const { return i; }
	constexpr pointer   operator->() const { return &i; }

	constexpr IIterator& operator++()
	{
		++i;
		return *this;
	}
	constexpr IIterator operator++( int ) { return IIterator{ i++ }; }

	constexpr IIterator& operator--()
	{
		--i;
		return *this;
	}
	constexpr IIterator operator--( int ) { return IIterator{ i-- }; }

	constexpr IIterator& operator+=( difference_type diff )
	{
		i += diff;
		return *this;
	}
	constexpr IIterator& operator-=( difference_type diff )
	{
		i -= diff;
		return *this;
	}

	constexpr value_type operator[]( difference_type diff ) const { return i + diff; }

	friend constexpr bool operator!=( IIterator l, IIterator r ) { return l.i != r.i; }
	friend constexpr bool operator==( IIterator l, IIterator r ) { return l.i == r.i; }
	friend constexpr bool operator<( IIterator l, IIterator r ) { return l.i < r.i; }
	friend constexpr bool operator<=( IIterator l, IIterator r ) { return l.i <= r.i; }
	friend constexpr bool operator>( IIterator l, IIterator r ) { return l.i > r.i; }
	friend constexpr bool operator>=( IIterator l, IIterator r ) { return l.i >= r.i; }

	friend constexpr IIterator operator+( IIterator l, difference_type n ) { return l.i + n; }
	friend constexpr IIterator operator+( difference_type n, IIterator l ) { return n + l.i; }

	friend constexpr difference_type operator-( IIterator l, IIterator r ) { return l.i - r.i; }
	friend constexpr IIterator       operator-( IIterator l, difference_type n ) { return l.i - n; }

private:
	T i;
};

template<class IT>
class EndAwareIterator : public IT {
public:
	template<class C>
	constexpr EndAwareIterator( IT it, C&& c )
		: IT{it}
		, _is_end{it == c.end()} {};

	constexpr explicit operator bool() const { return !_is_end; }
	constexpr bool     is_end() const { return _is_end; }

private:
	bool _is_end;
};

template<class T>
class EndAwareIterator<T*> : public EndAwareIterator<mart::Pointer<T>> {
	using EndAwareIterator<mart::Pointer<T>>::EndAwareIterator;
};

namespace _impl_irange {

template<class T>
struct irange_t {
	static_assert( std::is_integral<T>::value, "irange can only be used for integral values" );
	using iterator = IIterator<T>;

	constexpr iterator begin() const { return _start; };
	constexpr iterator end() const { return _past_end; };

	iterator _start;
	iterator _past_end;
};

template<class T>
struct non_deduced {
	using type = T;
};

template<class T>
using non_deduced_t = typename non_deduced<T>::type;

template<class IT>
struct is_random_it {
	static constexpr bool value
		= std::is_same<typename std::iterator_traits<IT>::iterator_category, std::random_access_iterator_tag>::value;
};

template<class U, class = typename U::iterator>
struct has_random_it {
	static constexpr bool value = is_random_it<typename U::iterator>::value;
};

} // namespace _impl_irange

template<class T>
constexpr _impl_irange::irange_t<T> irange( _impl_irange::non_deduced_t<T> start, T end )
{
	return _impl_irange::irange_t<T>{start, end};
}

template<class T, class = std::enable_if_t<std::is_integral<T>::value>>
constexpr _impl_irange::irange_t<T> irange( T end )
{
	return _impl_irange::irange_t<T>{0, end};
}

template<class C,
		 class IT = typename C::iterator,
		 class    = std::enable_if_t<std::is_base_of<std::random_access_iterator_tag,
                                                   typename std::iterator_traits<IT>::iterator_category>::value>>
constexpr _impl_irange::irange_t<typename C::size_type> irange( const C& container )
{
	return _impl_irange::irange_t<typename C::size_type>{0, container.size()};
}

template<class T, size_t N>
constexpr _impl_irange::irange_t<size_t> irange( const T ( & )[N] )
{
	return _impl_irange::irange_t<size_t>{0, N};
}

} // namespace mart

namespace std {
template<class IT>
struct iterator_traits<mart::EndAwareIterator<IT>> : public std::iterator_traits<IT> {
};
} // namespace std

#endif
