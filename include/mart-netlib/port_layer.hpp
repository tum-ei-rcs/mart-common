#ifndef LIB_MART_COMMON_GUARD_NW_PORT_LAYER_H
#define LIB_MART_COMMON_GUARD_NW_PORT_LAYER_H
/**
 * port_layer.h (mart-common/nw)
 *
 * Copyright (C) 2015-2019: Michael Balszun <michael.balszun@mytum.de>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See either the LICENSE file in the library's root
 * directory or http://opensource.org/licenses/MIT for details.
 *
 * @author: Michael Balszun <michael.balszun@mytum.de>
 * @brief:  Contains all network related functions that need platform specific implementation
 *
 */

#include "basic_types.hpp"

#include <cerrno>
#include <chrono>
#include <errno.h>

namespace mart {
namespace nw {
namespace socks {

namespace port_layer {

/* ########################################################################################### */
/* ############# Translation from portable enums and values to native api values ############# */

#ifdef MBA_UTILS_USE_WINSOCKS
using native_handle_t = std::uintptr_t;
enum class handle_t : native_handle_t { Invalid = ~0 };

#else
using native_handle_t = int;
enum class handle_t : native_handle_t { Invalid = -1 };

#endif // MBA_UTILS_USE_WINSOCKS

inline native_handle_t to_native( handle_t h ) noexcept
{
	return static_cast<native_handle_t>( h );
}

int to_native( Domain domain ) noexcept;
int to_native( TransportType transport_Type ) noexcept;
int to_native( Protocol protocol ) noexcept;

// Wrapper functions for socket related functions, that are specific to a certain platform

/* ################################################################################ */
/* ############# Wrapper around native socket API ################################# */

ReturnValue<handle_t> socket( Domain domain, TransportType transport_type, Protocol protocol = Protocol::Default );
ErrorCode close_socket( handle_t handle ) noexcept;

ReturnValue<handle_t> accept( handle_t handle, Sockaddr& addr ) noexcept;
ReturnValue<handle_t> accept( handle_t handle ) noexcept;

ErrorCode connect( handle_t handle, const Sockaddr& addr ) noexcept;
ErrorCode bind( handle_t handle, const Sockaddr& addr ) noexcept;
ErrorCode listen( handle_t handle, int backlog ) noexcept;

ReturnValue<txrx_size_t> send( handle_t handle, byte_range buf, int flags ) noexcept;
ReturnValue<txrx_size_t> sendto( handle_t handle, byte_range buf, int flags, const Sockaddr& to ) noexcept;
ReturnValue<txrx_size_t> recv( handle_t handle, byte_range_mut buf, int flags ) noexcept;
ReturnValue<txrx_size_t> recvfrom( handle_t handle, byte_range_mut buf, int flags, Sockaddr& from ) noexcept;

ErrorCode setsockopt( handle_t handle, SocketOptionLevel level, SocketOption optname, byte_range data ) noexcept;
ErrorCode getsockopt( handle_t handle, SocketOptionLevel level, SocketOption optname, byte_range_mut& buffer ) noexcept;

ErrorCode get_last_socket_error() noexcept;
bool      waInit() noexcept;

ErrorCode set_timeout( handle_t handle, Direction direction, std::chrono::microseconds timeout ) noexcept;
ReturnValue<std::chrono::microseconds> get_timeout( handle_t handle, Direction direction ) noexcept;
ErrorCode                              set_blocking( handle_t handle, bool should_block ) noexcept;


/* ################################################################################ */
/* ############# Wrapper for various address types ################################ */

struct SockaddrIn : mart::nw::socks::Sockaddr {
	SockaddrIn()
		: SockaddrIn( Storage {} )
	{
	}
	SockaddrIn( const SockaddrIn& other )
		: SockaddrIn( other._storage )
	{
	}

	SockaddrIn& operator=( const SockaddrIn& other ) noexcept
	{
		_storage = other._storage;
		return *this;
	}

	explicit SockaddrIn( const ::sockaddr_in& native ) noexcept;
	SockaddrIn( mart::nw::uint32_net_t address, mart::nw::uint16_net_t port ) noexcept;

	const ::sockaddr_in& native() const noexcept;
	::sockaddr_in&       native() noexcept;

	mart::nw::uint32_net_t address() const noexcept;
	mart::nw::uint16_net_t port() const noexcept;

private:
	// this should have at least the same size and alignment as the platform's SockaddrIn
	struct alignas( 4 ) Storage {
		char raw_bytes[16];
	};
	Storage _storage {};

	// All constructors forward to this
	explicit SockaddrIn( const SockaddrIn::Storage& src )
		: Sockaddr( mart::nw::socks::Domain::Inet, byte_range_from_pod( _storage ) )
		, _storage( src )
	{
	}
};

struct SockaddrIn6 : mart::nw::socks::Sockaddr {
	SockaddrIn6()
		: SockaddrIn6( Storage {} )
	{
	}
	SockaddrIn6( const SockaddrIn6& other )
		: SockaddrIn6( other._storage )
	{
	}

	SockaddrIn6& operator=( const SockaddrIn6& other )
	{
		_storage = other._storage;
		return *this;
	}

	explicit SockaddrIn6( const ::sockaddr_in6& native );

private:
	// this should have the same size and alignment as the platform's SockaddrIn
	struct alignas( 8 ) Storage {
		char raw_bytes[32];
	};
	Storage _storage {};

	explicit SockaddrIn6( const SockaddrIn6::Storage& src )
		: Sockaddr( mart::nw::socks::Domain::Inet6, byte_range_from_pod( _storage ) )
		, _storage( src )
	{
	}
};
const char* inet_net_to_pres( mart::nw::socks::Domain af, const void* src, char* dst, size_t size );
int         inet_pres_to_net( mart::nw::socks::Domain af, const char* src, void* dst );
} // namespace port_layer
} // namespace socks

} // namespace nw
} // namespace mart

#endif