#pragma once

#ifdef __ANDROID__
#include <string>
#include <sstream>
namespace std {
	template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}
#endif

#include <cereal/types/map.hpp>
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/set.hpp"
#include "cereal/archives/binary.hpp"
#include "slices.h"

template <class Archive> inline void serialize(
	Archive& archive, Slice::App::Connection& conn)
{
	archive(conn.ip_local, conn.port_local, conn.ip_remote, conn.port_remote);
}

template <class Archive> inline void serialize(
	Archive& archive, Slice::App::Server& serv)
{
	archive(serv.ip_listen, serv.port_listen);
}

template <class Archive> inline void serialize(
	Archive& archive, Slice::App& app)
{
	archive(app.connections, app.servers, app.files, app.pid);
}

template <class Archive> inline void serialize(
	Archive& archive, Slice& s)
{
	archive(s.apps, s.ifaces);
}

bool inline operator == (const Slice::App::Connection& c1, const Slice::App::Connection& c2) {
	return
		c1.ip_local == c2.ip_local && c1.port_local == c2.port_local &&
		c1.ip_remote == c2.ip_remote && c1.port_remote == c2.port_remote;
}

bool inline operator == (const Slice::App::Server& s1, const Slice::App::Server& s2) {
	return s1.ip_listen == s2.ip_listen && s1.port_listen == s2.port_listen;
}

bool inline operator == (const Slice::App& a1, const Slice::App& a2) {
	return
		a1.connections == a2.connections && a1.servers == a2.servers &&
		a1.files == a2.files && a1.pid == a2.pid;
}

bool inline operator == (const Slice& s1, const Slice& s2) {
	return
		s1.apps == s2.apps && s1.ifaces == s2.ifaces;
}
