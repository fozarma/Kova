// Kova -- freestanding native Windows x64 C++ application.
// Built with clang-cl target and lld-link against Win32 import libraries, no CRT or browser runtime.
// Requires Windows 10+; desktop Win32 / GDI+ rendering and standard Windows system DLLs.
// The app is GPU-agnostic. Actual stretched GPU scaling requires the GPU driver's scaling setting.

using U8=unsigned char; using U16=unsigned short; using U32=unsigned int; using I32=int;
using U64=unsigned long long; using S64=long long; using F32=float; using UINT_PTR=U64;
using DWORD=U32; using UINT=U32; using WORD=U16; using BYTE=U8; using BOOL=I32;
using LONG=long; using ULONG_PTR=U64; using DWORD_PTR=U64; using WPARAM=U64; using LPARAM=S64; using LRESULT=S64;
using HANDLE=void*; using HWND=void*; using HINSTANCE=void*; using HICON=void*; using HCURSOR=void*;
using HBRUSH=void*; using HGDIOBJ=void*; using HFONT=void*; using HDC=void*; using HBITMAP=void*;
using HRGN=void*; using HGLOBAL=void*; using LPCWSTR=const wchar_t*; using LPWSTR=wchar_t*; using LPCSTR=const char*;
using WNDPROC=LRESULT(__stdcall*)(HWND,UINT,WPARAM,LPARAM);
#define DLL extern "C" __declspec(dllimport)
#include "dust_preview.h"
#define API __stdcall
#define MAKERESOURCE(i) ((LPCWSTR)((U64)(U16)(i)))

struct POINT{LONG x,y;};
struct RECT{LONG left,top,right,bottom;};
struct MSG{HWND hwnd;UINT message;WPARAM wParam;LPARAM lParam;DWORD time;POINT pt;DWORD lPrivate;};
struct PAINTSTRUCT{HDC hdc;BOOL fErase;RECT rcPaint;BOOL fRestore;BOOL fIncUpdate;BYTE rgbReserved[32];};
struct WNDCLASSEXW{UINT cbSize;UINT style;WNDPROC lpfnWndProc;int cbClsExtra,cbWndExtra;HINSTANCE hInstance;HICON hIcon;HCURSOR hCursor;HBRUSH hbrBackground;LPCWSTR lpszMenuName;LPCWSTR lpszClassName;HICON hIconSm;};
struct BITMAPINFOHEADER{DWORD biSize;LONG biWidth,biHeight;WORD biPlanes,biBitCount;DWORD biCompression,biSizeImage;LONG biXPelsPerMeter,biYPelsPerMeter;DWORD biClrUsed,biClrImportant;};
struct BITMAPINFO{BITMAPINFOHEADER bmiHeader;DWORD bmiColors[3];};
struct GdiplusStartupInput{DWORD GdiplusVersion;void* DebugEventCallback;BOOL SuppressBackgroundThread;BOOL SuppressExternalCodecs;};
struct NOTIFYICONDATAW{DWORD cbSize;HWND hWnd;UINT uID,uFlags,uCallbackMessage;HICON hIcon;wchar_t szTip[128];DWORD dwState,dwStateMask;wchar_t szInfo[256];UINT uTimeoutOrVersion;wchar_t szInfoTitle[64];DWORD dwInfoFlags;BYTE guidItem[16];HICON hBalloonIcon;};
struct DEVMODEW{wchar_t dmDeviceName[32];WORD dmSpecVersion,dmDriverVersion,dmSize,dmDriverExtra;DWORD dmFields;
  union{struct {POINT dmPosition;DWORD dmDisplayOrientation,dmDisplayFixedOutput;} display; BYTE pad[16];} displayOrPrint;
  short dmColor,dmDuplex,dmYResolution,dmTTOption,dmCollate;wchar_t dmFormName[32];WORD dmLogPixels;
  DWORD dmBitsPerPel,dmPelsWidth,dmPelsHeight,dmDisplayFlags,dmDisplayFrequency,dmICMMethod,dmICMIntent,
    dmMediaType,dmDitherType,dmReserved1,dmReserved2,dmPanningWidth,dmPanningHeight;};
static_assert(sizeof(DEVMODEW)==220,"DEVMODEW must match the native Win32 ABI");

// Win32 imports. No MSVC runtime dependency.
DLL void API ExitProcess(UINT);
DLL HINSTANCE API GetModuleHandleW(LPCWSTR);
DLL DWORD API GetModuleFileNameW(HINSTANCE,LPWSTR,DWORD);
DLL HANDLE API CreateFileW(LPCWSTR,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
DLL BOOL API ReadFile(HANDLE,void*,DWORD,DWORD*,void*);
DLL BOOL API WriteFile(HANDLE,const void*,DWORD,DWORD*,void*);
DLL BOOL API CloseHandle(HANDLE);
DLL DWORD API GetLastError();
DLL DWORD API GetTickCount();
DLL BOOL API CopyFileW(LPCWSTR,LPCWSTR,BOOL);
DLL BOOL API DeleteFileW(LPCWSTR);
DLL BOOL API CreateDirectoryW(LPCWSTR,void*);
DLL DWORD API GetEnvironmentVariableW(LPCWSTR,LPWSTR,DWORD);
DLL HGLOBAL API GlobalAlloc(UINT,U64);
DLL void* API GlobalLock(HGLOBAL);
DLL BOOL API GlobalUnlock(HGLOBAL);
DLL HGLOBAL API GlobalFree(HGLOBAL);
DLL U16 API RegisterClassExW(const WNDCLASSEXW*);
DLL HWND API CreateWindowExW(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,void*,HINSTANCE,void*);
DLL LRESULT API DefWindowProcW(HWND,UINT,WPARAM,LPARAM);
DLL BOOL API DestroyWindow(HWND);
DLL BOOL API ShowWindow(HWND,int);
DLL int API GetWindowTextW(HWND,LPWSTR,int);
DLL BOOL API SetWindowTextW(HWND,LPCWSTR);
DLL LRESULT API SendMessageW(HWND,UINT,WPARAM,LPARAM);
DLL BOOL API SetFocus(HWND);
DLL BOOL API UpdateWindow(HWND);
DLL BOOL API GetMessageW(MSG*,HWND,UINT,UINT);
DLL BOOL API TranslateMessage(const MSG*);
DLL LRESULT API DispatchMessageW(const MSG*);
DLL void API PostQuitMessage(int);
DLL BOOL API PostMessageW(HWND,UINT,WPARAM,LPARAM);
DLL BOOL API GetClientRect(HWND,RECT*);
DLL BOOL API GetWindowRect(HWND,RECT*);
DLL BOOL API SetWindowPos(HWND,HWND,int,int,int,int,UINT);
DLL HDC API BeginPaint(HWND,PAINTSTRUCT*);
DLL BOOL API EndPaint(HWND,const PAINTSTRUCT*);
DLL BOOL API InvalidateRect(HWND,const RECT*,BOOL);
DLL UINT_PTR API SetTimer(HWND,UINT_PTR,UINT,void*);
DLL BOOL API KillTimer(HWND,UINT_PTR);
DLL HCURSOR API LoadCursorW(HINSTANCE,LPCWSTR);
DLL HICON API LoadIconW(HINSTANCE,LPCWSTR);
DLL int API GetSystemMetrics(int);
DLL HWND API SetCapture(HWND);
DLL BOOL API ReleaseCapture();
DLL BOOL API RegisterHotKey(HWND,int,UINT,UINT);
DLL BOOL API UnregisterHotKey(HWND,int);
DLL BOOL API SetLayeredWindowAttributes(HWND,DWORD,BYTE,DWORD);
struct SIZE{LONG cx,cy;};
struct BLENDFUNCTION{BYTE BlendOp,BlendFlags,SourceConstantAlpha,AlphaFormat;};
struct TRACKMOUSEEVENT{DWORD cbSize,dwFlags;HWND hwndTrack;DWORD dwHoverTime;};
DLL BOOL API TrackMouseEvent(TRACKMOUSEEVENT*);
DLL BOOL API UpdateLayeredWindow(HWND,HDC,const POINT*,const SIZE*,HDC,const POINT*,DWORD,const BLENDFUNCTION*,DWORD);
DLL int API SetWindowRgn(HWND,HRGN,BOOL);
DLL BOOL API SetProcessDPIAware();
DLL int API MessageBoxW(HWND,LPCWSTR,LPCWSTR,UINT);
DLL int API DrawTextW(HDC,LPCWSTR,int,RECT*,UINT);
DLL DWORD API SetTextColor(HDC,DWORD);
DLL DWORD API SetBkColor(HDC,DWORD);
DLL HBRUSH API CreateSolidBrush(DWORD);
DLL int API SetBkMode(HDC,int);
DLL int API wsprintfA(char*,const char*,...);
DLL BOOL API EnumDisplaySettingsW(LPCWSTR,DWORD,DEVMODEW*);
DLL LONG API ChangeDisplaySettingsExW(LPCWSTR,DEVMODEW*,HWND,DWORD,void*);
DLL BOOL API Shell_NotifyIconW(DWORD,NOTIFYICONDATAW*);
DLL HDC API CreateCompatibleDC(HDC);
DLL BOOL API DeleteDC(HDC);
DLL HBITMAP API CreateDIBSection(HDC,const BITMAPINFO*,UINT,void**,HANDLE,DWORD);
DLL HGDIOBJ API SelectObject(HDC,HGDIOBJ);
DLL BOOL API DeleteObject(HGDIOBJ);
DLL BOOL API BitBlt(HDC,int,int,int,int,HDC,int,int,DWORD);
DLL HFONT API CreateFontW(int,int,int,int,int,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPCWSTR);
DLL HDC API GetDC(HWND);
DLL int API ReleaseDC(HWND,HDC);
DLL HRGN API CreateRoundRectRgn(int,int,int,int,int,int);
DLL BOOL API GetDeviceGammaRamp(HDC,void*);
DLL BOOL API SetDeviceGammaRamp(HDC,const void*);
// Unicode Windows file picker, supplied by COMDLG32.DLL
struct OPENFILENAMEW{DWORD lStructSize;HWND hwndOwner;HINSTANCE hInstance;LPCWSTR lpstrFilter;LPWSTR lpstrCustomFilter;DWORD nMaxCustFilter;DWORD nFilterIndex;LPWSTR lpstrFile;DWORD nMaxFile;LPWSTR lpstrFileTitle;DWORD nMaxFileTitle;LPCWSTR lpstrInitialDir;LPCWSTR lpstrTitle;DWORD Flags;WORD nFileOffset;WORD nFileExtension;LPCWSTR lpstrDefExt;LPARAM lCustData;void* lpfnHook;LPCWSTR lpTemplateName;void* pvReserved;DWORD dwReserved;DWORD FlagsEx;};
static_assert(sizeof(OPENFILENAMEW)==152,"OPENFILENAMEW 64-bit ABI");
DLL BOOL API GetOpenFileNameW(OPENFILENAMEW*);
// Windows Magnification API is GPU-brand independent. A fullscreen 5x5 colour
// transform provides *real* RGB channel mixing (unlike a 1D gamma ramp).
DLL BOOL API MagInitialize();
DLL BOOL API MagUninitialize();
DLL BOOL API MagGetFullscreenColorEffect(float*);
DLL BOOL API MagSetFullscreenColorEffect(const float*);
DLL int API GdipCreateBitmapFromScan0(int,int,int,int,const BYTE*,void**);
// GDI+ flat C API; MS Windows includes gdiplus.dll.
DLL int API GdiplusStartup(ULONG_PTR*,const GdiplusStartupInput*,void*);
DLL void API GdiplusShutdown(ULONG_PTR);
DLL int API GdipCreateFromHDC(HDC,void**);
DLL int API GdipDeleteGraphics(void*);
DLL int API GdipSetSmoothingMode(void*,int);
DLL int API GdipSetPixelOffsetMode(void*,int);
DLL int API GdipGraphicsClear(void*,DWORD);
DLL int API GdipCreateSolidFill(DWORD,void**);
DLL int API GdipDeleteBrush(void*);
DLL int API GdipCreatePen1(DWORD,float,int,void**);
DLL int API GdipDeletePen(void*);
DLL int API GdipFillRectangleI(void*,void*,int,int,int,int);
DLL int API GdipDrawLineI(void*,void*,int,int,int,int);
DLL int API GdipFillEllipseI(void*,void*,int,int,int,int);
DLL int API GdipDrawEllipseI(void*,void*,int,int,int,int);
DLL int API GdipCreatePath(int,void**);
DLL int API GdipDeletePath(void*);
DLL int API GdipAddPathArc(void*,float,float,float,float,float,float);
DLL int API GdipAddPathLine(void*,float,float,float,float);
DLL int API GdipClosePathFigure(void*);
DLL int API GdipFillPath(void*,void*,void*);
DLL int API GdipDrawPath(void*,void*,void*);
DLL int API GdipCreateBitmapFromStream(void*,void**);
DLL int API GdipCreateBitmapFromFile(LPCWSTR,void**);
DLL int API GdipGetImageWidth(void*,UINT*);
DLL int API GdipGetImageHeight(void*,UINT*);
DLL int API GdipBitmapGetPixel(void*,int,int,U32*);
DLL int API GdipDrawImageRectI(void*,void*,int,int,int,int);
DLL int API GdipDisposeImage(void*);
DLL int API GdipSetClipPath(void*,void*,int);
DLL int API GdipResetClip(void*);
DLL LONG API CreateStreamOnHGlobal(HGLOBAL,BOOL,void**);

// Constants
constexpr UINT WM_CREATE=1,WM_DESTROY=2,WM_MOVE=3,WM_SIZE=5,WM_PAINT=15,WM_CLOSE=16,WM_COMMAND=0x111,WM_CTLCOLOREDIT=0x133;
constexpr UINT WM_ERASEBKGND=20,WM_NCHITTEST=132,WM_TIMER=275,WM_HOTKEY=786;
constexpr UINT WM_LBUTTONDOWN=513,WM_LBUTTONUP=514,WM_MOUSEMOVE=512,WM_RBUTTONUP=517,WM_LBUTTONDBLCLK=515;
constexpr UINT WM_KEYDOWN=256,WM_DISPLAYCHANGE=126,WM_MOUSELEAVE=0x2A3,WM_APP=0x8000;
constexpr UINT WM_TRAY=WM_APP+15;
constexpr DWORD WS_POPUP=0x80000000,WS_VISIBLE=0x10000000,WS_MINIMIZEBOX=0x20000,
 WS_CHILD=0x40000000, WS_BORDER=0x800000, WS_SYSMENU=0x80000, ES_NUMBER=0x2000,
 ES_AUTOHSCROLL=0x80, WS_TABSTOP=0x10000, WS_EX_APPWINDOW=0x40000;
constexpr DWORD WS_EX_LAYERED=0x80000,WS_EX_TRANSPARENT=0x20,WS_EX_TOPMOST=0x8,WS_EX_TOOLWINDOW=0x80,WS_EX_NOACTIVATE=0x08000000;
constexpr DWORD LWA_COLORKEY=1,BI_RGB=0,SRCCOPY=0x00CC0020;
constexpr UINT SW_HIDE=0,SW_SHOW=5,SW_RESTORE=9,SW_MINIMIZE=6;
constexpr UINT SWP_NOMOVE=2,SWP_NOSIZE=1,SWP_NOACTIVATE=0x10,SWP_SHOWWINDOW=0x40;
constexpr int SM_CXSCREEN=0,SM_CYSCREEN=1,HTCLIENT=1,HTCAPTION=2;
constexpr DWORD DT_LEFT=0,DT_SINGLELINE=0x20,DT_VCENTER=4,DT_RIGHT=2,DT_CENTER=1,DT_NOPREFIX=0x800;
constexpr DWORD NIM_ADD=0,NIM_MODIFY=1,NIM_DELETE=2,NIF_MESSAGE=1,NIF_ICON=2,NIF_TIP=4;
constexpr DWORD WM_LBUTTONDBLCLK_TRAY=515,WM_LBUTTONUP_TRAY=514;
constexpr DWORD GENERIC_READ=0x80000000,GENERIC_WRITE=0x40000000,FILE_SHARE_READ=1;
constexpr DWORD OPEN_EXISTING=3,CREATE_ALWAYS=2,FILE_ATTRIBUTE_NORMAL=0x80;
constexpr DWORD DM_BITSPERPEL=0x40000,DM_PELSWIDTH=0x80000,DM_PELSHEIGHT=0x100000,DM_DISPLAYFREQUENCY=0x400000;
constexpr DWORD ENUM_CURRENT_SETTINGS=0xFFFFFFFF, ENUM_REGISTRY_SETTINGS=0xFFFFFFFE,
 CDS_UPDATEREGISTRY=1,CDS_TEST=2;
constexpr DWORD VK_F1=0x70,WM_SETFONT=0x30;

// Standard libc primitives, provided inline to avoid pulling in a C/C++ CRT.
extern "C" { int _fltused=0; }
extern "C" float sqrtf(float x){ if(x<=0)return 0;float y=x>=1?x:1;for(int i=0;i<12;i++)y=0.5f*(y+x/y);return y;}
extern "C" void* memset(void* dst,int value,U64 n){U8* p=(U8*)dst;for(U64 i=0;i<n;i++)p[i]=(U8)value;return dst;}
extern "C" void* memcpy(void* dst,const void* src,U64 n){U8* p=(U8*)dst;const U8*q=(const U8*)src;for(U64 i=0;i<n;i++)p[i]=q[i];return dst;}
static int imin(int a,int b){return a<b?a:b;} static int imax(int a,int b){return a>b?a:b;}
static int clamp(int v,int a,int b){return imax(a,imin(v,b));}
static float fclamp(float v,float a,float b){return v<a?a:(v>b?b:v);}
static U32 ARGB(U8 a,U8 r,U8 g,U8 b){return ((U32)a<<24)|((U32)r<<16)|((U32)g<<8)|b;}
static U32 RGBX(U8 r,U8 g,U8 b){return ARGB(255,r,g,b);}
static int absI(int n){return n<0?-n:n;}
static bool hit(int x,int y,int a,int b,int w,int h){return x>=a&&x<a+w&&y>=b&&y<b+h;}
static int strlenA(const char*s){int i=0;while(s[i]&&i<32767)i++;return i;}
static bool strEq(const char*a,const char*b){for(int i=0;;i++){if(a[i]!=b[i])return false;if(!a[i])return true;}}
static void wcopy(wchar_t* dst,const wchar_t*src,int n){int i=0;for(;i<n-1&&src[i];i++)dst[i]=src[i];dst[i]=0;}
static int wlen(const wchar_t*s){int i=0;while(s[i]&&i<500)i++;return i;}
static bool wappend(wchar_t*dst,int cap,const wchar_t*tail){int a=wlen(dst),b=wlen(tail);if(a+b>=cap)return false;for(int j=0;j<=b;j++)dst[a+j]=tail[j];return true;}

// Profiles are deliberately persisted ONLY by an explicit Save button.
// Loading the app merely inspects the slot names, never applies their settings.
struct ProfileData{
 U32 magic,version,bytes;
 wchar_t name[32];
 I32 preset,size,thickness,gap,opacity,color,swatch;
 I32 selectedMode,customW,customH,gameFilter;
 I32 saturation,gamma,shadows,brightness,contrast,colorPreset,desktopColorOn;
 I32 overlayOn,minimizeToTray;
 U32 checksum;
};
static constexpr U32 PROFILE_MAGIC=0x41564F4B; // KOVA
static U32 profileChecksum(const ProfileData& d){
 const U8*bytes=(const U8*)&d;U32 hash=2166136261u;
 for(U32 i=0;i<(U32)sizeof(ProfileData)-4;i++)hash=(hash^bytes[i])*16777619u;
 return hash;
}
static const wchar_t* defaultProfileNames[4]={L"Preset 1",L"Preset 2",L"Preset 3",L"Preset 4"};

struct State{
 HWND hwnd,overlay,editWidth,editHeight,editProfileName; HBRUSH editBrush; HICON icon;
 wchar_t profileDirectory[512],profileFiles[4][512],profilePNGs[4][512];
 ProfileData storedProfiles[4];bool profileExists[4];int activeProfile;
 void* previewImage; void* previewStream; HDC memoryDC; HBITMAP memoryBitmap; HGDIOBJ oldBitmap;
 HFONT fontTitle,fontBody,fontSmall,fontTiny,fontSection,fontSplash;
 ULONG_PTR gdipToken; int w,h, tab, hoverNav,hoverTile,hoverControl,downSlider,dragY,winX,winY;
 int preset,size,thickness,gap,opacity,swatch,color;
 int pickerHue,pickerSat,pickerValue,pickerDrag;
 int saturation,vibrance,highlights,blackLevel;
 bool magReady,magSupported,magModified;
 float originalMatrix[25];
 DWORD lastColorApply,lastGammaApply; int introPhase,introProgress,introFade;
 int customW,customH,red,green,blue,temperature,shadows;
 bool desktopColorOn,minimizeToTray,initializingEditors;
 int brightness,contrast,gamma,colorPreset;
 bool overlayOn,trayShown,mouseDown,customPicker, gammaSupported,hotkeyOK,keepPending,modeChanged,isExpanded,trackingMouse;
 int selectedMode,remaining,resW,resH,desktopW,desktopH;
 U32 modeTimestamp; DEVMODEW startMode,nativeMode,priorMode,pendingMode;
 U16 originalGamma[3][256]; bool savedRamp;
 wchar_t modulePath[512],customPath[512]; wchar_t status[220];
 int gameFilter;
 void* importedImage; int importedW,importedH,importCacheW,importCacheH;bool importedReady;
} g={};

// Art direction: measured against the supplied 1120x634 reference frame.
constexpr int BASE_W=1120,BASE_H=634;
constexpr U32 C_WINDOW=0xFF101113,C_RAIL=0xFF161719,C_HEADER=0xFF161719;
constexpr U32 C_MAIN=0xFF0F1012,C_TILE=0xFF1A1B1E,C_TILE_HOVER=0xFF242529;
constexpr U32 C_WHITE=0xFFF1F2F3,C_DIM=0xFF96999E,C_LINE=0xFF303235;
constexpr U32 C_PREVIEW=0xFF6E7075;
// Vivid crosshair palette; final circle opens the embedded HSV picker.
static U32 swatches[7]={0xFFFF5555,0xFFFFDB40,0xFF65FF63,0xFF25F5FF,0xFF5189FF,0xFFE05CFF,0xFFFFFFFF};
static U32 pickerField[200*124], pickerHueStrip[200*14];
static U32 importedPixels[128*128],candidatePixels[128*128];
struct ResChoice{int w,h;const wchar_t* name;const wchar_t* note;};
static const ResChoice resolutions[15]={
 {0,0,L"Native",L"Monitor default"},
 {1280,960,L"1280 x 960",L"4:3  /  CS2 favourite"},
 {1024,768,L"1024 x 768",L"4:3  /  CS2 classic"},
 {1440,1080,L"1440 x 1080",L"4:3  /  high detail"},
 {1152,864,L"1152 x 864",L"4:3  /  CS2"},
 {1280,1024,L"1280 x 1024",L"5:4  /  CS2"},
 {1600,1200,L"1600 x 1200",L"4:3  /  high detail"},
 {1600,1080,L"1600 x 1080",L"Fortnite  /  medium"},
 {1720,1080,L"1720 x 1080",L"Fortnite  /  mild"},
 {1728,1080,L"1728 x 1080",L"Fortnite  /  mild"},
 {1680,1050,L"1680 x 1050",L"16:10  /  subtle"},
 {1750,1080,L"1750 x 1080",L"Fortnite  /  subtle"},
 {1080,1080,L"1080 x 1080",L"1:1  /  square"},
 {1280,800,L"1280 x 800",L"16:10  /  compact"},
 {1440,900,L"1440 x 900",L"16:10  /  balanced"}
};
// IDs 0..14 retain their v3 meanings, 15 is typed custom, and extra IDs begin at 16.
static const ResChoice additionalResolutions[]={
 {1920,1080,L"1920 x 1080",L"16:9 / native-style"},
 {1920,1200,L"1920 x 1200",L"16:10 / high detail"},
 {1680,1080,L"1680 x 1080",L"14:9 / stretched"},
 {1280,720,L"1280 x 720",L"16:9 / performance"},
 {1366,768,L"1366 x 768",L"16:9 / compact"},
 {1600,900,L"1600 x 900",L"16:9 / performance"},
 {1920,1440,L"1920 x 1440",L"4:3 / high detail"},
 {1280,768,L"1280 x 768",L"5:3 / alternative"},
 {1536,960,L"1536 x 960",L"16:10 / balanced"},
 {1600,1024,L"1600 x 1024",L"25:16 / alternative"},
 {1400,1050,L"1400 x 1050",L"4:3 / classic"},
 {2560,1440,L"2560 x 1440",L"16:9 / high detail"}
};
static const wchar_t* gameNames[8]={L"All",L"CS2",L"VALORANT",L"Fortnite",L"R6 Siege",L"Apex",L"PUBG",L"Overwatch"};
static const wchar_t* gameNotes[8]={
 L"Popular competitive display sizes",L"CS2  /  4:3 and classic stretched formats",
 L"VALORANT  /  supported aspect ratios may show black bars",L"Fortnite  /  stretched and native-style options",
 L"Rainbow Six Siege  /  4:3, 5:4 and 16:10 options",L"Apex Legends  /  16:10 and performance options",
 L"PUBG  /  balanced competitive display formats",L"Overwatch  /  native and performance-oriented options"
};
// Category choices are suggestions, not measurements of player popularity or game guarantees.
static const int gameChoices[8][8]={
 {0,1,2,3,7,9,10,16},
 {1,2,3,4,5,6,16,22},
 {16,1,2,3,6,18,20,26},
 {16,7,8,9,10,11,18,21},
 {16,1,2,3,5,10,17,21},
 {16,10,13,14,17,21,22,19},
 {16,10,17,13,14,1,21,19},
 {16,10,21,14,19,17,3,1}
};
static const ResChoice& getResChoice(int i){return i>=16&&i<=27?additionalResolutions[i-16]:resolutions[clamp(i,0,14)];}
static void invalidate();
static U32 hsvToRGB(int H,int S,int V){
 float h=(float)(H%360)/60.0f, sat=fclamp((float)S/100.0f,0,1),v=fclamp((float)V/100.0f,0,1);
 float c=v*sat,k=h-(float)((int)h/2*2),x=c*(1.0f-((k>1.0f)?k-1.0f:1.0f-k));
 float r=0,green=0,b=0;
 switch((int)h){case 0:r=c;green=x;break;case 1:r=x;green=c;break;
 case 2:green=c;b=x;break;case 3:green=x;b=c;break;
 case 4:r=x;b=c;break;default:r=c;b=x;break;}
 float m=v-c;return RGBX((U8)((r+m)*255.0f+0.5f),(U8)((green+m)*255.0f+0.5f),(U8)((b+m)*255.0f+0.5f));
}
static void rgbToHSV(){
 float r=(float)((g.color>>16)&255)/255.0f, gg=(float)((g.color>>8)&255)/255.0f,b=(float)(g.color&255)/255.0f;
 float hi=r>gg?r:gg;if(b>hi)hi=b;
 float lo=r<gg?r:gg;if(b<lo)lo=b;float d=hi-lo,h=0;
 if(d>0.0001f){if(hi==r){h=(gg-b)/d;if(h<0)h+=6;}else if(hi==gg)h=(b-r)/d+2;else h=(r-gg)/d+4;}
 g.pickerHue=clamp((int)(h*60.0f+0.5f),0,359);
 g.pickerSat=hi>0.0001f?clamp((int)(d/hi*100.0f+0.5f),0,100):0;
 g.pickerValue=clamp((int)(hi*100.0f+0.5f),0,100);
}
static void updatePickerColor(){g.color=(int)(hsvToRGB(g.pickerHue,g.pickerSat,g.pickerValue)&0xffffff);g.swatch=-1;
 if(g.overlayOn&&g.overlay)InvalidateRect(g.overlay,nullptr,0);invalidate();}


static void invalidate(){if(g.hwnd)InvalidateRect(g.hwnd,nullptr,0);}
static int modeW(int m){return m==15?g.customW:(m==0?(int)g.nativeMode.dmPelsWidth:getResChoice(m).w);}
static int modeH(int m){return m==15?g.customH:(m==0?(int)g.nativeMode.dmPelsHeight:getResChoice(m).h);}
static void setStatus(const wchar_t*s){wcopy(g.status,s,220);invalidate();}
static void makePath(){
 DWORD len=GetModuleFileNameW(nullptr,g.modulePath,500);
 wcopy(g.customPath,g.modulePath,512);
 int slash=-1;for(int i=0;i<(int)len;i++)if(g.customPath[i]==L'\\'||g.customPath[i]==L'/')slash=i;
 if(slash>=0)wcopy(g.customPath+slash+1,L"CurrentCrosshair.png",512-slash-1);
 else wcopy(g.customPath,L"CurrentCrosshair.png",512);
}
// App settings are never automatically read from or saved to config.json.
// Only explicit Save Preset writes persistent settings.

// Import a transparent PNG as a true raster crosshair. The 128x128 cache means
// the overlay never performs slow GDI+ file reads while the player is gaming.
static bool loadImportedPNG(){
 void* bitmap=nullptr;
 if(GdipCreateBitmapFromFile(g.customPath,&bitmap)!=0||!bitmap)return false;
 UINT w=0,h=0;
 if(GdipGetImageWidth(bitmap,&w)!=0||GdipGetImageHeight(bitmap,&h)!=0||w==0||h==0||w>4096||h>4096){GdipDisposeImage(bitmap);return false;}
 int cw=(int)(w>=h?128:((U64)w*128+h/2)/h),ch=(int)(h>=w?128:((U64)h*128+w/2)/w);
 cw=clamp(cw,1,128);ch=clamp(ch,1,128);
 bool transparent=false,visible=false;
 for(int y=0;y<ch;y++)for(int x=0;x<cw;x++){
  U32 argb=0;int sx=(int)((U64)x*w/cw),sy=(int)((U64)y*h/ch);
  if(GdipBitmapGetPixel(bitmap,sx,sy,&argb)!=0){GdipDisposeImage(bitmap);return false;}
  candidatePixels[y*cw+x]=argb;
  if(((argb>>24)&255)<240)transparent=true;
  if(((argb>>24)&255)>12)visible=true;
 }
 if(!transparent||!visible){GdipDisposeImage(bitmap);return false;}
 if(g.importedImage)GdipDisposeImage(g.importedImage);
 g.importedImage=bitmap;g.importedW=(int)w;g.importedH=(int)h;
 g.importCacheW=cw;g.importCacheH=ch;g.importedReady=true;
 for(int i=0;i<cw*ch;i++)importedPixels[i]=candidatePixels[i];
 return true;
}
static void importCrosshair(){
 wchar_t path[1024]={};
 OPENFILENAMEW dialog={};dialog.lStructSize=sizeof(dialog);dialog.hwndOwner=g.hwnd;
 dialog.lpstrFilter=L"Transparent PNG (*.png)\0*.png\0All files (*.*)\0*.*\0\0";
 dialog.lpstrFile=path;dialog.nMaxFile=1024;
 dialog.lpstrTitle=L"Import a transparent crosshair PNG";
 dialog.lpstrDefExt=L"png";
 dialog.Flags=0x00080000|0x00001000|0x00000008; // Explorer, file must exist, path must exist
 if(!GetOpenFileNameW(&dialog))return;
 // Check the candidate before replacing the session image, to avoid losing an existing crosshair.
 void* candidate=nullptr;
 if(GdipCreateBitmapFromFile(path,&candidate)!=0||!candidate){setStatus(L"Could not decode this PNG file.");return;}
 UINT iw=0,ih=0;GdipGetImageWidth(candidate,&iw);GdipGetImageHeight(candidate,&ih);
 GdipDisposeImage(candidate);
 if(iw==0||ih==0||iw>4096||ih>4096){setStatus(L"Import a PNG no larger than 4096 x 4096.");return;}
 if(g.importedImage){GdipDisposeImage(g.importedImage);g.importedImage=nullptr;g.importedReady=false;}
 if(!CreateDirectoryW(g.profileDirectory,nullptr)&&GetLastError()!=183){
  setStatus(L"Cannot create the Kova AppData directory for your PNG.");return;
 }
 if(!CopyFileW(path,g.customPath,0)){
  loadImportedPNG();setStatus(L"Could not store the imported PNG in Kova AppData. Check permissions.");return;
 }
 if(loadImportedPNG()){
  g.preset=12;setStatus(L"Custom PNG imported. Size and opacity apply to the overlay.");
  if(g.overlay)InvalidateRect(g.overlay,nullptr,0);
 }else{setStatus(L"Use a transparent PNG with visible crosshair pixels (no solid background).");}
}
static void applyColours(bool force);
static void applyOverlay();
static void revertDisplay();
// Explicit, versioned, integrity-checked saved profile files in LocalAppData.
// Filename is derived from a fixed slot number; users can rename labels safely.
static void prepareProfileStore(){
 wchar_t local[450]={};DWORD n=GetEnvironmentVariableW(L"LOCALAPPDATA",local,440);
 if(n>0&&n<440){
  wcopy(g.profileDirectory,local,512);wappend(g.profileDirectory,512,L"\\Kova");
 }else{
  wcopy(g.profileDirectory,g.customPath,512);
  for(int i=wlen(g.profileDirectory)-1;i>=0;i--)if(g.profileDirectory[i]==L'\\'||g.profileDirectory[i]==L'/'){
   g.profileDirectory[i]=0;break;
  }
 }
 for(int i=0;i<4;i++){
  wcopy(g.profileFiles[i],g.profileDirectory,512);
  wcopy(g.profilePNGs[i],g.profileDirectory,512);
  wchar_t suffix[24]=L"\\Preset_1.kova";
  suffix[8]=(wchar_t)(L'1'+i);
  wappend(g.profileFiles[i],512,suffix);
  wchar_t pngSuffix[24]=L"\\Preset_1.png";
  pngSuffix[8]=(wchar_t)(L'1'+i);
  wappend(g.profilePNGs[i],512,pngSuffix);
 }
}
static bool validProfile(const ProfileData& d){
 return d.magic==PROFILE_MAGIC&&d.version==1&&d.bytes==sizeof(ProfileData)&&
  d.name[31]==0&&d.checksum==profileChecksum(d)&&
  d.preset>=0&&d.preset<=12&&d.size>=1&&d.size<=48&&
  d.thickness>=1&&d.thickness<=12&&d.gap>=0&&d.gap<=24&&
  d.opacity>=10&&d.opacity<=100&&d.selectedMode>=0&&d.selectedMode<=27&&
  d.customW>=640&&d.customW<=7680&&d.customH>=480&&d.customH<=4320&&
  d.gameFilter>=0&&d.gameFilter<=7&&d.saturation>=0&&d.saturation<=200&&
  d.gamma>=75&&d.gamma<=160&&d.shadows>=0&&d.shadows<=100&&
  d.brightness>=70&&d.brightness<=130&&d.contrast>=70&&d.contrast<=130;
}
static void discoverProfiles(){
 for(int i=0;i<4;i++){
  ProfileData d={};DWORD got=0;
  HANDLE f=CreateFileW(g.profileFiles[i],GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
  if(f==(HANDLE)-1)continue;
  BOOL ok=ReadFile(f,&d,sizeof(d),&got,nullptr);CloseHandle(f);
  if(ok&&got==sizeof(d)&&validProfile(d)){g.storedProfiles[i]=d;g.profileExists[i]=true;}
 }
}
static void syncProfileEditor(){
 if(!g.editProfileName)return;
 g.initializingEditors=true;
 int i=g.activeProfile;
 SetWindowTextW(g.editProfileName,g.profileExists[i]?g.storedProfiles[i].name:defaultProfileNames[i]);
 g.initializingEditors=false;
}
static ProfileData captureProfile(){
 ProfileData d={};d.magic=PROFILE_MAGIC;d.version=1;d.bytes=sizeof(d);
 int i=g.activeProfile;
 if(g.editProfileName)GetWindowTextW(g.editProfileName,d.name,32);
 d.name[31]=0;
 if(!d.name[0])wcopy(d.name,defaultProfileNames[i],32);
 d.preset=g.preset;d.size=g.size;d.thickness=g.thickness;d.gap=g.gap;
 d.opacity=g.opacity;d.color=g.color;d.swatch=g.swatch;
 d.selectedMode=g.selectedMode;d.customW=g.customW;d.customH=g.customH;d.gameFilter=g.gameFilter;
 d.saturation=g.saturation;d.gamma=g.gamma;d.shadows=g.shadows;
 d.brightness=g.brightness;d.contrast=g.contrast;d.colorPreset=g.colorPreset;
 d.desktopColorOn=g.desktopColorOn?1:0;d.overlayOn=g.overlayOn?1:0;
 d.minimizeToTray=g.minimizeToTray?1:0;
 d.checksum=profileChecksum(d);return d;
}
static void saveSelectedProfile(){
 int i=g.activeProfile;
 // A custom PNG must travel with its saved slot so another import cannot overwrite it.
 if(!CreateDirectoryW(g.profileDirectory,nullptr)&&GetLastError()!=183){
  setStatus(L"Cannot create the Kova preset directory. Check folder permissions.");return;
 }
 if(g.preset==12){
  if(!g.importedReady||!CopyFileW(g.customPath,g.profilePNGs[i],0)){
   setStatus(L"Could not save the imported PNG with this preset.");return;
  }
 }
 ProfileData d=captureProfile();
 HANDLE f=CreateFileW(g.profileFiles[i],GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(f==(HANDLE)-1){setStatus(L"Preset could not be saved. Check your AppData permissions.");return;}
 DWORD written=0;BOOL ok=WriteFile(f,&d,sizeof(d),&written,nullptr);CloseHandle(f);
 if(!ok||written!=sizeof(d)){setStatus(L"Preset save failed. Try again.");return;}
 if(d.preset!=12)DeleteFileW(g.profilePNGs[i]);
 g.storedProfiles[i]=d;g.profileExists[i]=true;
 setStatus(L"Preset saved. Kova will not load it automatically at startup.");
}
static void editResolutionFields(){
 if(!g.editWidth||!g.editHeight)return;
 g.initializingEditors=true;
 wchar_t chars[16];int values[2]={g.customW,g.customH};
 HWND edits[2]={g.editWidth,g.editHeight};
 for(int j=0;j<2;j++){
  int v=values[j],n=0;wchar_t backwards[16];
  do{backwards[n++]=(wchar_t)(L'0'+v%10);v/=10;}while(v&&n<15);
  for(int k=0;k<n;k++)chars[k]=backwards[n-1-k];chars[n]=0;
  SetWindowTextW(edits[j],chars);
 }
 g.initializingEditors=false;
}
static void loadSelectedProfile(){
 int i=g.activeProfile;
 if(!g.profileExists[i]){setStatus(L"This slot is empty. Click Save Preset first.");return;}
 if(g.keepPending){setStatus(L"Confirm or revert your pending resolution before loading a preset.");return;}
 const ProfileData& d=g.storedProfiles[i];
 if(!validProfile(d)){setStatus(L"Saved preset is damaged or incompatible.");return;}
 bool customReady=true;
 if(d.preset==12){
  // GDI+ keeps Bitmap::FromFile open until disposed; release it before overwrite.
  if(g.importedImage){GdipDisposeImage(g.importedImage);g.importedImage=nullptr;g.importedReady=false;}
  if(!CopyFileW(g.profilePNGs[i],g.customPath,0))customReady=false;
  else customReady=loadImportedPNG();
 }
 g.preset=(d.preset==12&&!customReady)?1:d.preset;
 g.size=d.size;g.thickness=d.thickness;g.gap=d.gap;g.opacity=d.opacity;
 g.color=d.color&0xffffff;g.swatch=clamp(d.swatch,-1,6);rgbToHSV();
 g.selectedMode=d.selectedMode;g.customW=d.customW;g.customH=d.customH;
 g.gameFilter=d.gameFilter;g.saturation=d.saturation;g.gamma=d.gamma;
 g.shadows=d.shadows;g.brightness=d.brightness;g.contrast=d.contrast;
 g.colorPreset=clamp(d.colorPreset,-1,2);
 g.desktopColorOn=d.desktopColorOn!=0;g.overlayOn=d.overlayOn!=0;
 g.minimizeToTray=d.minimizeToTray!=0;
 editResolutionFields();applyColours(true);applyOverlay();
 if(!customReady)setStatus(L"Preset loaded, but its custom PNG is missing. Default crosshair selected.");
 else setStatus(L"Preset loaded. To change display resolution, press Apply on the Display tab.");
}
static void deleteSelectedProfile(){
 int i=g.activeProfile;if(!g.profileExists[i]){setStatus(L"This preset slot is already empty.");return;}
 if(MessageBoxW(g.hwnd,L"Delete this saved preset? The current session will not change.",L"Kova - Delete preset",0x00000004|0x00000030)!=6)return;
 if(!DeleteFileW(g.profileFiles[i])){setStatus(L"Couldn't delete the preset file.");return;}
 DeleteFileW(g.profilePNGs[i]);g.profileExists[i]=false;
 memset(&g.storedProfiles[i],0,sizeof(ProfileData));syncProfileEditor();
 setStatus(L"Saved preset deleted. Current session settings were kept.");
}
static void resetSession(){
 if(g.keepPending)revertDisplay();
 g.preset=1;g.size=19;g.thickness=5;g.gap=1;g.opacity=100;g.color=0x59FF69;
 g.swatch=-1;rgbToHSV();g.selectedMode=0;g.customW=1440;g.customH=1080;
 g.gameFilter=0;g.saturation=100;g.gamma=100;g.shadows=0;
 g.brightness=100;g.contrast=100;g.colorPreset=0;g.desktopColorOn=false;
 g.overlayOn=false;g.minimizeToTray=false;
 editResolutionFields();applyColours(true);applyOverlay();
 setStatus(L"Session settings reset. Your saved presets were not changed.");
}

// GDI+ helpers. All components are drawn to one off-screen DIB, copied in a single BitBlt.
struct Painter{void* gfx;};
static void solidRect(Painter&p,int x,int y,int w,int h,U32 c){void*b=nullptr;GdipCreateSolidFill(c,&b);if(b){GdipFillRectangleI(p.gfx,b,x,y,w,h);GdipDeleteBrush(b);}}
static void rrPath(void**path,float x,float y,float w,float h,float r){
 GdipCreatePath(0,path);if(!*path)return;
 r=fclamp(r,0,fclamp(w/2,0,h/2));
 GdipAddPathArc(*path,x,y,r*2,r*2,180,90);GdipAddPathArc(*path,x+w-2*r,y,r*2,r*2,270,90);
 GdipAddPathArc(*path,x+w-2*r,y+h-2*r,r*2,r*2,0,90);GdipAddPathArc(*path,x,y+h-2*r,r*2,r*2,90,90);
 GdipClosePathFigure(*path);
}
static void roundRect(Painter&p,int x,int y,int w,int h,int radius,U32 c){void*path=nullptr;rrPath(&path,(float)x,(float)y,(float)w,(float)h,(float)radius);if(!path)return;void*b=nullptr;GdipCreateSolidFill(c,&b);if(b){GdipFillPath(p.gfx,b,path);GdipDeleteBrush(b);}GdipDeletePath(path);}
static void strokedRoundRect(Painter&p,int x,int y,int w,int h,int radius,U32 c,float width){void*path=nullptr;rrPath(&path,(float)x,(float)y,(float)w,(float)h,(float)radius);if(!path)return;void*pen=nullptr;GdipCreatePen1(c,width,2,&pen);if(pen){GdipDrawPath(p.gfx,pen,path);GdipDeletePen(pen);}GdipDeletePath(path);}
static void circle(Painter&p,int x,int y,int radius,U32 c){void*b=nullptr;GdipCreateSolidFill(c,&b);if(b){GdipFillEllipseI(p.gfx,b,x-radius,y-radius,2*radius,2*radius);GdipDeleteBrush(b);}}
static void ring(Painter&p,int x,int y,int radius,U32 c,float width){void*pen=nullptr;GdipCreatePen1(c,width,2,&pen);if(pen){GdipDrawEllipseI(p.gfx,pen,x-radius,y-radius,2*radius,2*radius);GdipDeletePen(pen);}}
static void line(Painter&p,int x1,int y1,int x2,int y2,U32 c,float w){void*pen=nullptr;GdipCreatePen1(c,w,2,&pen);if(pen){GdipDrawLineI(p.gfx,pen,x1,y1,x2,y2);GdipDeletePen(pen);}}
// Store text during vector drawing and flush only after GDI+ releases the HDC.
// This avoids undefined GDI / GDI+ paint ordering and prevents missing labels.
struct TextCommand{HFONT font;wchar_t value[256];int x,y,w,h;U32 argb;UINT align;};
static TextCommand queuedText[160];static int textCount=0;
static void text(HDC dc,HFONT font,const wchar_t*s,int x,int y,int w,int h,U32 argb,UINT align=DT_LEFT){
 (void)dc;if(!s||textCount>=160)return;
 TextCommand &t=queuedText[textCount++];t.font=font;t.x=x;t.y=y;t.w=w;t.h=h;t.argb=argb;t.align=align;wcopy(t.value,s,256);
}
static void drawQueuedText(HDC dc){
 for(int i=0;i<textCount;i++){
  TextCommand &t=queuedText[i];SelectObject(dc,t.font);SetBkMode(dc,1);
  SetTextColor(dc,((t.argb&255)<<16)|(t.argb&0x00FF00)|((t.argb>>16)&255));
  RECT r={(LONG)t.x,(LONG)t.y,(LONG)(t.x+t.w),(LONG)(t.y+t.h)};
  DrawTextW(dc,t.value,-1,&r,DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|t.align);
 }
 textCount=0;
}
static void numText(HDC dc,HFONT font,int v,int x,int y,int w,int h,U32 color,UINT align=DT_RIGHT){
 wchar_t b[20];int q=v,i=0;bool neg=q<0;if(neg)q=-q;
 do{b[i++]=L'0'+(q%10);q/=10;}while(q&&i<16);if(neg)b[i++]=L'-';
 for(int a=0;a<i/2;a++){wchar_t c=b[a];b[a]=b[i-a-1];b[i-a-1]=c;}b[i]=0;
 text(dc,font,b,x,y,w,h,color,align);
}
static void brand(Painter&p,int x,int y){
 // Matching stylised H glyph used by the executable and taskbar icon.
 roundRect(p,x-13,y-13,26,26,5,0xFF25272C);
 line(p,x-9,y-9,x-3,y,C_WHITE,3.5f);
 line(p,x-3,y,x-9,y+9,C_WHITE,3.5f);
 line(p,x+9,y-9,x+3,y,C_WHITE,3.5f);
 line(p,x+3,y,x+9,y+9,C_WHITE,3.5f);
 line(p,x-3,y,x+3,y,C_WHITE,3.5f);
 circle(p,x+9,y-9,2,0xFF29CC79);
}
static void navIcon(Painter&p,int type,int x,int y,U32 c){
 if(type==0){ring(p,x,y,11,c,1.7f);ring(p,x,y,3,c,1.4f);line(p,x-15,y,x-6,y,c,1.6f);line(p,x+6,y,x+15,y,c,1.6f);line(p,x,y-15,x,y-6,c,1.6f);line(p,x,y+6,x,y+15,c,1.6f);}
 if(type==1){strokedRoundRect(p,x-13,y-11,26,20,2,c,1.4f);line(p,x,y+9,x,y+13,c,1.6f);line(p,x-8,y+13,x+8,y+13,c,1.6f);}
 if(type==2){ring(p,x,y,12,c,1.5f);circle(p,x-6,y-3,2,c);circle(p,x+2,y-7,2,c);circle(p,x+7,y,2,c);ring(p,x+1,y+7,2,c,1.5f);}
 if(type==3){
  ring(p,x,y,9,c,1.6f);ring(p,x,y,3,c,1.4f);
  for(int i=0;i<8;i++){
    // Cardinal + diagonal teeth without jittering or translation on hover.
    int dx[8]={0,7,11,7,0,-7,-11,-7},dy[8]={-11,-7,0,7,11,7,0,-7};
    int ex[8]={0,9,15,9,0,-9,-15,-9},ey[8]={-15,-9,0,9,15,9,0,-9};
    line(p,x+dx[i],y+dy[i],x+ex[i],y+ey[i],c,2.1f);
  }
 }
}
static void drawCross(Painter&p,int style,int x,int y,int size,int thick,int gap,U32 color,bool miniature){
 int L=miniature?15:clamp(size*2,3,100);
 int T=miniature?2:clamp(thick,1,12);
 int G=miniature?4:clamp(gap+4,1,30);
 int R=miniature?5:clamp((size+thick+2)/5,1,17);
 if(style==0){circle(p,x,y,R,color);return;} // scalable dot
 if(style==1||style==3||style==5||style==11){
  line(p,x-L,y,x-G,y,color,(float)T);line(p,x+G,y,x+L,y,color,(float)T);
  line(p,x,y+G,x,y+L,color,(float)T);
  if(style!=5)line(p,x,y-L,x,y-G,color,(float)T);
  if(style==3||style==11)circle(p,x,y,miniature?2:clamp(T,1,6),color);
  if(style==11)ring(p,x,y,miniature?9:clamp(size/2,4,24),color,miniature?1.0f:(float)T);
  return;
 }
 if(style==2){ring(p,x,y,miniature?10:clamp(size,3,48),color,(float)T);return;}
 if(style==4){ring(p,x,y,miniature?10:clamp(size,3,48),color,(float)T);circle(p,x,y,miniature?2:clamp(T,1,6),color);return;}
 if(style==6){ // upward chevron
  line(p,x-L,y+L/2,x,y-L/2,color,(float)T);
  line(p,x,y-L/2,x+L,y+L/2,color,(float)T);return;
 }
 if(style==7){ // frame
  int r=miniature?12:clamp(size+3,4,48);
  line(p,x-r,y-r,x+r,y-r,color,(float)T);line(p,x+r,y-r,x+r,y+r,color,(float)T);
  line(p,x+r,y+r,x-r,y+r,color,(float)T);line(p,x-r,y+r,x-r,y-r,color,(float)T);return;
 }
 if(style==8){ // four bracket corners
  int a=miniature?8:clamp(size/2+3,4,22),b=miniature?15:clamp(size+6,9,50);
  for(int dx=-1;dx<=1;dx+=2)for(int dy=-1;dy<=1;dy+=2){
   line(p,x+dx*a,y+dy*b,x+dx*b,y+dy*b,color,(float)T);
   line(p,x+dx*b,y+dy*a,x+dx*b,y+dy*b,color,(float)T);}
  return;
 }
 if(style==9){ // diamond
  int r=miniature?14:clamp(size+3,5,55);
  line(p,x,y-r,x+r,y,color,(float)T);line(p,x+r,y,x,y+r,color,(float)T);
  line(p,x,y+r,x-r,y,color,(float)T);line(p,x-r,y,x,y-r,color,(float)T);return;
 }
 if(style==10){ // horizontal bar
  line(p,x-L,y,x+L,y,color,(float)T);return;
 }
}

static void slider(Painter&p,HDC dc,const wchar_t*label,int value,int min,int max,int x,int y,int width){
 text(dc,g.fontBody,label,x,y,width-55,24,C_WHITE);
 numText(dc,g.fontBody,value,x,y,width,24,C_DIM);
 int trackY=y+41, sliderW=width;
 roundRect(p,x,trackY,sliderW,3,2,0xFF35363A);
 int thumb=x+(value-min)*sliderW/(max-min);
 roundRect(p,x,trackY,imax(2,thumb-x),3,2,0xFFE5E6E9);
 circle(p,thumb,trackY+1,10,0xFF2A2C31);
 circle(p,thumb,trackY+1,6,0xFFA0A3AB);
}
static void button(Painter&p,HDC dc,int x,int y,int w,int h,const wchar_t*s,bool active=false,bool danger=false){
 U32 base=active?0xFFE3E5E9:((g.hoverControl>=0)?C_TILE:C_TILE);
 if(danger)base=0xFF37292C;
 roundRect(p,x,y,w,h,9,base);text(dc,g.fontSection,s,x+10,y,w-20,h,active?0xFF17191C:C_WHITE,DT_CENTER);
}
static void drawSplash(Painter&p,HDC dc){
 solidRect(p,0,0,g.w,g.h,0xFF0E1012);
 // Subtle, architectural geometry; no bloom, jitter or noisy particles.
 roundRect(p,1,1,g.w-2,g.h-2,21,0xFF111315);
 strokedRoundRect(p,1,1,g.w-2,g.h-2,21,0xFF393C40,1.0f);
 // Dark matte header and user-controlled close/minimize.
 solidRect(p,17,19,g.w-34,39,0xFF111315);
 text(dc,g.fontSection,L"Kova",36,25,180,31,0xFFE4E6E7);
 circle(p,g.w-137,39,4,0xFF42CC87);
 line(p,g.w-99,40,g.w-82,40,0xFFD2D4D6,1.3f);
 line(p,g.w-41,32,g.w-27,46,0xFFD2D4D6,1.4f);
 line(p,g.w-27,32,g.w-41,46,0xFFD2D4D6,1.4f);
 // Brand mark with generous negative space.
 roundRect(p,527,157,66,66,17,0xFF222528);
 line(p,545,175,558,190,0xFFF3F4F4,5.1f);
 line(p,558,190,545,206,0xFFF3F4F4,5.1f);
 line(p,575,175,562,190,0xFFF3F4F4,5.1f);
 line(p,562,190,575,206,0xFFF3F4F4,5.1f);
 line(p,557,190,563,190,0xFFF3F4F4,4.0f);
 circle(p,578,173,4,0xFF35D187);
 text(dc,g.fontSplash,L"Kova",350,252,420,73,0xFFF3F4F4,DT_CENTER);
 text(dc,g.fontSmall,L"GAMING DISPLAY UTILITY",350,331,420,28,0xFF95999B,DT_CENTER);
 if(g.introPhase==0){
  roundRect(p,415,403,290,3,2,0xFF33363A);
  roundRect(p,415,403,imax(2,g.introProgress*290/100),3,2,0xFFE7E9EA);
  text(dc,g.fontSmall,L"INITIALISING",429,420,262,27,0xFF8C9094,DT_CENTER);
 }else{
  roundRect(p,440,395,240,49,11,g.introPhase==1?0xFFE7E9EB:0xFF777B80);
  text(dc,g.fontSection,g.introPhase==1?L"Enter Kova":L"Opening",440,399,240,40,0xFF161719,DT_CENTER);
  text(dc,g.fontSmall,L"READY",461,462,198,24,0xFF8E9696,DT_CENTER);
 }
 text(dc,g.fontTiny,L"CROSSHAIR  /  DISPLAY  /  VISIBILITY",339,572,442,23,0xFF666A6D,DT_CENTER);
}
static void drawFrame(Painter&p){
 solidRect(p,0,0,g.w,g.h,C_WINDOW);
 solidRect(p,0,0,67,g.h,C_RAIL);
 solidRect(p,67,0,g.w-67,56,C_HEADER);
 solidRect(p,67,56,g.w-67,g.h-56,C_MAIN);
 line(p,67,56,67,g.h,0xFF24272A,1);
 line(p,2,54,g.w-3,54,0xFF202225,1);
 strokedRoundRect(p,1,1,g.w-2,g.h-2,20,0xFF3A3B3F,1.0f);
 brand(p,34,28);
 int iconY[4]={106,176,246,316};
 for(int i=0;i<4;i++){
  if(i==g.tab)roundRect(p,2,85+70*i,3,39,2,0xFFEBEDEF);
  U32 c=i==g.tab?0xFFF5F6F7:(i==g.hoverNav?0xFFCACED0:0xFF80858A);
  if(i==g.hoverNav&&i!=g.tab)roundRect(p,15,88+70*i,39,39,8,0xFF24272A);
  navIcon(p,i,34,iconY[i],c);
 }
 text(g.memoryDC,g.fontSection,L"Kova",89,12,100,32,0xFFECEDEF);
 circle(p,g.w-136,28,5,0xFF39C87D);
 if(g.hoverControl==200)roundRect(p,g.w-118,7,55,43,6,0xFF242629);
 if(g.hoverControl==202)roundRect(p,g.w-61,7,55,43,6,0xFF33272A);
 line(p,g.w-99,29,g.w-81,29,C_WHITE,1.2f);
 line(p,g.w-41,21,g.w-26,36,C_WHITE,1.4f);
 line(p,g.w-26,21,g.w-41,36,C_WHITE,1.4f);
}
static void drawCrosshairTab(Painter&p,HDC dc){
 text(dc,g.fontTitle,L"Crosshair",115,83,700,51,C_WHITE);
 text(dc,g.fontSmall,L"Choose a style and fine-tune your overlay",115,132,700,28,C_DIM);
 // Twelve genuinely different designs, in a compact four-row grid.
 for(int i=0;i<12;i++){
  int tx=115+(i%3)*97,ty=175+(i/3)*89;
  roundRect(p,tx,ty,88,80,9,i==g.preset?0xFF303438:(i==g.hoverTile?C_TILE_HOVER:C_TILE));
  if(i==g.preset)roundRect(p,tx+32,ty+75,24,2,1,0xFFE3E6E8);
  drawCross(p,i,tx+44,ty+40,20,3,4,0xFFF5F5F6,true);
 }
 // Import/remove actions fit under the preset grid, with no extra bulky panel.
 roundRect(p,115,544,178,35,8,g.preset==12?0xFF33383A:0xFF26292C);
 text(dc,g.fontSmall,g.importedReady?L"Import / Replace PNG":L"Import crosshair PNG",127,547,166,29,C_WHITE);
 roundRect(p,304,544,91,35,8,0xFF202225);
 text(dc,g.fontSmall,L"Use presets",308,547,82,29,C_DIM,DT_CENTER);
 // Reference screenshot scene is embedded directly into the EXE (no external PNG needed).
 roundRect(p,443,175,353,366,10,0xFF515156);
 if(g.previewImage){void* mask=nullptr;rrPath(&mask,443.0f,175.0f,353.0f,366.0f,10.0f);
  if(mask){GdipSetClipPath(p.gfx,mask,0);}
  GdipDrawImageRectI(p.gfx,g.previewImage,443,175,353,366);
  if(mask){GdipResetClip(p.gfx);GdipDeletePath(mask);}
 }
 strokedRoundRect(p,443,175,353,366,10,0xFF55585B,1.0f);
 U32 c=ARGB((U8)(g.opacity*255/100),(U8)((g.color>>16)&255),(U8)((g.color>>8)&255),(U8)(g.color&255));
 if(g.preset==12&&g.importedReady){
  int biggest=clamp(g.size*3,6,144),wide=imax(1,biggest*g.importCacheW/imax(1,imax(g.importCacheW,g.importCacheH)));
  int tall=imax(1,biggest*g.importCacheH/imax(1,imax(g.importCacheW,g.importCacheH)));
  GdipDrawImageRectI(p.gfx,g.importedImage,620-wide/2,358-tall/2,wide,tall);
 }else drawCross(p,g.preset,620,358,g.size,g.thickness,g.gap,c,false);
 U32 mark=0xFFCDD0D1;
 line(p,457,189,457,218,mark,2);line(p,457,189,485,189,mark,2);
 line(p,781,189,781,218,mark,2);line(p,753,189,781,189,mark,2);
 line(p,457,525,457,496,mark,2);line(p,457,525,485,525,mark,2);
 line(p,781,525,781,496,mark,2);line(p,753,525,781,525,mark,2);
 int sx=839,sw=230;
 slider(p,dc,L"Size",g.size,1,48,sx,174,sw);
 slider(p,dc,L"Thickness",g.thickness,1,12,sx,250,sw);
 slider(p,dc,L"Gap",g.gap,0,24,sx,326,sw);
 slider(p,dc,L"Opacity",g.opacity,10,100,sx,402,sw);
 for(int i=0;i<7;i++){
   int cx=851+i*34,cy=504;
   circle(p,cx,cy,11,i==6?(0xFF000000|g.color):swatches[i]);
   if(i==6){ring(p,cx,cy,14,0xFF8E949A,1.1f);line(p,cx-4,cy,cx+4,cy,C_WHITE,1.1f);line(p,cx,cy-4,cx,cy+4,C_WHITE,1.1f);}
   if(g.swatch==i||(i==6&&g.customPicker))ring(p,cx,cy,15,0xFFE5E7EB,1.4f);
 }
 // The overlay switch is on the Crosshair screen rather than hidden in Settings.
 text(dc,g.fontSection,L"In-game overlay",839,545,165,28,C_WHITE);
 roundRect(p,1006,548,58,28,14,g.overlayOn?0xFF627D69:0xFF3A3D42);
 circle(p,g.overlayOn?1050:1021,562,10,0xFFF1F2F3);
 text(dc,g.fontSmall,L"F1 to toggle",839,579,210,24,C_DIM);
 if(g.customPicker){
  // Intentionally overlaid inside the screenshot preview, not a Windows chooser.
  roundRect(p,551,274,243,248,11,0xFF222428);
  strokedRoundRect(p,551,274,243,248,11,0xFF52565B,1);
  text(dc,g.fontSection,L"Custom colour",566,285,210,29,C_WHITE);
  for(int py=0;py<124;py++)for(int px=0;px<200;px++){
   pickerField[py*200+px]=hsvToRGB(g.pickerHue,px*100/199,(123-py)*100/123);
  }
  for(int px=0;px<200;px++)for(int py=0;py<14;py++)pickerHueStrip[py*200+px]=hsvToRGB(px*359/199,100,100);
  void* gradient=nullptr;void* hue=nullptr;
  if(GdipCreateBitmapFromScan0(200,124,200*4,0x26200A,(const BYTE*)pickerField,&gradient)==0){
   GdipDrawImageRectI(p.gfx,gradient,566,321,200,124);GdipDisposeImage(gradient);}
  if(GdipCreateBitmapFromScan0(200,14,200*4,0x26200A,(const BYTE*)pickerHueStrip,&hue)==0){
   GdipDrawImageRectI(p.gfx,hue,566,455,200,14);GdipDisposeImage(hue);}
  int crossx=566+g.pickerSat*199/100,crossy=321+(100-g.pickerValue)*123/100;
  ring(p,crossx,crossy,6,0xFF0A0A0C,2);ring(p,crossx,crossy,4,0xFFFFFFFF,1);
  int hx=566+g.pickerHue*199/359;line(p,hx,452,hx,472,C_WHITE,2);
  circle(p,581,492,12,0xFF000000|g.color);
  wchar_t hex[12]=L"#000000";const wchar_t* digits=L"0123456789ABCDEF";
  for(int j=0;j<6;j++)hex[j+1]=digits[(g.color>>(20-j*4))&15];
  text(dc,g.fontSection,hex,604,478,136,31,C_WHITE);
  text(dc,g.fontTiny,L"Drag to pick  \u00b7  Esc to close",566,501,208,20,C_DIM);
 }

}
static void drawResolutionTab(Painter&p,HDC dc){
 text(dc,g.fontTitle,L"Game resolutions",115,81,650,56,C_WHITE);
 text(dc,g.fontSmall,L"Choose a game, select a size, then apply",115,132,690,27,C_DIM);
 text(dc,g.fontTiny,L"CURRENT",833,130,82,26,C_DIM);
 numText(dc,g.fontSection,g.desktopW,905,130,68,26,C_WHITE);
 text(dc,g.fontTiny,L"x",975,132,14,22,C_DIM);
 numText(dc,g.fontSection,g.desktopH,995,130,73,26,C_WHITE);
 // Game selector: compact flat chips, fixed positions for predictable mouse targeting.
 for(int i=0;i<8;i++){
  int x=115+i*120,y=182;
  bool active=(g.gameFilter==i);
  roundRect(p,x,y,111,37,8,active?0xFFE5E7E8:(0xFF1D1F22));
  text(dc,g.fontSmall,gameNames[i],x+3,y+6,105,25,active?0xFF16191A:C_DIM,DT_CENTER);
 }
 text(dc,g.fontSmall,gameNotes[g.gameFilter],116,232,929,26,C_DIM);
 line(p,115,267,1069,267,0xFF2F3235,1);
 // Eight curated modes shown for the selected game. Selecting doesn't change
 // the desktop until the Apply button is pressed.
 for(int slot=0;slot<8;slot++){
  int id=gameChoices[g.gameFilter][slot],x=115+(slot%4)*241,y=285+(slot/4)*78;
  bool selected=g.selectedMode==id;
  roundRect(p,x,y,229,69,9,selected?0xFF2B2E31:(g.hoverTile==slot?C_TILE_HOVER:0xFF1A1C1F));
  if(selected)roundRect(p,x+1,y+17,3,35,2,0xFFE9EAEB);
  const ResChoice &r=getResChoice(id);
  text(dc,g.fontSection,r.name,x+15,y+8,208,30,C_WHITE);
  text(dc,g.fontTiny,r.note,x+15,y+37,205,23,C_DIM);
 }
 line(p,115,452,1069,452,0xFF303337,1);
 text(dc,g.fontSection,L"Custom resolution",125,466,228,30,C_WHITE);
 text(dc,g.fontTiny,L"Driver-supported display modes only",125,501,310,21,C_DIM);
 text(dc,g.fontTiny,L"WIDTH",594,464,97,22,C_DIM);
 text(dc,g.fontTiny,L"HEIGHT",750,464,97,22,C_DIM);
 line(p,589,520,709,520,g.selectedMode==15?0xFFE2E4E5:0xFF565B60,1);
 line(p,745,520,865,520,g.selectedMode==15?0xFFE2E4E5:0xFF565B60,1);
 text(dc,g.fontBody,L"x",722,485,17,30,C_DIM);
 text(dc,g.fontTiny,L"TYPE YOUR OWN",889,483,166,25,C_DIM);
 button(p,dc,115,540,226,46,L"Apply",true);
 button(p,dc,355,540,226,46,L"Reset to native");
 if(g.keepPending){
  text(dc,g.fontSection,L"Keep new resolution?",603,533,301,31,C_WHITE);
  numText(dc,g.fontSection,g.remaining,1024,533,33,31,0xFFF0C6B1);
  button(p,dc,602,568,213,28,L"Keep",true);
  button(p,dc,827,568,226,28,L"Revert");
 }else{
  int chosenW=modeW(g.selectedMode),chosenH=modeH(g.selectedMode);
  text(dc,g.fontTiny,L"SELECTED",614,545,96,23,C_DIM);
  numText(dc,g.fontSmall,chosenW,705,545,70,24,C_WHITE);
  text(dc,g.fontTiny,L"x",778,547,13,20,C_DIM);
  numText(dc,g.fontSmall,chosenH,789,545,61,24,C_WHITE);
  text(dc,g.fontTiny,L"15s safety revert after applying",615,574,451,22,C_DIM);
 }
 if(g.status[0])text(dc,g.fontTiny,g.status,115,600,954,24,0xFFD5D7D9);
}
static void drawColorTab(Painter&p,HDC dc){
 text(dc,g.fontTitle,L"Visibility",115,83,660,50,C_WHITE);
 text(dc,g.fontSmall,L"Simple desktop colour tuning  /  no NVIDIA driver required",115,136,950,30,C_DIM);
 const wchar_t* names[3]={L"Original",L"Competitive",L"Night boost"};
 const wchar_t* sub[3]={L"Reset your display",L"Bright, clear colours",L"Lift darker scenes"};
 for(int i=0;i<3;i++){
  int x=115+i*323,y=189;bool sel=g.colorPreset==i;
  roundRect(p,x,y,307,68,11,sel?0xFF2D3033:(i==g.hoverTile?C_TILE_HOVER:C_TILE));
  if(sel)roundRect(p,x+17,y+17,3,34,1,0xFFF0F1F1);
  text(dc,g.fontSection,names[i],x+33,y+7,248,28,C_WHITE);
  text(dc,g.fontSmall,sub[i],x+33,y+34,250,23,C_DIM);
 }
 line(p,115,279,1068,279,0xFF323537,1);
 text(dc,g.fontTiny,L"COLOUR",121,282,245,26,C_DIM);
 slider(p,dc,L"Digital vibrance",g.saturation,0,200,130,316,405);
 slider(p,dc,L"Gamma / midtones",g.gamma,75,160,614,316,405);
 slider(p,dc,L"Shadow detail",g.shadows,0,100,130,411,405);
 slider(p,dc,L"Brightness",g.brightness,70,130,614,411,405);
 slider(p,dc,L"Contrast",g.contrast,70,130,130,506,405);
 // Desktop toggle and active Windows API capability reporting.
 text(dc,g.fontSection,L"Desktop effects",615,505,279,34,C_WHITE);
 roundRect(p,956,509,62,28,14,g.desktopColorOn?0xFF49755A:0xFF414449);
 circle(p,g.desktopColorOn?1003:971,523,10,0xFFF0F2F2);
 const wchar_t* stat=!g.desktopColorOn?L"Disabled - no colours are changed":
   !g.magSupported?L"Windows colour filter unavailable on this PC":
   !g.gammaSupported?L"Hardware gamma unavailable - midtone approximation active":
   L"Windows desktop filter ready - some full-screen games may bypass";
 text(dc,g.fontTiny,stat,614,544,432,34,C_DIM);
 if(g.status[0])text(dc,g.fontTiny,g.status,115,597,949,26,0xFFD0D3D5);
}
static void drawSettingsTab(Painter&p,HDC dc){
 text(dc,g.fontTitle,L"Settings",115,83,700,51,C_WHITE);
 text(dc,g.fontSmall,L"Every launch starts fresh. Saved presets load only when you choose.",115,134,900,30,C_DIM);
 line(p,115,177,1069,177,C_LINE,1);
 text(dc,g.fontSection,L"Your presets",115,187,295,27,C_WHITE);
 text(dc,g.fontTiny,L"Choose a slot, give it a name, then Save or Load.",480,189,585,26,C_DIM);
 for(int i=0;i<4;i++){
  int x=115+(i%2)*486,y=223+(i/2)*72;
  U32 tile=i==g.activeProfile?0xFF2A2D2F:C_TILE;
  roundRect(p,x,y,469,61,9,tile);
  if(i==g.activeProfile)strokedRoundRect(p,x+1,y+1,467,59,9,0xFF757A7F,1.0f);
  U32 dot=g.profileExists[i]?0xFF47C782:0xFF5B6065;
  circle(p,x+21,y+30,5,dot);
  const wchar_t* name=g.profileExists[i]?g.storedProfiles[i].name:defaultProfileNames[i];
  text(dc,g.fontSection,name,x+41,y+8,405,27,C_WHITE);
  text(dc,g.fontTiny,g.profileExists[i]?L"Saved profile":L"Empty slot",x+42,y+34,330,20,C_DIM);
 }
 text(dc,g.fontSmall,L"Preset name",115,379,128,27,C_DIM);
 line(p,241,409,572,409,0xFF676B70,1.0f);
 text(dc,g.fontTiny,L"Name the selected slot",606,379,396,27,C_DIM);
 button(p,dc,115,432,219,43,L"Save Preset",true);
 button(p,dc,346,432,219,43,L"Load Preset");
 button(p,dc,578,432,219,43,L"Delete");
 button(p,dc,810,432,259,43,L"Reset Session");
 line(p,115,499,1069,499,C_LINE,1);
 text(dc,g.fontSection,L"Minimize to tray",115,509,320,32,C_WHITE);
 text(dc,g.fontTiny,L"Otherwise Kova minimizes to the taskbar",115,543,525,20,C_DIM);
 roundRect(p,983,515,57,28,14,g.minimizeToTray?0xFF567C64:0xFF3C3F44);
 circle(p,g.minimizeToTray?1024:998,529,10,0xFFF2F3F3);
 text(dc,g.fontTiny,L"F1 toggles overlay  -  Profiles stay saved, settings don't auto-restore",115,572,954,22,C_DIM);
 if(g.status[0])text(dc,g.fontTiny,g.status,115,597,954,25,C_WHITE);
}
static void render(HDC hdc){
 if(!g.memoryDC||!g.memoryBitmap)return;
 textCount=0;
 void*gfx=nullptr;if(GdipCreateFromHDC(g.memoryDC,&gfx)!=0)return;
 GdipSetSmoothingMode(gfx,4);GdipSetPixelOffsetMode(gfx,4);
 Painter p={gfx};if(g.introPhase!=3)drawSplash(p,g.memoryDC);else drawFrame(p);
 // Interleave GDI+ and GDI text carefully: finish the GDI+ graphics first.
 // To enforce layering, draw all vector shapes first then all text over them using a separate gfx pass pattern.
 // Current paint helpers emit GDI text on the DIB while the GDI+ graphics is active.
 // GDI+ flushes during drawing in this implementation; the mixed operations are synchronous on the memory HDC.
 if(g.introPhase==3){
 if(g.tab==0)drawCrosshairTab(p,g.memoryDC);
 if(g.tab==1)drawResolutionTab(p,g.memoryDC);
 if(g.tab==2)drawColorTab(p,g.memoryDC);
 if(g.tab==3)drawSettingsTab(p,g.memoryDC);
 }
 GdipDeleteGraphics(gfx);
 drawQueuedText(g.memoryDC);
 BitBlt(hdc,0,0,g.w,g.h,g.memoryDC,0,0,SRCCOPY);
}
static void renewBitmap(){
 if(!g.hwnd)return;RECT r;GetClientRect(g.hwnd,&r);g.w=(int)(r.right-r.left);g.h=(int)(r.bottom-r.top);
 HDC target=GetDC(g.hwnd);if(!g.memoryDC)g.memoryDC=CreateCompatibleDC(target);
 if(g.memoryBitmap){SelectObject(g.memoryDC,g.oldBitmap);DeleteObject(g.memoryBitmap);}
 BITMAPINFO info={};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=g.w;
 info.bmiHeader.biHeight=-g.h;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;
 void* bits=nullptr;g.memoryBitmap=CreateDIBSection(target,&info,0,&bits,nullptr,0);
 g.oldBitmap=SelectObject(g.memoryDC,g.memoryBitmap);ReleaseDC(g.hwnd,target);
}
static void applyOverlay(){
 if(!g.overlay)return;
 if(g.overlayOn){ShowWindow(g.overlay,SW_SHOW);SetWindowPos(g.overlay,(HWND)-1,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);InvalidateRect(g.overlay,nullptr,0);}
 else ShowWindow(g.overlay,SW_HIDE);
 invalidate();
}
// Layered overlay draws into a premultiplied BGRA DIB then commits atomically.
// No color key approximation: alpha opacity truly works, including antialiased edges.
struct PixelCanvas{U32* pixels;int w,h;};
static void alphaPixel(PixelCanvas &b,int x,int y,float coverage,U32 col){
 if(x<0||y<0||x>=b.w||y>=b.h)return;
 coverage=fclamp(coverage,0,1);int srcA=(int)(coverage*(g.opacity*255/100)+0.5f);
 if(srcA<=0)return;
 U32 old=b.pixels[y*b.w+x];int dstA=(old>>24)&255;
 int r=(col>>16)&255,green=(col>>8)&255,blue=col&255;
 int factor=255-srcA;
 int a=clamp(srcA+dstA*factor/255,0,255);
 int pr=clamp(r*srcA/255+((old>>16)&255)*factor/255,0,255);
 int pg=clamp(green*srcA/255+((old>>8)&255)*factor/255,0,255);
 int pb=clamp(blue*srcA/255+(old&255)*factor/255,0,255);
 b.pixels[y*b.w+x]=(a<<24)|(pr<<16)|(pg<<8)|pb;
}
static void softwareLine(PixelCanvas&b,float ax,float ay,float bx,float by,float thickness,U32 color){
 float dx=bx-ax,dy=by-ay;float len2=dx*dx+dy*dy;if(len2<0.001f)len2=0.001f;
 float pad=thickness*0.5f+1.5f;
 int minx=clamp((int)((ax<bx?ax:bx)-pad),0,b.w-1);
 int maxx=clamp((int)((ax>bx?ax:bx)+pad+1),0,b.w-1);
 int miny=clamp((int)((ay<by?ay:by)-pad),0,b.h-1);
 int maxy=clamp((int)((ay>by?ay:by)+pad+1),0,b.h-1);
 for(int y=miny;y<=maxy;y++)for(int x=minx;x<=maxx;x++){
  float t=fclamp(((x-ax)*dx+(y-ay)*dy)/len2,0,1);
  float ex=x-(ax+dx*t),ey=y-(ay+dy*t);
  float distance=sqrtf(ex*ex+ey*ey);
  float c=fclamp(thickness*0.5f+0.5f-distance,0,1);
  if(c>0)alphaPixel(b,x,y,c,color);
 }
}
static void softwareCircle(PixelCanvas&b,float cx,float cy,float radius,float thickness,bool filled,U32 color){
 int extent=(int)(radius+thickness+2);
 for(int y=clamp((int)cy-extent,0,b.h-1);y<=clamp((int)cy+extent,0,b.h-1);y++){
  for(int x=clamp((int)cx-extent,0,b.w-1);x<=clamp((int)cx+extent,0,b.w-1);x++){
   float dx=x-cx,dy=y-cy;float dist=sqrtf(dx*dx+dy*dy);
   float c=filled?fclamp(radius+0.5f-dist,0,1):fclamp(thickness*0.5f+0.5f-((dist>radius)?dist-radius:radius-dist),0,1);
   if(c>0)alphaPixel(b,x,y,c,color);
  }
 }
}
static void drawOverlayInto(PixelCanvas &b){
 U32 col=g.color;int cx=b.w/2,cy=b.h/2,L=clamp(g.size*2,3,100),T=g.thickness;
 int G=clamp(g.gap+4,1,30),R=clamp((g.size+T+2)/5,1,17);
 int style=g.preset;
 if(style==12){
  if(!g.importedReady)return;
  int maxSide=imax(g.importCacheW,g.importCacheH),dstMax=clamp(g.size*3,6,144);
  int dw=imax(1,dstMax*g.importCacheW/maxSide),dh=imax(1,dstMax*g.importCacheH/maxSide);
  for(int yy=0;yy<dh;yy++)for(int xx=0;xx<dw;xx++){
   U32 src=importedPixels[(yy*g.importCacheH/dh)*g.importCacheW+(xx*g.importCacheW/dw)];
   int alpha=(src>>24)&255;
   if(alpha>0)alphaPixel(b,cx-dw/2+xx,cy-dh/2+yy,(float)alpha/255.0f,src&0xffffff);
  }
  return;
 }
 if(style==0){softwareCircle(b,cx,cy,(float)R,0,true,col);return;}
 if(style==1||style==3||style==5||style==11){
  softwareLine(b,cx-L,cy,cx-G,cy,T,col);softwareLine(b,cx+G,cy,cx+L,cy,T,col);
  softwareLine(b,cx,cy+G,cx,cy+L,T,col);
  if(style!=5)softwareLine(b,cx,cy-L,cx,cy-G,T,col);
  if(style==3||style==11)softwareCircle(b,cx,cy,clamp(T,1,6),0,true,col);
  if(style==11)softwareCircle(b,cx,cy,clamp(g.size/2,4,24),T,false,col);
  return;
 }
 if(style==2||style==4){softwareCircle(b,cx,cy,clamp(g.size,3,48),T,false,col);
  if(style==4)softwareCircle(b,cx,cy,clamp(T,1,6),0,true,col);return;}
 if(style==6){softwareLine(b,cx-L,cy+L/2,cx,cy-L/2,T,col);
  softwareLine(b,cx,cy-L/2,cx+L,cy+L/2,T,col);return;}
 if(style==7){int r=clamp(g.size+3,4,48);
  softwareLine(b,cx-r,cy-r,cx+r,cy-r,T,col);softwareLine(b,cx+r,cy-r,cx+r,cy+r,T,col);
  softwareLine(b,cx+r,cy+r,cx-r,cy+r,T,col);softwareLine(b,cx-r,cy+r,cx-r,cy-r,T,col);return;}
 if(style==8){int a=clamp(g.size/2+3,4,22),z=clamp(g.size+6,9,50);
  for(int dx=-1;dx<=1;dx+=2)for(int dy=-1;dy<=1;dy+=2){
   softwareLine(b,cx+dx*a,cy+dy*z,cx+dx*z,cy+dy*z,T,col);
   softwareLine(b,cx+dx*z,cy+dy*a,cx+dx*z,cy+dy*z,T,col);}
  return;}
 if(style==9){int r=clamp(g.size+3,5,55);
  softwareLine(b,cx,cy-r,cx+r,cy,T,col);softwareLine(b,cx+r,cy,cx,cy+r,T,col);
  softwareLine(b,cx,cy+r,cx-r,cy,T,col);softwareLine(b,cx-r,cy,cx,cy-r,T,col);return;}
 if(style==10)softwareLine(b,cx-L,cy,cx+L,cy,T,col);
}
static void overlayPaint(HWND h){
 PAINTSTRUCT ps={};BeginPaint(h,&ps);EndPaint(h,&ps);
 HDC screen=GetDC(nullptr);HDC mem=CreateCompatibleDC(screen);
 BITMAPINFO info={};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
 info.bmiHeader.biWidth=256;info.bmiHeader.biHeight=-256;
 info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
 void*raw=nullptr;HBITMAP bmp=CreateDIBSection(screen,&info,0,&raw,nullptr,0);
 if(bmp&&raw){HGDIOBJ old=SelectObject(mem,bmp);memset(raw,0,256*256*4);
  PixelCanvas p={(U32*)raw,256,256};drawOverlayInto(p);
  POINT dst={GetSystemMetrics(SM_CXSCREEN)/2-128,GetSystemMetrics(SM_CYSCREEN)/2-128};
  POINT src={0,0};SIZE dimensions={256,256};BLENDFUNCTION blend={0,0,255,1};
  UpdateLayeredWindow(h,screen,&dst,&dimensions,mem,&src,0,&blend,2);
  SelectObject(mem,old);DeleteObject(bmp);
 }
 DeleteDC(mem);ReleaseDC(nullptr,screen);
}
static void initGamma(){
 HDC dc=GetDC(nullptr);g.savedRamp=GetDeviceGammaRamp(dc,g.originalGamma)!=0;ReleaseDC(nullptr,dc);g.gammaSupported=g.savedRamp;
 g.magReady=MagInitialize()!=0;g.magSupported=false;g.magModified=false;
 if(g.magReady){g.magSupported=MagGetFullscreenColorEffect(g.originalMatrix)!=0;
  if(!g.magSupported){MagUninitialize();g.magReady=false;}}
}
static void restoreGamma(){if(g.savedRamp){HDC dc=GetDC(nullptr);SetDeviceGammaRamp(dc,g.originalGamma);ReleaseDC(nullptr,dc);}}
static void setGamma(){
 if(!g.savedRamp){g.gammaSupported=false;return;}
 if(!g.desktopColorOn||g.gamma==100){
  restoreGamma();g.gammaSupported=g.savedRamp;return;
 }
 // Only a true nonlinear gamma ramp uses the hardware API. Other settings
 // intentionally use the fullscreen matrix so drivers without a LUT still work.
 U16 ramp[3][256];float gm=(float)g.gamma/100.0f;
 for(int i=0;i<256;i++){
  float x=(float)i/255.0f;
  float y=gm>=1.0f?x+(sqrtf(x)-x)*fclamp((gm-1.0f)/0.60f,0,1):
    x+(x*x-x)*fclamp((1.0f-gm)/0.25f,0,1);
  U16 v=(U16)(fclamp(y,0,1)*65535.0f+0.5f);
  for(int c=0;c<3;c++)ramp[c][i]=v;
 }
 HDC dc=GetDC(nullptr);
 BOOL accepted=SetDeviceGammaRamp(dc,ramp);
 U16 verify[3][256]={};BOOL readable=GetDeviceGammaRamp(dc,verify);
 ReleaseDC(nullptr,dc);
 // The function may falsely report success if Windows rejects the LUT.
 bool matching=readable&&absI((int)verify[0][96]-(int)ramp[0][96])<360&&
    absI((int)verify[0][176]-(int)ramp[0][176])<360;
 g.gammaSupported=(accepted&&matching)!=0;
}
static void restoreColorMatrix(){if(g.magReady&&g.magModified){
  MagSetFullscreenColorEffect(g.originalMatrix);g.magModified=false;}}
static void setColorMatrix(){
 if(!g.magReady||!g.magSupported)return;
 if(!g.desktopColorOn||(g.saturation==100&&g.shadows==0&&g.brightness==100&&g.contrast==100&&g.gamma==100)){
  restoreColorMatrix();return;
 }
 // The fullscreen colour matrix supports real chroma mixing, brightness,
 // and linear shadow lift. It is not a non-linear gamma curve, so the gamma
 // portion is a midtone approximation when Windows rejects hardware ramps.
 float sat=fclamp((float)g.saturation/100.0f,0,2.0f);
 float ct=fclamp((float)g.contrast/100.0f,0.7f,1.3f);
 float brightness=(g.brightness-100)/220.0f;
 float lift=(float)g.shadows/430.0f;
 float midtone=(g.gamma-100)/580.0f;
 const float luma[3]={0.2126f,0.7152f,0.0722f};
 float m[25]={};
 for(int src=0;src<3;src++)for(int dst=0;dst<3;dst++){
  m[src*5+dst]=ct*((1.0f-sat)*luma[src]+(src==dst?sat:0.0f));
 }
 m[18]=1.0f;m[24]=1.0f;
 // Fullscreen color transform uses matrix row 5 for RGB translation.
 float bias=0.5f*(1-ct)+brightness+lift+midtone;
 for(int dst=0;dst<3;dst++)m[20+dst]=bias;
 if(!MagSetFullscreenColorEffect(m)){
  g.magSupported=false;setStatus(L"Windows rejected desktop colour effects. Try disabling HDR or other display filters.");return;
 }
 g.magModified=true;
}
static void applyColours(bool force=false){
 DWORD now=GetTickCount();if(!force&&now-g.lastColorApply<25)return;
 g.lastColorApply=now;
 // The matrix updates while dragging for immediate feedback.
 setColorMatrix();
 // SetDeviceGammaRamp can block for 200 ms. Commit the real ramp only
 // on slider release, preset selection, app start or a display-mode change.
 if(force){g.lastGammaApply=now;setGamma();}
}
static void chooseColorPreset(int i){
 g.colorPreset=i;g.saturation=100;g.gamma=100;g.shadows=0;
 g.brightness=100;g.contrast=100;
 g.vibrance=100;g.red=g.green=g.blue=g.temperature=100;
 g.highlights=100;g.blackLevel=0;
 if(i==1){g.saturation=145;g.gamma=112;g.shadows=23;g.brightness=103;g.contrast=107;}
 if(i==2){g.saturation=123;g.gamma=128;g.shadows=65;g.brightness=110;g.contrast=102;}
 applyColours(true);invalidate();
}
static bool findNativeAndCurrent(){
 DEVMODEW d={};d.dmSize=sizeof(d);
 if(!EnumDisplaySettingsW(nullptr,ENUM_CURRENT_SETTINGS,&d))return false;
 g.startMode=d;g.desktopW=d.dmPelsWidth;g.desktopH=d.dmPelsHeight;g.nativeMode=d;
 U64 best=(U64)d.dmPelsWidth*d.dmPelsHeight;
 for(int i=0;i<512;i++){
   DEVMODEW trial={};trial.dmSize=sizeof(trial);
   if(!EnumDisplaySettingsW(nullptr,i,&trial))break;
   if(trial.dmPelsWidth<640||trial.dmPelsHeight<480)continue;
   U64 pixels=(U64)trial.dmPelsWidth*trial.dmPelsHeight;
   if(pixels>best||(pixels==best&&trial.dmDisplayFrequency>g.nativeMode.dmDisplayFrequency)){
      best=pixels;g.nativeMode=trial;
   }
 }
 return true;
}
static void refreshMode(){DEVMODEW d={};d.dmSize=sizeof(d);if(EnumDisplaySettingsW(nullptr,ENUM_CURRENT_SETTINGS,&d)){
 g.desktopW=d.dmPelsWidth;g.desktopH=d.dmPelsHeight;
 }}
static int parsePositive(const wchar_t*s){int n=0;if(!s||!*s)return 0;for(int i=0;s[i];i++){
 if(s[i]<L'0'||s[i]>L'9')return 0;n=n*10+(int)(s[i]-L'0');if(n>100000)return 0;}return n;}
static void readCustomResolution(){
 wchar_t b[24]={};if(g.editWidth){GetWindowTextW(g.editWidth,b,24);g.customW=parsePositive(b);}
 if(g.editHeight){GetWindowTextW(g.editHeight,b,24);g.customH=parsePositive(b);}
}
static void syncEditors(){
 if(g.editWidth)ShowWindow(g.editWidth,g.tab==1&&g.introPhase==3?SW_SHOW:SW_HIDE);
 if(g.editHeight)ShowWindow(g.editHeight,g.tab==1&&g.introPhase==3?SW_SHOW:SW_HIDE);
 if(g.editProfileName)ShowWindow(g.editProfileName,g.tab==3&&g.introPhase==3?SW_SHOW:SW_HIDE);
}
static void revertDisplay(){
 if(!g.keepPending)return;
 ChangeDisplaySettingsExW(nullptr,&g.priorMode,nullptr,0,nullptr);
 g.keepPending=false;KillTimer(g.hwnd,2);g.remaining=0;refreshMode();
 setStatus(L"Display mode reverted to the previous resolution.");
}
static void startDisplayMode(int selected){
 if(g.keepPending)revertDisplay();
 if(selected==15){readCustomResolution();
  if(g.customW<640||g.customW>7680||g.customH<480||g.customH>4320){
   setStatus(L"Enter width 640-7680 and height 480-4320.");return;}
 }
 DEVMODEW cur={};cur.dmSize=sizeof(cur);if(!EnumDisplaySettingsW(nullptr,ENUM_CURRENT_SETTINGS,&cur)){
  setStatus(L"Could not query the current Windows display mode.");return;
 }
 g.priorMode=cur;
 DEVMODEW target=selected==0?g.nativeMode:cur;
 target.dmPelsWidth=modeW(selected);target.dmPelsHeight=modeH(selected);
 target.dmFields=DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL|DM_DISPLAYFREQUENCY;
 // Choose a supported refresh-rate for a custom-sized display mode, if available.
 if(selected!=0){
  bool found=false;int bestScore=-2147483647;
  for(int i=0;i<512;i++){
   DEVMODEW t={};t.dmSize=sizeof(t);if(!EnumDisplaySettingsW(nullptr,i,&t))break;
   if((int)t.dmPelsWidth==modeW(selected)&&(int)t.dmPelsHeight==modeH(selected)){
    int hz=(int)t.dmDisplayFrequency;
    int score=hz<=(int)cur.dmDisplayFrequency?hz*10:100-(hz-(int)cur.dmDisplayFrequency);
    if(score>bestScore){target=t;bestScore=score;found=true;}
   }
  }
  if(!found){target.dmDisplayFrequency=0;target.dmFields=DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL;}
 }
 if((int)cur.dmPelsWidth==(int)target.dmPelsWidth&&(int)cur.dmPelsHeight==(int)target.dmPelsHeight){
  setStatus(L"This display mode is already active.");return;
 }
 // A typed custom size can only be activated when Windows / the driver accepts it.
 LONG test=ChangeDisplaySettingsExW(nullptr,&target,nullptr,CDS_TEST,nullptr);
 if(test!=0){setStatus(L"Display driver does not support that mode. Create it in GPU settings first.");return;}
 LONG result=ChangeDisplaySettingsExW(nullptr,&target,nullptr,0,nullptr);
 if(result!=0){setStatus(L"Windows rejected this display mode. Add it in your GPU driver if unsupported.");return;}
 g.pendingMode=target;g.keepPending=true;g.remaining=15;g.modeTimestamp=GetTickCount();
 SetTimer(g.hwnd,2,1000,nullptr);refreshMode();setStatus(L"");
}
static void keepDisplay(){
 if(!g.keepPending)return;
 DEVMODEW current={};current.dmSize=sizeof(current);
 if(!EnumDisplaySettingsW(nullptr,ENUM_CURRENT_SETTINGS,&current)){revertDisplay();return;}
 // Do not persist monitor settings to the Windows registry. User wants every
 // Kova launch to start with defaults; display changes are session-only.
 setStatus(L"Resolution kept for this Windows session. No automatic profile save.");
 g.keepPending=false;KillTimer(g.hwnd,2);refreshMode();
}
static void showTray(bool minimize){
 if(minimize&&!g.minimizeToTray){ShowWindow(g.hwnd,SW_MINIMIZE);return;}
 if(minimize){
  if(!g.trayShown){NOTIFYICONDATAW n={};n.cbSize=sizeof(n);n.hWnd=g.hwnd;n.uID=7;n.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
   n.uCallbackMessage=WM_TRAY;n.hIcon=g.icon;wcopy(n.szTip,L"Kova  |  Click to open",128);
   g.trayShown=Shell_NotifyIconW(NIM_ADD,&n)!=0;}
  if(g.trayShown)ShowWindow(g.hwnd,SW_HIDE);
  else {ShowWindow(g.hwnd,SW_MINIMIZE);setStatus(L"Tray unavailable; minimized to the Windows taskbar instead.");}
 }else{
  if(g.trayShown){NOTIFYICONDATAW n={};n.cbSize=sizeof(n);n.hWnd=g.hwnd;n.uID=7;Shell_NotifyIconW(NIM_DELETE,&n);g.trayShown=false;}
  ShowWindow(g.hwnd,SW_SHOW);ShowWindow(g.hwnd,SW_RESTORE);SetWindowPos(g.hwnd,(HWND)0,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
 }
}
static void onSlide(int x,int control){
 if(g.tab==0){
  int lo=1,hi=48;if(control==1){lo=1;hi=12;}if(control==2){lo=0;hi=24;}if(control==3){lo=10;hi=100;}
  int value=clamp(lo+(x-839)*(hi-lo)/230,lo,hi);
  if(control==0)g.size=value;else if(control==1)g.thickness=value;
  else if(control==2)g.gap=value;else g.opacity=value;
  if(g.overlayOn&&g.overlay)InvalidateRect(g.overlay,nullptr,0);
 }else if(g.tab==2){
  const int lo[5]={0,75,0,70,70};
  const int hi[5]={200,160,100,130,130};
  int sx=control%2==0?130:614;
  int value=clamp(lo[control]+(x-sx)*(hi[control]-lo[control])/405,lo[control],hi[control]);
  if(control==0)g.saturation=value;
  else if(control==1)g.gamma=value;
  else if(control==2)g.shadows=value;
  else if(control==3)g.brightness=value;
  else g.contrast=value;
  g.colorPreset=-1;applyColours();
 }
 invalidate();
}
static void pickerMove(int x,int y){
 if(g.pickerDrag==1){g.pickerSat=clamp((x-566)*100/199,0,100);g.pickerValue=clamp((445-y)*100/123,0,100);}
 else if(g.pickerDrag==2){g.pickerHue=clamp((x-566)*359/199,0,359);}
 updatePickerColor();
}
static void clickWindow(int x,int y){
 if(g.introPhase!=3){
  if(y>=7&&y<=58){if(x>=g.w-61){DestroyWindow(g.hwnd);return;}if(x>=g.w-120){showTray(true);return;}}
  if(g.introPhase==1&&hit(x,y,440,395,240,49)){g.introPhase=2;g.introFade=0;SetTimer(g.hwnd,5,16,nullptr);invalidate();}
  return;
 }
 if(y>=7&&y<=50){
  if(x>=g.w-61){DestroyWindow(g.hwnd);return;}
  if(x>=g.w-120){showTray(true);return;}
 }
 for(int i=0;i<4;i++)if(hit(x,y,8,84+i*70,53,46)){
  g.tab=i;g.customPicker=false;g.hoverTile=-1;g.status[0]=0;
  syncEditors();invalidate();return;}
 if(g.tab==0){
  if(g.customPicker){
   if(absI(x-1055)<=14&&absI(y-504)<=15){g.customPicker=false;invalidate();return;}
   if(hit(x,y,566,321,200,124)){g.pickerDrag=1;SetCapture(g.hwnd);pickerMove(x,y);return;}
   if(hit(x,y,566,455,200,14)){g.pickerDrag=2;SetCapture(g.hwnd);pickerMove(x,y);return;}
   if(hit(x,y,551,274,243,248))return;
   g.customPicker=false;
  }
  // Import stores the PNG, but does not auto-load it on the next launch.
  if(hit(x,y,115,544,178,35)){importCrosshair();invalidate();return;}
  if(hit(x,y,304,544,91,35)){g.preset=1;if(g.overlay)InvalidateRect(g.overlay,nullptr,0);invalidate();return;}
  // Overlay is accessible from the crosshair page without switching tabs.
  if(hit(x,y,996,540,73,47)){g.overlayOn=!g.overlayOn;applyOverlay();return;}
  for(int i=0;i<12;i++)if(hit(x,y,115+(i%3)*97,175+(i/3)*89,88,80)){
    g.preset=i;if(g.overlay)InvalidateRect(g.overlay,nullptr,0);invalidate();return;
  }
  for(int i=0;i<4;i++)if(hit(x,y,833,203+i*76,245,30)){
    g.downSlider=i;g.mouseDown=true;SetCapture(g.hwnd);onSlide(x,i);return;
  }
  for(int i=0;i<7;i++)if(absI(x-(851+i*34))<=13&&absI(y-504)<=14){
    if(i==6){g.customPicker=!g.customPicker;if(g.customPicker)rgbToHSV();}
    else{g.swatch=i;g.color=swatches[i]&0xFFFFFF;g.customPicker=false;}
    if(g.overlay)InvalidateRect(g.overlay,nullptr,0);invalidate();return;
  }
  g.customPicker=false;
 }
 if(g.tab==1){
  for(int i=0;i<8;i++)if(hit(x,y,115+i*120,182,111,37)){
   g.gameFilter=i;g.hoverTile=-1;g.status[0]=0;invalidate();return;
  }
  for(int slot=0;slot<8;slot++)if(hit(x,y,115+(slot%4)*241,285+(slot/4)*78,229,69)){
   g.selectedMode=gameChoices[g.gameFilter][slot];g.status[0]=0;invalidate();return;
  }
  if(hit(x,y,115,458,954,72)){g.selectedMode=15;g.status[0]=0;invalidate();return;}
  if(hit(x,y,115,540,226,46)){startDisplayMode(g.selectedMode);invalidate();return;}
  if(hit(x,y,355,540,226,46)){g.selectedMode=0;startDisplayMode(0);invalidate();return;}
  if(g.keepPending&&hit(x,y,602,568,213,28)){keepDisplay();return;}
  if(g.keepPending&&hit(x,y,827,568,226,28)){revertDisplay();return;}
 }
 if(g.tab==2){
  for(int i=0;i<3;i++)if(hit(x,y,115+i*323,189,307,68)){chooseColorPreset(i);return;}
  if(hit(x,y,946,497,85,52)){g.desktopColorOn=!g.desktopColorOn;applyColours(true);invalidate();return;}
  const int xs[5]={130,614,130,614,130};
  const int ys[5]={316,316,411,411,506};
  for(int i=0;i<5;i++)if(hit(x,y,xs[i]-7,ys[i]+31,421,28)){
   g.downSlider=i;g.mouseDown=true;SetCapture(g.hwnd);onSlide(x,i);return;
  }
 }
 if(g.tab==3){
  for(int i=0;i<4;i++)if(hit(x,y,115+(i%2)*486,223+(i/2)*72,469,61)){
   g.activeProfile=i;syncProfileEditor();g.status[0]=0;invalidate();return;
  }
  if(hit(x,y,115,432,219,43)){saveSelectedProfile();invalidate();return;}
  if(hit(x,y,346,432,219,43)){loadSelectedProfile();invalidate();return;}
  if(hit(x,y,578,432,219,43)){deleteSelectedProfile();invalidate();return;}
  if(hit(x,y,810,432,259,43)){resetSession();invalidate();return;}
  if(hit(x,y,973,506,87,48)){g.minimizeToTray=!g.minimizeToTray;invalidate();return;}
 }
}
static void moveMouse(int x,int y){
 int nav=-1,tile=-1,control=-1;
 for(int i=0;i<4;i++)if(hit(x,y,8,84+i*70,53,46))nav=i;
 if(y<50){if(x>=g.w-61)control=202;else if(x>=g.w-120)control=200;}
 if(g.tab==0){for(int i=0;i<12;i++)if(hit(x,y,115+(i%3)*97,175+(i/3)*89,88,80))tile=i;}
 if(g.tab==1){for(int slot=0;slot<8;slot++)if(hit(x,y,115+(slot%4)*241,285+(slot/4)*78,229,69))tile=slot;}
 if(g.tab==2){for(int i=0;i<3;i++)if(hit(x,y,115+i*323,189,307,68))tile=i;}
 if(nav!=g.hoverNav||tile!=g.hoverTile||control!=g.hoverControl){
  g.hoverNav=nav;g.hoverTile=tile;g.hoverControl=control;invalidate();}
 if(g.pickerDrag)pickerMove(x,y);
 if(g.mouseDown&&g.downSlider>=0)onSlide(x,g.downSlider);
}
static LRESULT API overlayProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
 if(msg==WM_PAINT){overlayPaint(hwnd);return 0;}
 if(msg==WM_NCHITTEST)return -1; // HTTRANSPARENT, never consume game inputs.
 if(msg==WM_ERASEBKGND)return 1;
 return DefWindowProcW(hwnd,msg,wp,lp);
}
static LRESULT API mainProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
 if(msg==WM_NCHITTEST){
  // WM_NCHITTEST uses screen coordinates, not client coordinates.
  RECT r={};GetWindowRect(hwnd,&r);int x=(int)(short)(lp&0xffff)-(int)r.left,y=(int)(short)((lp>>16)&0xffff)-(int)r.top;
  if(y>=0&&y<56&&x>(g.introPhase==3?70:21)&&x<g.w-122)return HTCAPTION;
  return HTCLIENT;
 }
 if(msg==WM_SIZE){if(wp!=1&&g.memoryDC)renewBitmap();invalidate();return 0;}
 if(msg==WM_CTLCOLOREDIT){
  HDC dc=(HDC)wp;SetTextColor(dc,0x00EEEAE6);SetBkColor(dc,0x0012100F);
  return (LRESULT)g.editBrush;
 }
 if(msg==WM_COMMAND){
  int id=(int)(wp&0xFFFF),notification=(int)((wp>>16)&0xFFFF);
  if(!g.initializingEditors&&(id==101||id==102)&&notification==0x300){
   g.selectedMode=15;readCustomResolution();invalidate();
  }
  return 0;
 }
 if(msg==WM_ERASEBKGND)return 1;
 if(msg==WM_PAINT){PAINTSTRUCT ps={};HDC dc=BeginPaint(hwnd,&ps);render(dc);EndPaint(hwnd,&ps);return 0;}
 if(msg==WM_MOUSEMOVE){if(g.introPhase!=3)return 0;if(!g.trackingMouse){TRACKMOUSEEVENT t={sizeof(t),2,hwnd,0};g.trackingMouse=TrackMouseEvent(&t)!=0;}int x=(short)(lp&0xFFFF),y=(short)((lp>>16)&0xFFFF);moveMouse(x,y);return 0;}
 if(msg==WM_MOUSELEAVE){g.trackingMouse=false;g.hoverNav=-1;g.hoverTile=-1;g.hoverControl=-1;invalidate();return 0;}
 if(msg==WM_LBUTTONDOWN){int x=(short)(lp&0xFFFF),y=(short)((lp>>16)&0xFFFF);clickWindow(x,y);return 0;}
 if(msg==WM_LBUTTONUP){if(g.pickerDrag){g.pickerDrag=0;ReleaseCapture();}
  if(g.mouseDown){g.mouseDown=false;g.downSlider=-1;ReleaseCapture();if(g.tab==2)applyColours(true);}return 0;}
 if(msg==WM_KEYDOWN&&(wp==0x0D||wp==0x20)&&g.introPhase==1){g.introPhase=2;g.introFade=0;SetTimer(hwnd,5,16,nullptr);invalidate();return 0;}
 if(msg==WM_KEYDOWN&&wp==0x1B){if(g.customPicker){g.customPicker=false;invalidate();}return 0;}
 if(msg==WM_HOTKEY&&wp==1){if(g.introPhase!=3)return 0;g.overlayOn=!g.overlayOn;applyOverlay();return 0;}
 if(msg==WM_TRAY){if(lp==WM_LBUTTONDBLCLK_TRAY||lp==WM_LBUTTONUP_TRAY){showTray(false);}return 0;}
 if(msg==WM_DISPLAYCHANGE){refreshMode();if(g.introPhase==3&&g.desktopColorOn)SetTimer(hwnd,4,750,nullptr);if(g.overlay){int sx=GetSystemMetrics(SM_CXSCREEN),sy=GetSystemMetrics(SM_CYSCREEN);SetWindowPos(g.overlay,(HWND)-1,sx/2-128,sy/2-128,256,256,SWP_NOACTIVATE);InvalidateRect(g.overlay,nullptr,0);}invalidate();return 0;}
 if(msg==WM_TIMER&&wp==5){
  if(g.introPhase==0){g.introProgress=imin(100,g.introProgress+3);if(g.introProgress>=100){g.introPhase=1;KillTimer(hwnd,5);}invalidate();}
  else if(g.introPhase==2){g.introFade++;if(g.introFade>=12){g.introPhase=3;KillTimer(hwnd,5);syncEditors();
    if(g.desktopColorOn)applyColours(true);applyOverlay();}invalidate();}
  return 0;
 }
 if(msg==WM_TIMER&&wp==4){KillTimer(hwnd,4);if(g.introPhase==3&&g.desktopColorOn)applyColours(true);return 0;}
 if(msg==WM_TIMER&&wp==2&&g.keepPending){
  // Use elapsed monotonic clock, not timer tick count, to enforce actual 15 s.
  int elapsed=(int)((GetTickCount()-g.modeTimestamp)/1000);
  g.remaining=clamp(15-elapsed,0,15);
  if(g.remaining==0)revertDisplay();invalidate();return 0;
 }
 if(msg==WM_CLOSE){DestroyWindow(hwnd);return 0;}
 if(msg==WM_DESTROY){
  KillTimer(hwnd,5);if(g.keepPending)revertDisplay();KillTimer(hwnd,4);restoreGamma();restoreColorMatrix();
  if(g.magReady)MagUninitialize();DeleteFileW(g.customPath);
  UnregisterHotKey(hwnd,1);
  if(g.trayShown){NOTIFYICONDATAW n={};n.cbSize=sizeof(n);n.hWnd=hwnd;n.uID=7;Shell_NotifyIconW(NIM_DELETE,&n);}
  if(g.overlay)DestroyWindow(g.overlay);
  if(g.previewImage)GdipDisposeImage(g.previewImage);
  if(g.importedImage)GdipDisposeImage(g.importedImage);
  if(g.previewStream){void*** vp=(void***)g.previewStream;((U32(API*)(void*))((*vp)[2]))(g.previewStream);}
  if(g.editBrush)DeleteObject(g.editBrush);
  if(g.memoryDC){if(g.memoryBitmap){SelectObject(g.memoryDC,g.oldBitmap);DeleteObject(g.memoryBitmap);}DeleteDC(g.memoryDC);}
  DeleteObject(g.fontTitle);DeleteObject(g.fontBody);DeleteObject(g.fontSmall);DeleteObject(g.fontSection);DeleteObject(g.fontTiny);DeleteObject(g.fontSplash);
  PostQuitMessage(0);return 0;
 }
 return DefWindowProcW(hwnd,msg,wp,lp);
}
static int launch(){
 SetProcessDPIAware();g.w=BASE_W;g.h=BASE_H;g.hoverNav=g.hoverTile=g.hoverControl=g.downSlider=-1;
 g.preset=1;g.size=19;g.thickness=5;g.gap=1;g.opacity=100;g.swatch=-1;g.color=0x59FF69;
 g.brightness=100;g.contrast=100;g.gamma=100;g.selectedMode=0;
 g.customW=1440;g.customH=1080;g.red=g.green=g.blue=g.temperature=100;
 g.shadows=0;g.desktopColorOn=false;g.minimizeToTray=false;
 g.saturation=100;g.vibrance=100;g.highlights=100;g.blackLevel=0;
 GdiplusStartupInput gd={1,nullptr,0,0};if(GdiplusStartup(&g.gdipToken,&gd,nullptr)!=0)return 1;
 findNativeAndCurrent();initGamma();makePath();
 prepareProfileStore();discoverProfiles();g.activeProfile=0;
 wcopy(g.customPath,g.profileDirectory,512);wappend(g.customPath,512,L"\\CurrentCrosshair.png");
 DeleteFileW(g.customPath); // Discard uncommitted imported PNG from a prior run.
 g.vibrance=100;g.red=g.green=g.blue=g.temperature=100;g.highlights=100;g.blackLevel=0;
 g.saturation=clamp(g.saturation,0,200);g.gamma=clamp(g.gamma,75,160);g.brightness=clamp(g.brightness,70,130);g.contrast=clamp(g.contrast,70,130);
 if(g.colorPreset>2)g.colorPreset=-1;rgbToHSV();
 // Decode bundled Dust-style preview in memory. No external files or resource packs.
 HGLOBAL pngMem=GlobalAlloc(2,kDustPreviewSize);
 if(pngMem){void* rawPNG=GlobalLock(pngMem);
  if(rawPNG){memcpy(rawPNG,kDustPreviewPNG,kDustPreviewSize);GlobalUnlock(pngMem);
   if(CreateStreamOnHGlobal(pngMem,1,&g.previewStream)==0){
    GdipCreateBitmapFromStream(g.previewStream,&g.previewImage);
   }else GlobalFree(pngMem);
  }else GlobalFree(pngMem);
 }
 g.fontTitle=CreateFontW(-29,0,0,0,600,0,0,0,1,0,0,5,0,L"Segoe UI");
 g.fontSplash=CreateFontW(-51,0,0,0,700,0,0,0,1,0,0,5,0,L"Segoe UI");
 g.fontBody=CreateFontW(-17,0,0,0,500,0,0,0,1,0,0,5,0,L"Segoe UI");
 g.fontSmall=CreateFontW(-14,0,0,0,400,0,0,0,1,0,0,5,0,L"Segoe UI");
 g.fontTiny=CreateFontW(-13,0,0,0,400,0,0,0,1,0,0,5,0,L"Segoe UI");
 g.fontSection=CreateFontW(-18,0,0,0,600,0,0,0,1,0,0,5,0,L"Segoe UI");
 HINSTANCE inst=GetModuleHandleW(nullptr);
 g.icon=LoadIconW(inst,MAKERESOURCE(1));if(!g.icon)g.icon=LoadIconW(nullptr,MAKERESOURCE(32512));
 WNDCLASSEXW klass={};klass.cbSize=sizeof(klass);klass.style=3;klass.lpfnWndProc=mainProc;klass.hInstance=inst;klass.hIcon=g.icon;klass.hIconSm=g.icon;klass.hCursor=LoadCursorW(nullptr,MAKERESOURCE(32512));klass.lpszClassName=L"KovaNativeWindow";
 if(!RegisterClassExW(&klass))return 2;
 klass.lpfnWndProc=overlayProc;klass.lpszClassName=L"KovaClickThroughOverlay";klass.style=0;klass.hIcon=nullptr;
 if(!RegisterClassExW(&klass))return 3;
 int sx=GetSystemMetrics(SM_CXSCREEN),sy=GetSystemMetrics(SM_CYSCREEN);
 g.hwnd=CreateWindowExW(WS_EX_APPWINDOW,L"KovaNativeWindow",L"Kova",WS_POPUP|WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
  (sx-BASE_W)/2,(sy-BASE_H)/2,BASE_W,BASE_H,nullptr,nullptr,inst,nullptr);
 if(!g.hwnd)return 4;
 // Match 20px-radius source reference window on Windows 10 and Windows 11.
 HRGN rgn=CreateRoundRectRgn(0,0,BASE_W+1,BASE_H+1,25,25);SetWindowRgn(g.hwnd,rgn,1);
 renewBitmap();
 g.editBrush=CreateSolidBrush(0x0012100F);
 g.editWidth=CreateWindowExW(0,L"EDIT",L"",WS_CHILD|ES_NUMBER|ES_AUTOHSCROLL|WS_TABSTOP,
  592,488,111,28,g.hwnd,(void*)101,inst,nullptr);
 g.editHeight=CreateWindowExW(0,L"EDIT",L"",WS_CHILD|ES_NUMBER|ES_AUTOHSCROLL|WS_TABSTOP,
  748,488,111,28,g.hwnd,(void*)102,inst,nullptr);
 g.editProfileName=CreateWindowExW(0,L"EDIT",L"",WS_CHILD|ES_AUTOHSCROLL|WS_TABSTOP,
  249,378,316,29,g.hwnd,(void*)103,inst,nullptr);
 SendMessageW(g.editProfileName,WM_SETFONT,(WPARAM)g.fontSection,1);
 syncProfileEditor();
 SendMessageW(g.editWidth,WM_SETFONT,(WPARAM)g.fontSection,1);
 SendMessageW(g.editHeight,WM_SETFONT,(WPARAM)g.fontSection,1);
 g.initializingEditors=true;
 wchar_t numbers[32];int vals[2]={g.customW,g.customH};HWND editors[2]={g.editWidth,g.editHeight};
 for(int j=0;j<2;j++){int n=vals[j],i=0;wchar_t reverse[24];
  do{reverse[i++]=L'0'+n%10;n/=10;}while(n&&i<23);
  for(int k=0;k<i;k++)numbers[k]=reverse[i-k-1];numbers[i]=0;
  SetWindowTextW(editors[j],numbers);
 }
 g.initializingEditors=false;
 syncEditors();ShowWindow(g.editWidth,SW_HIDE);ShowWindow(g.editHeight,SW_HIDE);
 g.overlay=CreateWindowExW(WS_EX_TOPMOST|WS_EX_TRANSPARENT|WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,
  L"KovaClickThroughOverlay",L"",WS_POPUP,sx/2-128,sy/2-128,256,256,nullptr,nullptr,inst,nullptr);
 // UpdateLayeredWindow owns the per-pixel alpha; do not call SetLayeredWindowAttributes here.
 g.hotkeyOK=RegisterHotKey(g.hwnd,1,0,VK_F1)!=0;
 if(!g.hotkeyOK)setStatus(L"F1 hotkey is already in use by another application.");
 // Imported PNG is intentionally NOT loaded at startup. Manual import or profile Load only.
 g.introPhase=0;g.introProgress=0;g.introFade=0;SetTimer(g.hwnd,5,16,nullptr);
 ShowWindow(g.overlay,SW_HIDE);ShowWindow(g.hwnd,SW_SHOW);UpdateWindow(g.hwnd);
 MSG msg={};while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}
 GdiplusShutdown(g.gdipToken);return 0;
}
extern "C" void WinMainCRTStartup(){ExitProcess((UINT)launch());}
