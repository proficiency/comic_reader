#include "context.h"

extern long ImGui_ImplWin32_WndProcHandler( HWND hwnd, u32 msg, u32 wParam, long lParam );
long __stdcall wndproc( HWND hwnd, u32 msg, u32 wide_param, long param )
{
	if ( ImGui_ImplWin32_WndProcHandler( hwnd, msg, wide_param, param ) )
		return true;

	switch ( msg )
	{
		case WM_SIZE:
		{
			if ( wide_param != SIZE_MINIMIZED && ( g_render.m_screensize[0] != LOWORD( param ) || g_render.m_screensize[1] != HIWORD( param ) ) )
				g_render.reset( param );

			return false;
		}

		case WM_SYSCOMMAND:
		{
			if ( ( wide_param & 0xfff0 ) == SC_KEYMENU )
				return false;

			break;
		}

		case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return false;
		}
	}

	return DefWindowProcA( hwnd, msg, wide_param, param );
}

int __stdcall WinMain( HINSTANCE instance, HINSTANCE previous_instance, char* cmd_param, int cmd_show )
{
	if ( AllocConsole( ) )
		freopen( "CONOUT$", "w", stdout );

	std::thread( []( ) { ctx.fill_library( ); ctx.m_state = STATE_WAITING; } ).detach( );

	WNDCLASSEX wnd_class =
	{
		sizeof( WNDCLASSEX ),
		CS_HREDRAW | CS_VREDRAW,
		wndproc,
		0,
		0,
		instance,
		LoadIconA( instance, IDI_APPLICATION ),
		LoadCursor( 0, IDC_ARROW ),
		( HBRUSH ) ( COLOR_WINDOW + 1 ),
		0,
		"comic reader",
		LoadIcon( instance, IDI_APPLICATION )
	};

	RegisterClassExA( &wnd_class );

	HWND hwnd = CreateWindowA( "comic reader", "comic reader", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, g_render.m_screensize[0], g_render.m_screensize[1], 0, 0, instance, 0 );

	ShowWindow( hwnd, SW_SHOWDEFAULT );
	UpdateWindow( hwnd );

	if ( ( g_render.m_d3d9 = Direct3DCreate9( D3D_SDK_VERSION ) ) == nullptr )
		return 0;

	g_render.m_params = {};
	g_render.m_params.Windowed = true;
	g_render.m_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_render.m_params.BackBufferFormat = D3DFMT_UNKNOWN;
	g_render.m_params.EnableAutoDepthStencil = true;
	g_render.m_params.AutoDepthStencilFormat = D3DFMT_D16;
	g_render.m_params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if ( g_render.m_d3d9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_render.m_params, &g_render.m_device ) < 0 )
	{
		g_render.m_d3d9->Release( );
		return 0;
	}

	IMGUI_CHECKVERSION( );

	ImGui::CreateContext( );
	{
		ImGuiIO& io = ImGui::GetIO( );
		ImGuiStyle* style = &ImGui::GetStyle( );

		io.Fonts->AddFontFromFileTTF( "c:\\Windows\\Fonts\\Verdana.ttf", 16.0f, 0, io.Fonts->GetGlyphRangesDefault( ) );
		io.Fonts->AddFontFromFileTTF( "c:\\Windows\\Fonts\\Arial.ttf", 16.0f, 0, io.Fonts->GetGlyphRangesDefault( ) );
		io.Fonts->AddFontFromFileTTF( "c:\\Windows\\Fonts\\Tahoma.ttf", 16.0f, 0, io.Fonts->GetGlyphRangesDefault( ) );
		io.Fonts->AddFontFromFileTTF( "c:\\Windows\\Fonts\\comic.ttf", 24.0f, 0, io.Fonts->GetGlyphRangesDefault( ) );

		style->WindowBorderSize = 0.0f;
		style->WindowPadding = ImVec2( 15, 15 );
		style->WindowRounding = 0.0f;
		style->FramePadding = ImVec2( 5, 5 );
		style->FrameRounding = 5.0f;
		style->ItemSpacing = ImVec2( 3, 8 );
		style->ItemInnerSpacing = ImVec2( 8, 6 );
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		ImGui::SetMouseCursor( ImGuiMouseCursor_Arrow );

		ImGui::StyleColorsDark( );
	}

	ImGui_ImplWin32_Init( hwnd );
	ImGui_ImplDX9_Init( g_render.m_device );

	MSG msg{};
	while ( msg.message != WM_QUIT )
	{
		if ( PeekMessageA( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessageA( &msg );
			continue;
		}

		ImGui_ImplDX9_NewFrame( );
		ImGui_ImplWin32_NewFrame( );

		ImGui::NewFrame( );
		ui::draw( );
		ImGui::EndFrame( );

		g_render.m_device->SetRenderState( D3DRS_ZENABLE, false );
		g_render.m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
		g_render.m_device->SetRenderState( D3DRS_SCISSORTESTENABLE, false );
		g_render.m_device->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA( 30, 30, 30, 255 ), 1.0f, 0 );

		if ( g_render.m_device->BeginScene( ) >= 0 )
		{
			ImGui::Render( );
			ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
			g_render.m_device->EndScene( );
		}

		if ( g_render.m_device->Present( 0, 0, 0, 0 ) == D3DERR_DEVICELOST && g_render.m_device->TestCooperativeLevel( ) == D3DERR_DEVICENOTRESET )
			g_render.reset( );
	}

	ImGui_ImplDX9_Shutdown( );
	ImGui_ImplWin32_Shutdown( );
	ImGui::DestroyContext( );

	g_render.release( );

	DestroyWindow( hwnd );
	UnregisterClass( wnd_class.lpszClassName, wnd_class.hInstance );

	return 0;
}