#ifndef LIBS_MART_COMMON_STRING_VIEW_H
#define LIBS_MART_COMMON_STRING_VIEW_H
#pragma once

/* ######## INCLUDES ######### */
/* Standard Library Includes */
#include <algorithm>
#include <cstring>
#include <cassert>
#include <memory>
#include <ostream>
#include <string>

/* Proprietary Library Includes */
#include "cpp_std/type_traits.h"
#include "algorithm.h"

/* Project Includes */
#include "ArrayViewAdaptor.h"

namespace mart {

class StringView : public ArrayViewAdaptor<const char, StringView> {
public:
	// type defs
	using CharT = char;
	using traits_type = std::char_traits<CharT>;

	static constexpr size_type npos = size_type(-1);

	/* #### CTORS #### */
	constexpr StringView() noexcept = default;

	StringView(const std::string& other) noexcept
		: _start(other.data())
		, _size(other.size())
	{
	}

	constexpr StringView(const char* other, size_type size) noexcept
		: _start(other)
		, _size(size)
	{
	}

	static StringView fromZString(const char* other) { return{ other, std::strlen(other) }; }

	// NOTE: Use only for string literals!!!
	template <size_t N>
	constexpr StringView(const char(&other)[N]) noexcept
		: _start(other)
		, _size(N - 1)
	{
	}

	constexpr StringView(const char &other) noexcept
		: _start(&other)
		, _size(1)
	{
	}

	//prevent construction from integral type except char
	template<class T, class = typename std::enable_if<std::is_integral<T>::value && (!std::is_same<T, char>::value)>::type >
	constexpr StringView(const T &other) = delete;

	template <class T>
	StringView(const T* const& other) = delete;

	/* #### Special member functions #### */
	constexpr StringView(const StringView& other) noexcept = default;
	StringView& operator=(const StringView& other) noexcept = default;
	constexpr StringView(StringView&& other) noexcept = default;
	StringView& operator=(StringView&& other) noexcept = default;

	/*#### string functions ####*/
	std::string to_string() const { return std::string(this->cbegin(), this->cend()); }

	constexpr StringView substr(size_t offset, size_t count = npos) const
	{
		return offset + count <= this->_size
			? StringView{ this->_start + offset, count }
			: count == npos
			? substr(offset, size() - offset)
			: throw std::out_of_range(
				"Tried to create a substring that would exceed the original string. "
				"Original string:\n\n" + this->to_string() + "\n");
	}

	/**
	* Splits the string at given position and returns a pair holding both substrings
	*  - if 0 <= pos < size():
	*		return substrings [0...pos) [pos ... size())
	*	- if pos == size() or npos:
	*		returns a copy of the current stringview and a default constructed one
	*	- if pos > size()
	*		throws std::out_of_range exception
	*/
	constexpr std::pair<StringView, StringView> split(size_t pos) const
	{
		return pos < _size
			? std::make_pair(StringView{ this->_start, pos }, StringView{ this->_start + pos + 1, _size - pos - 1 })
			: (pos == _size || pos == npos)
			? std::pair<StringView, StringView>{ *this, StringView{} }
		: throw std::out_of_range("\nTried to create a substring that would exceed the original string. "
								  "Original string:\n\n"
								  + this->to_string()
								  + "\n");
	}

	/*#### algorithms ####*/

	size_type find(char c, size_type start_pos = 0) const
	{
		if (start_pos >= size()) {
			return npos;
		}

		const size_t pos = std::find(this->cbegin() + start_pos, this->cend(), c) - this->cbegin();
		return pos < this->size() ? pos : npos;
	}

	template <class P>
	size_type find_if(P p, size_type start_pos = 0) const
	{
		if (start_pos >= size()) {
			return npos;
		}

		const size_t pos = std::find_if(this->cbegin() + start_pos, this->cend(), p) - this->cbegin();
		return pos < this->size() ? pos : npos;
	}

	StringView substr_sentinel(size_t offset, char sentinel) const
	{
		return substr(offset, this->find(sentinel, offset));
	}

	template <class P>
	StringView substr_predicate(size_t offset, P p) const
	{
		return substr(offset, this->find_if(p, offset));
	}

	friend int compare(StringView l, StringView r);
	friend std::ostream& operator<<(std::ostream& out, const StringView string)
	{
		out.write(string.data(), string.size());
		return out;
	}

	bool isValid() const { return _start != nullptr; }

protected:
	friend class ArrayViewAdaptor<const char, StringView>;
	constexpr size_type		_arrayView_size() const { return _size; }
	constexpr const_pointer _arrayView_data() const { return _start; }

	const char* _start = nullptr;
	size_type   _size = 0;
};

inline int compare(StringView l, StringView r)
{
	if ((l._start == r._start) && (l.size() == l.size())) {
		return 0;
	}
	int ret = StringView::traits_type::compare(l.cbegin(), r.cbegin(), std::min(l.size(), r.size()));

	if (ret == 0) {
		// couldn't find a difference yet -> compare sizes
		if (l.size() < r.size()) {
			ret = -1;
		} else if (l.size() > r.size()) {
			ret = 1;
		}
	}
	return ret;
}

/* operator overloads */
// clang-format off
inline bool operator==(const StringView& l, const StringView& r) { return compare(l, r) == 0; }
inline bool operator!=(const StringView& l, const StringView& r) { return !(l == r); }
inline bool operator< (const StringView& l, const StringView& r) { return compare(l, r) < 0; }
inline bool operator> (const StringView& l, const StringView& r) { return r<l; }
inline bool operator<=(const StringView& l, const StringView& r) { return !(l>r); }
inline bool operator>=(const StringView& l, const StringView& r) { return !(l < r); }
// clang-format on
constexpr StringView EmptyStringView{ "" };
}

namespace std {
template <>
struct hash<mart::StringView> {
	// form http://stackoverflow.com/questions/24923289/is-there-a-standard-mechanism-to-retrieve-the-hash-of-a-c-string
	std::size_t operator()(mart::StringView str) const noexcept
	{
		std::size_t h = 0;

		for (auto c : str) {
			h += h * 65599 + c;
		}

		return h ^ (h >> 16);
	}
};
}

namespace mart {

namespace details_to_integral {
//Core logic.
//Assuming number starts at first character and string is supposed to be parsed until first non-digit
template<class T>
T core(mart::StringView str)
{
	T tmp = 0;
	for (auto c : str) {
		int d = c - '0';
		if (d < 0 || 9 < d ) {
			break;
		}
		if (tmp >= std::numeric_limits<T>::max() / 16) { // quick check against simple constant
			if ( tmp > (std::numeric_limits<T>::max() - d) / 10 ) {
				throw std::out_of_range("String representing an integral (\"" + str.to_string() + "\") overflows overflows type " + typeid(T).name());
			}
		}
		tmp = tmp * 10 + d;

	}
	return tmp;
}

//for unsigned types
template<class T>
auto base(const mart::StringView str) -> mart::enable_if_t<std::is_unsigned<T>::value, T>
{
	assert(str.size() > 0);
	if (str[0] == '+') {
		return details_to_integral::core<T>(str.substr(1));
	} else {
		return details_to_integral::core<T>(str);
	}
}

//for signed types
template<class T>
auto base(const mart::StringView str) -> mart::enable_if_t<std::is_signed<T>::value, T>
{
	assert(str.size() > 0);
	switch(str[0]) {
		case '-' : return -details_to_integral::core<T>(str.substr(1));
		case '+' : return  details_to_integral::core<T>(str.substr(1));
		default  : return  details_to_integral::core<T>(str);
	}
}
}

template<class T = int>
T to_integral(const mart::StringView str) {
	if (str.size() == 0) {
		return T{};
	}
	return details_to_integral::base<T>(str);
}

//minimal implementation for sanitized strings that only contain digits (no negative numbers)
template<class T>
T to_integral_unsafe(mart::StringView str)
{
	return mart::accumulate(str, T{}, [](T sum, char c) { return sum * 10 + c - '0'; });
}

}

#endif
