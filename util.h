#pragma once
#include <time.h>

namespace util
{
	inline std::string filename_from_path( std::string_view path )
	{
		std::string ret( path );

		const u32 pos = ret.find_last_of( "\\" );
		if ( pos != std::string::npos )
			ret.erase( 0, pos + 1 );

		return ret;
	}

	inline std::string dbg_time( )
	{
		time_t current_time;
		tm* time_info;
		char time_str[64];

		::time( &current_time );
		time_info = localtime( &current_time );

		strftime( time_str, sizeof( time_str ), "[%D %r] ", time_info );

		return std::string( time_str );
	}

	// formatted print with timestamp
	inline void dbg_print( const char* fmt, ... )
	{
		va_list args;

		va_start( args, fmt );
		vprintf( ( dbg_time( ) + fmt ).c_str( ), args );
		va_end( args );
	}
}