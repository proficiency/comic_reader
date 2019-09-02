#pragma once
// soon.

// http://www.pinvoke.net/default.aspx/Enums/VirtualKeys.html
enum keys_t
{
	KEY_NONE = 0x0,
	KEY_MOUSE1 = 0X01,
	KEY_MOUSE2 = 0X02,
	KEY_CANCEL = 0X03,
	KEY_MIDDLEBUTTON = 0X04,
	KEY_EXTRABUTTON1 = 0X05,
	KEY_EXTRABUTTON2 = 0X06,
	KEY_BACK = 0X08,
	KEY_TAB = 0X09,
	KEY_CLEAR = 0X0C,
	KEY_RETURN = 0X0D,
	KEY_SHIFT = 0X10,
	KEY_CONTROL = 0X11,
	KEY_MENU = 0X12,
	KEY_PAUSE = 0X13,
	KEY_CAPSLOCK = 0X14,
	KEY_FINAL = 0X18,
	KEY_ESCAPE = 0X1B,
	KEY_CONVERT = 0X1C,
	KEY_NONCONVERT = 0X1D,
	KEY_ACCEPT = 0X1E,
	KEY_MODECHANGE = 0X1F,
	KEY_SPACE = 0X20,
	KEY_PRIOR = 0X21,
	KEY_NEXT = 0X22,
	KEY_END = 0X23,
	KEY_HOME = 0X24,
	KEY_LEFTARROW = 0X25,
	KEY_UPARROW = 0X26,
	KEY_RIGHTARROW = 0X27,
	KEY_DOWNARROW = 0X28,
	KEY_SELECT = 0X29,
	KEY_PRINT = 0X2A,
	KEY_SNAPSHOT = 0X2C,
	KEY_INSERT = 0X2D,
	KEY_DELETE = 0X2E,
	KEY_HELP = 0X2F,
	KEY_N0 = 0X30,
	KEY_N1 = 0X31,
	KEY_N2 = 0X32,
	KEY_N3 = 0X33,
	KEY_N4 = 0X34,
	KEY_N5 = 0X35,
	KEY_N6 = 0X36,
	KEY_N7 = 0X37,
	KEY_N8 = 0X38,
	KEY_N9 = 0X39,
	KEY_A = 0X41,
	KEY_B = 0X42,
	KEY_C = 0X43,
	KEY_D = 0X44,
	KEY_E = 0X45,
	KEY_F = 0X46,
	KEY_G = 0X47,
	KEY_H = 0X48,
	KEY_I = 0X49,
	KEY_J = 0X4A,
	KEY_K = 0X4B,
	KEY_L = 0X4C,
	KEY_M = 0X4D,
	KEY_N = 0X4E,
	KEY_O = 0X4F,
	KEY_P = 0X50,
	KEY_Q = 0X51,
	KEY_R = 0X52,
	KEY_S = 0X53,
	KEY_T = 0X54,
	KEY_U = 0X55,
	KEY_V = 0X56,
	KEY_W = 0X57,
	KEY_X = 0X58,
	KEY_Y = 0X59,
	KEY_Z = 0X5A,
	KEY_LEFTWINDOWS = 0X5B,
	KEY_RIGHTWINDOWS = 0X5C,
	KEY_APPLICATION = 0X5D,
	KEY_SLEEP = 0X5F,
	KEY_NUMPAD0 = 0X60,
	KEY_NUMPAD1 = 0X61,
	KEY_NUMPAD2 = 0X62,
	KEY_NUMPAD3 = 0X63,
	KEY_NUMPAD4 = 0X64,
	KEY_NUMPAD5 = 0X65,
	KEY_NUMPAD6 = 0X66,
	KEY_NUMPAD7 = 0X67,
	KEY_NUMPAD8 = 0X68,
	KEY_NUMPAD9 = 0X69,
	KEY_MULTIPLY = 0X6A,
	KEY_ADD = 0X6B,
	KEY_SEPARATOR = 0X6C,
	KEY_SUBTRACT = 0X6D,
	KEY_DECIMAL = 0X6E,
	KEY_DIVIDE = 0X6F,
	KEY_F1 = 0X70,
	KEY_F2 = 0X71,
	KEY_F3 = 0X72,
	KEY_F4 = 0X73,
	KEY_F5 = 0X74,
	KEY_F6 = 0X75,
	KEY_F7 = 0X76,
	KEY_F8 = 0X77,
	KEY_F9 = 0X78,
	KEY_F10 = 0X79,
	KEY_F11 = 0X7A,
	KEY_F12 = 0X7B,
	KEY_F13 = 0X7C,
	KEY_F14 = 0X7D,
	KEY_F15 = 0X7E,
	KEY_F16 = 0X7F,
	KEY_F17 = 0X80,
	KEY_F18 = 0X81,
	KEY_F19 = 0X82,
	KEY_F20 = 0X83,
	KEY_F21 = 0X84,
	KEY_F22 = 0X85,
	KEY_F23 = 0X86,
	KEY_F24 = 0X87,
	KEY_NUMLOCK = 0X90,
	KEY_SCROLLLOCK = 0X91,
	KEY_NEC_EQUAL = 0X92,
	KEY_LEFTSHIFT = 0XA0,
	KEY_RIGHTSHIFT = 0XA1,
	KEY_LEFTCONTROL = 0XA2,
	KEY_RIGHTCONTROL = 0XA3,
	KEY_LEFTMENU = 0XA4,
	KEY_RIGHTMENU = 0XA5,
	KEY_PROCESSKEY = 0XE5,
	KEY_OEMWSCTRL = 0XEE, // not rly sure what this is but it's probably oem windows key
	KEY_OEMCOPY = 0XF2,
	KEY_PLAY = 0XFA,
	KEY_ZOOM = 0XFB,
	KEY_NONAME = 0XFC,
	KEY_MAX,
};

class c_inputmanager
{
public:
	void register_input( u32 msg, u32 key )
	{
		switch ( msg )
		{
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			{
				if ( valid_key( key ) )
					m_keystates[key] = true;

				break;
			}

			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				if ( valid_key( key ) )
					m_keystates[key] = false;

				break;
			}

			case WM_LBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
			{
				m_keystates[KEY_MOUSE1] = true;
				break;
			}

			case WM_LBUTTONUP:
			{
				m_keystates[KEY_MOUSE1] = false;
				break;
			}

			case WM_RBUTTONDOWN:
			{
				m_keystates[KEY_MOUSE2] = true;
				break;
			}

			case WM_RBUTTONUP:
			{
				m_keystates[KEY_MOUSE2] = false;
				break;
			}

			case WM_XBUTTONDOWN:
			{
				m_keystates[GET_XBUTTON_WPARAM( key )] = true;
				break;
			}

			case WM_XBUTTONUP:
			{
				m_keystates[GET_XBUTTON_WPARAM( key )] = false;
				break;
			}
		}
	}

	bool key_down( int key )
	{
		return m_keystates[key];
	}

	bool valid_key( int key )
	{
		return key > KEY_NONE && key < KEY_MAX;
	}
	
private:
	std::array< bool, KEY_MAX > m_keystates;
}; 

inline c_inputmanager g_inputmanager;