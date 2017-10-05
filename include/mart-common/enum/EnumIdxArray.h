#ifndef LIB_MART_COMMON_GUARD_ENUM_ENUM_IDX_ARRAY_H
#define LIB_MART_COMMON_GUARD_ENUM_ENUM_IDX_ARRAY_H
/**
 * EnumIdxArray.h (mart-common/enum)
 *
 * Copyright (C) 2015-2017: Michael Balszun <michael.balszun@mytum.de>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See either the LICENSE file in the library's root
 * directory or http://opensource.org/licenses/MIT for details.
 *
 * @author: Michael Balszun <michael.balszun@mytum.de>
 * @brief: array that can directly be indexed by an enum
 *
 */

/* ######## INCLUDES ######### */
/* Standard Library Includes */
#include <array>

/* Proprietary Library Includes */

/* Project Includes */
#include "../cpp_std/type_traits.h"
/* ~~~~~~~~ INCLUDES ~~~~~~~~~ */


namespace mart {
/**
* encapsulates an array, such that it can be indexed by enum values.
* Most member functions are directly mapped to their std::array counterparts
*
* NOTE: Currently the enums have to be values that directly translate to 0-based indexes
*/
template<class T, class ENUM, std::size_t N>
struct EnumIdxArray : std::array<T, N> {
	using Storage_t = std::array<T, N>;

	//only explicitly importing types used in the new memberfunctions to reduce typing
	//(user will have automatically access to all member types of the base
	using typename Storage_t::size_type;
	using typename Storage_t::reference;
	using typename Storage_t::const_reference;

	/* ###### CTORS ###### */
	constexpr EnumIdxArray() noexcept : Storage_t{}{};
	constexpr EnumIdxArray(const EnumIdxArray<T, ENUM, N>& other) = default;
	constexpr EnumIdxArray(      EnumIdxArray<T, ENUM, N>&& ) = default;
	EnumIdxArray& operator=(const EnumIdxArray& ) = default;
	EnumIdxArray& operator=(	  EnumIdxArray&&) = default;

	// This is a constructor forwarding the arguments to the internal array,
	// but we have to make sure it doesn't superseed the copy constructor -> hence the enable_if
	template<class A1, class ... ARGS ,class = mart::enable_if_t<!std::is_same<EnumIdxArray<T,ENUM,N>, mart::decay_t<A1>>::value>>
	constexpr EnumIdxArray(A1&& arg, ARGS&& ... args)
		: Storage_t{ { std::forward<A1>(arg), std::forward<ARGS>(args)... } }
	{
	}
private:
	/*#### data ####*/
	static constexpr auto toIdx(ENUM e) -> size_type {
		return static_cast<size_type>(static_cast< underlying_type_t<ENUM> >(e));
	};
					Storage_t& as_array()		{ return static_cast<Storage_t&>(*this); }
	constexpr const Storage_t& as_array() const { return static_cast< const Storage_t&>(*this); }
public:
	/*#### container interface ####*/
					reference	at(ENUM pos)		{ return as_array().at(toIdx(pos)); }
	constexpr const_reference	at(ENUM pos) const	{ return as_array().at(toIdx(pos)); }

					reference	operator[](ENUM pos)		{ return as_array()[toIdx(pos)]; }
	constexpr const_reference	operator[](ENUM pos) const	{ return as_array()[toIdx(pos)]; }
};


}//mart

#endif //LIB_MART_COMMON_GUARD_UTILS_H