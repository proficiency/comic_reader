#pragma once

class c_render
{
public:
	void reset( long param = 0 )
	{
		if ( !m_device )
			return;

		if ( param != 0 )
		{
			m_params.BackBufferWidth = m_screensize[0] = LOWORD( param );
			m_params.BackBufferHeight = m_screensize[1] = HIWORD( param );
		}

		ImGui_ImplDX9_InvalidateDeviceObjects( );
		m_device->Reset( &m_params );
		ImGui_ImplDX9_CreateDeviceObjects( );
	}

	void release( )
	{
		if ( m_device )
		{
			m_device->Release( );
			m_device = nullptr;
		}

		if ( m_d3d9 )
		{
			m_d3d9->Release( );
			m_d3d9 = nullptr;
		}

		release_textures( );
	}

	void release_textures( )
	{
		for ( auto& texture : m_textures )
		{
			if ( texture )
				texture->Release( );
		}

		m_textures.clear( );
	}

	IDirect3DTexture9* make_textures( const std::vector< c_jpg >& images )
	{
		if ( !m_textures.empty( ) )
			release_textures( );

		for ( auto& image : images )
		{
			auto bounds = image.bounds( );

			m_textures.push_back( nullptr );
			D3DXCreateTextureFromFileInMemoryEx( m_device, &image.m_buffer[0], image.m_buffer.size( ), bounds.x, bounds.y, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &m_textures.back( ) );
		}

		return m_textures.back( );
	}

	IDirect3DTexture9* texture( int page )
	{
		return m_textures[page];
	}

	IDirect3D9*							m_d3d9;
	IDirect3DDevice9*					m_device;
	D3DPRESENT_PARAMETERS				m_params;
	std::vector< IDirect3DTexture9* >	m_textures;
	u32									m_screensize[2] = { 1060, 1000 };
}; inline c_render g_render;

namespace ui
{
	inline void draw( )
	{
		const c_comic* book = &ctx.m_books[ctx.m_selectedbook];

		// setup window
		{
			ImGui::SetNextWindowPos( ImVec2( -3, 0 ) );
			ImGui::SetNextWindowSize( ImVec2( g_render.m_screensize[0] + 3, g_render.m_screensize[1] ) );

			const u32 flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

			if ( book && ctx.m_state == STATE_READING )			
				ImGui::Begin( book->m_filename.c_str( ), 0, flags | ImGuiWindowFlags_MenuBar );

			else
				ImGui::Begin( "comic reader", 0, flags );
		}

		if ( ctx.m_state == STATE_PARSING )	
			ImGui::Text( "loading" );

		if ( ctx.m_state == STATE_WAITING )
		{
			ImGui::Text( "library" );
			ImGui::PushItemWidth( ImGui::GetWindowSize( ).x * 0.60f );
			ImGui::ListBox(
				"",
				&ctx.m_selectedbook,
				[]( void* data, int idx, const char** out_text ) { *out_text = ( ( std::vector< c_comic >* ) data )->at( idx ).m_filename.c_str( ); return true; },
				( void* ) & ctx.m_books,
				std::min( ctx.m_books.size( ), 5u )
			);

			if ( ImGui::Button( "read", ImVec2( ImGui::GetWindowSize( ).x * 0.60f, 27.0f ) ) )
			{
				g_render.make_textures( ctx.m_books[ctx.m_selectedbook].m_images );
				ctx.m_state = STATE_READING;
			}
		}

		// switch to reading menu
		if ( book && ctx.m_state == STATE_READING )
		{
			static u32 page			= 0;
			static u32 last_page	= 0;
			
			// exit to library
			if ( ImGui::BeginMenuBar( ) )
			{
				if ( ImGui::BeginMenu( "library" ) )
				{
					ctx.m_state	 = STATE_WAITING;
					page		 = 0;
					last_page	 = 0;

					ImGui::EndMenu( );
				}

				ImGui::EndMenuBar( );
			}

			ImGui::ProgressBar( ( float ) page / ( float ) ( book->m_images.size( ) - 1 ) );
	
			{
				last_page = page;

				// go to the next page
				if ( page < book->m_images.size( ) - 1 )
				{
					if ( ImGui::Button( "next", ImVec2( ImGui::GetWindowSize( ).x * 0.22f, 27.0f ) ) )
						page++;

					ImGui::SameLine( );
				}

				if ( page > 0 )
				{
					// go to the previous page
					if ( ImGui::Button( "previous", ImVec2( ImGui::GetWindowSize( ).x * 0.22f, 27.0f ) ) )
						page--;

					ImGui::SameLine( );

					// go to the very first page
					if ( ImGui::Button( "first", ImVec2( ImGui::GetWindowSize( ).x * 0.22f, 27.0f ) ) )
						page = 0;

					ImGui::SameLine( );
				}

				// go to the very last page
				if ( page < book->m_images.size( ) - 1 && ImGui::Button( "last", ImVec2( ImGui::GetWindowSize( ).x * 0.22f, 27.0f ) ) )
					page = book->m_images.size( ) - 1;
			}

			ImGui::SameLine( );
			ImGui::Text( "%i/%i", page, book->m_images.size( ) - 1 );
			ImGui::Image( g_render.texture( page ), ImVec2( ImGui::GetWindowSize( ).x - ( ImGui::GetStyle( ).WindowPadding.x * 2 ), ImGui::GetWindowSize( ).y * 0.85f ) );
		}

		ImGui::End( );
	}
};