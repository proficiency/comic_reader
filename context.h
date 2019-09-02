#pragma once
#include <string>
#include <algorithm>
#include <memory>
#include <fstream>
#include <thread>
#include <array>
#include <experimental/filesystem>

#include <d3d9.h>
#include <d3dx9.h>

#include "unrar/dll.hpp"
#include "miniz-cpp/zip_file.hpp"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "types.h"
#include "hash.h"
#include "input.h"
#include "util.h"
#include "api.h"

enum state_t
{
	STATE_PARSING,
	STATE_WAITING,
	STATE_READING,
};

class c_context
{
public:
	// library
	std::vector< c_comic > m_books;
	
	// user selected entry in library
	int m_selectedbook;

	// state of program
	int m_state;

	void fill_library( )
	{	
		for ( auto& file : std::experimental::filesystem::directory_iterator( std::experimental::filesystem::current_path( ) ) )
		{
			if ( !std::experimental::filesystem::is_regular_file( file ) )
				continue;

			const std::string path = file.path( ).string( ).c_str( );

			switch ( fnv32::hash( file.path( ).extension( ).string( ) ) )
			{
				case fnvc( ".cbz" ):
				case fnvc( ".zip" ):
				{
					m_books.push_back( c_cbz( path ).m_comic );
					break;
				}

				case fnvc( ".cbr" ):
				case fnvc( ".rar" ):
				{
					m_books.push_back( c_cbr( file.path( ).filename( ).string( ) ).m_comic );
					break;
				}
			}
		}
	}
}; inline c_context ctx;

#include "ui.h"