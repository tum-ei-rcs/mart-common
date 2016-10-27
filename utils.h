#pragma once
#include <type_traits>
#include <stdexcept>
#include <random>
#include "./cpp_std/type_traits.h"

namespace mart {

/* ######## narrowing ################################################ */

// narrow_cast(): a searchable way to do narrowing casts of values
template <class T, class U>
inline constexpr T narrow_cast(U u) noexcept
{
	static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value, "Narrow cast can only be used for arithmetic types");
	return static_cast<T>(u);
}

struct narrowing_error : std::exception {};

namespace _impl_narrow {

template <class T, class U>
struct is_same_signedness
	: std::integral_constant<bool, std::is_signed<T>::value == std::is_signed<U>::value> {};

template <class T, class U, bool SameSigndness= is_same_signedness<T,U>::value>
struct sign_check {
	//throws an narrowing error, of both parameters are of different signdness and one is negative
	static void check(T t,U u)
	{
		if ((t < T{}) != (u < U{})) {
			throw narrowing_error();
		}
	}
};

template <class T, class U>
struct sign_check<T,U,true> {
	//if both parameters are of same signess, we don't have to do anything
	static void check(T, U)
	{}
};

} // _impl_narrow

/**
 * Performs a narrowing cast and throws a narrowing_error exception if the source value can't be represented in the target type
 */
template <class T, class U>
inline T narrow(U u)
{
	T t = narrow_cast<T>(u);

	// this only does any work if T and U are of different signdness
	_impl_narrow::sign_check<T,U>::check(t, u);

	//check if roundtrip looses information
	if (static_cast<U>(t) != u) {
		throw narrowing_error();
	}

	return t;
}

/* ######## enum ################################################ */
template<class E>
using uType_t = underlying_type_t<E>;

//cast to underlying type
template<class E>
constexpr uType_t<E> toUType(E e) { return static_cast<uType_t<E>>(e); }

/* ######## random ################################################ */
inline std::default_random_engine& getRandomEngine()
{
	thread_local std::default_random_engine rg(std::random_device{}());
	return rg;
}

// Shorthand to get an random integral random number within a certain range
template <class T = int>
T getRandomInt(T min, T max)
{
	static_assert(std::is_integral<T>::value, "Parameters must be integral type");
	return std::uniform_int_distribution<T>(min, max)(getRandomEngine());
}

// Shorthand to get an random floating point number within a certain range
template<class T = double>
T getRandomFloat(T min, T max)
{
	static_assert(std::is_floating_point<T>::value, "Parameters must be floating point type");
	return std::uniform_real_distribution<T>(min, max)(getRandomEngine());
}


bool getRandomBool(double hitProb)
{
	return std::bernoulli_distribution{hitProb}(getRandomEngine());
}

/* ######## container ################################################ */

/**
 * creates a container and reserves the given amount of space.
 * So instead of
 *
 *	std::vector<int> vec;
 *	vec.reserve(100);
 *
 * you can write
 *
 *  auto vec = mart::make_with_capacity<std::vector<int>(100);
 **/

template<class T>
T make_with_capacity(size_t i)
{
	T t;
	t.reserve(i);
	return t;
}

}//mart