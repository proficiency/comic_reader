#pragma once

class c_jpg 
{
public:
	c_jpg( ) = default;
	c_jpg( const u8a& buf ) : m_buffer( buf ) {	}

	// https://en.wikipedia.org/wiki/JPEG#Syntax_and_structure
	ImVec2 bounds( ) const
	{
		const u8* data = m_buffer.data( );

		// SOI marker, every JPEG will have this
		if ( *( u16* ) data != 0xd8ff )
			return {};

		for ( u32 i = 0; i < 512; ++i )
		{
			// SOF0/ SOF2 markers, every JPEG will have one of the two
			if ( *( u16* ) &data[i] != 0xc0ff && *( u16* ) &data[i] != 0xc2ff )
				continue;


			i += 5;
			return ImVec2( data[i + 2] << 8 | data[i + 3], data[i] << 8 | data[i + 1] );
		}

		return {};
	}

	u8a m_buffer;
};

class c_comic
{
public:
	c_comic( ) = default;

	c_jpg* cover( )
	{
		return &m_images[0];
	}

	std::vector< u8a > data( )
	{
		std::vector< u8a > ret;

		for ( u32 i = 0; i < m_images.size( ); ++i )
			ret.push_back( m_images[i].m_buffer );

		return ret;
	}

	std::string			 m_filename;
	std::string			 m_path;
	std::ifstream		 m_file;
	std::vector< c_jpg > m_images;
};

class c_cbr
{
public:
	c_cbr( ) = default;

	c_cbr( std::string_view path )
	{
		RARHeaderDataEx			header_data		= {};
		RAROpenArchiveDataEx	archive_data	= {};

		archive_data.OpenMode	= RAR_OM_EXTRACT;
		archive_data.ArcName	= ( char* ) path.data( );

		auto archive = RAROpenArchiveEx( &archive_data );

		if ( archive_data.OpenResult != 0 )
		{
			RARCloseArchive( archive );
			util::dbg_print( "failed to open %s\n", path.data( ) );
			return;
		}

		if ( archive != nullptr )
		{
			u32 status = RARReadHeaderEx( archive, &header_data );
			while ( status != ERAR_END_ARCHIVE )
			{
				if ( status == ERAR_BAD_ARCHIVE )
					RARProcessFile( archive, RAR_SKIP, 0, 0 );

				else
					RARProcessFile( archive, RAR_EXTRACT, std::string( "./" ).data( ), 0 );				

				status = RARReadHeaderEx( archive, &header_data );
			}
		}
				
		for ( auto& file : std::experimental::filesystem::directory_iterator( std::experimental::filesystem::current_path( ) ) )
		{
			if ( !std::experimental::filesystem::is_directory( file ) || fnv32::hash( std::string_view( header_data.FileName ) ) != fnv32::hash( file.path( ).filename( ).string( ) ) )
				continue;

			m_comic.m_filename	= file.path( ).filename( ).string( ).c_str( );
			m_comic.m_path		= file.path( ).string( );

			// iterate subdirectory
			for ( std::experimental::filesystem::directory_iterator it( file.path( ) ); it != std::experimental::filesystem::directory_iterator( ); ++it )
			{
				// only get images
				if ( !it->path( ).has_extension( ) )				
					continue;				

				std::basic_ifstream< u8 > file( it->path( ).string( ), std::ios_base::binary );
				m_comic.m_images.push_back( c_jpg( { std::istreambuf_iterator< u8 >( file ), {} } ) );
				file.close( );
			}

			// delete extracted archive now that we've parsed it
			std::experimental::filesystem::remove_all( file.path( ) );
		}

		util::dbg_print( "parsed %i images from %s [RAR]\n", m_comic.m_images.size( ), m_comic.m_filename.c_str( ) );
	}

	c_comic m_comic;
};

class c_cbz
{
public:
	c_cbz( ) = default;

	c_cbz( std::string_view path )
	{
		m_comic.m_path = path.data( );
		miniz_cpp::zip_file zip( m_comic.m_path );

		for ( auto& file : zip.infolist( ) )
		{
			if ( file.filename.find( '.' ) == std::string::npos )
				m_comic.m_filename = file.filename.substr( 0, file.filename.length( ) - 1 );

			if ( !file.file_size )
				continue;

			const std::string data = zip.read( file );			
			m_comic.m_images.push_back( c_jpg( u8a( data.begin( ), data.end( ) ) ) );
		}

		util::dbg_print( "parsed %i images from %s [ZIP]\n", m_comic.m_images.size( ), m_comic.m_filename.c_str( ) );
	}

	c_comic m_comic;
};