/*
 * Optional.h
 *
 *  Created on: Oct 13, 2015
 *      Author: balszun
 */

#ifndef LIBS_C2_API_LIBS_COMMON_OPTIONAL_H_
#define LIBS_C2_API_LIBS_COMMON_OPTIONAL_H_
#pragma once

/* ######## INCLUDES ######### */
/* Standard Library Includes */
#include <stdexcept>

/* Proprietary Library Includes */

/* Project Includes */

namespace mart {
		template <class T>
		class Optional {
			enum class OPT_FLAG : uint8_t {
				Invalid = 0,
				Valid
			};
			T data;
			OPT_FLAG flag = OPT_FLAG::Invalid;

			void throwIfInvalid() const {
				if (flag != OPT_FLAG::Valid) {
					throw std::invalid_argument("Access to invalid data of Optional class");
				}
			}

		public:
			Optional() = default;
			Optional(const T& data) : data(data)			, flag(OPT_FLAG::Valid) {};
			Optional(T&& data)		: data(std::move(data))	, flag(OPT_FLAG::Valid) {};

			Optional(const Optional<T>& other) = default;
			Optional(Optional<T>&& other) = default;
			Optional& operator=(const Optional<T>& other) = default;
			Optional& operator=(Optional<T>&& other) = default;

			const T& getData() const {
				throwIfInvalid();
				return data;
			}

			void setData(const T& data) {
				this->data = data;
				this->flag = OPT_FLAG::Valid;
			}

			const T& 	operator*() const	{ return data; }
			T&			operator*()			{ return data; }
			const T*	operator->() const	{ return &data; }
			T*			operator->()		{ return &data; }

			explicit operator bool()	const { return isValid(); }
			OPT_FLAG getState()			const { return flag; }
			bool isValid()				const { return flag == OPT_FLAG::Valid; }

		};
}



#endif /* LIBS_C2_API_LIBS_COMMON_OPTIONAL_H_ */