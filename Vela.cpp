// Vela -- freestanding native Windows x64 C++ application.
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
DLL BOOL API QueryFullProcessImageNameW(HANDLE,DWORD,LPWSTR,DWORD*);
DLL HWND API GetForegroundWindow();
DLL DWORD API GetWindowThreadProcessId(HWND,DWORD*);
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
DLL HWND API SetFocus(HWND);
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
DLL BOOL API SystemParametersInfoW(UINT,UINT,void*,UINT);
DLL BOOL API IsIconic(HWND);
DLL int API GdipDrawArcI(void*,void*,int,int,int,int,float,float);
DLL int API GdipCreateImageAttributes(void**);
DLL int API GdipDisposeImageAttributes(void*);
DLL int API GdipSetImageAttributesColorMatrix(void*,int,BOOL,const float*,const float*,int);
DLL int API GdipDrawImageRectRectI(void*,void*,int,int,int,int,int,int,int,int,int,void*,void*,void*);
DLL BOOL API SetProcessDPIAware();
DLL void* API GetProcAddress(HINSTANCE,const char*);
DLL int API GdipTranslateWorldTransform(void*,float,float,int);
DLL int API GdipScaleWorldTransform(void*,float,float,int);
static int uiDpi=96;
static int uiPixels(int dip){return (dip*uiDpi+48)/96;}
static int logical(int px){return (px*96+uiDpi/2)/uiDpi;}
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
// Fast native crossfade, confined to main content (no moving nav controls).
struct BLEND_CONST{BYTE operation,flags,alpha,format;};
DLL BOOL API AlphaBlend(HDC,int,int,int,int,HDC,int,int,int,int,BLEND_CONST);
DLL HFONT API CreateFontW(int,int,int,int,int,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPCWSTR);
DLL int API GetTextFaceW(HDC,int,LPWSTR);
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
// Scoped graphics transformations: rotate hovered glyphs without moving hit areas.
DLL int API GdipSaveGraphics(void*,UINT*);
DLL int API GdipRestoreGraphics(void*,UINT);
DLL int API GdipTranslateWorldTransform(void*,float,float,int);
DLL int API GdipRotateWorldTransform(void*,float,int);
DLL int API GdipSetSmoothingMode(void*,int);
DLL int API GdipSetPixelOffsetMode(void*,int);
DLL int API GdipGraphicsClear(void*,DWORD);
DLL int API GdipCreateSolidFill(DWORD,void**);
DLL int API GdipDeleteBrush(void*);
DLL int API GdipCreatePen1(DWORD,float,int,void**);
DLL int API GdipDeletePen(void*);
DLL int API GdipFillRectangleI(void*,void*,int,int,int,int);
DLL int API GdipDrawLineI(void*,void*,int,int,int,int);
DLL int API GdipDrawLine(void*,void*,float,float,float,float);
DLL int API GdipDrawBezier(void*,void*,float,float,float,float,float,float,float,float);
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
constexpr DWORD LWA_COLORKEY=1,LWA_ALPHA=2,BI_RGB=0,SRCCOPY=0x00CC0020;
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
struct Accent {const wchar_t* name;U32 rgb;};
static const Accent accents[]={{L"Ice",0x32B8FF},{L"Mint",0x52D9AF},{L"Violet",0xB090FF},{L"Rose",0xFF80B5},{L"Amber",0xFFC167}};
constexpr int accentCount=sizeof(accents)/sizeof(accents[0]);
static int accentIndex=0;
static U32 tint(int intensity=255,int alpha=255){
 U32 rgb=accents[accentIndex].rgb;
 return ARGB((U8)alpha,(U8)(((rgb>>16)&255)*intensity/255),(U8)(((rgb>>8)&255)*intensity/255),(U8)((rgb&255)*intensity/255));
}
static U32 RGBX(U8 r,U8 g,U8 b){return ARGB(255,r,g,b);}
static int absI(int n){return n<0?-n:n;}
static bool hit(int x,int y,int a,int b,int w,int h){return x>=a&&x<a+w&&y>=b&&y<b+h;}
static int strlenA(const char*s){int i=0;while(s[i]&&i<32767)i++;return i;}
static bool strEq(const char*a,const char*b){for(int i=0;;i++){if(a[i]!=b[i])return false;if(!a[i])return true;}}
static void wcopy(wchar_t* dst,const wchar_t*src,int n){int i=0;for(;i<n-1&&src[i];i++)dst[i]=src[i];dst[i]=0;}
static int wlen(const wchar_t*s){int i=0;while(s[i]&&i<500)i++;return i;}
static bool wappend(wchar_t*dst,int cap,const wchar_t*tail){int a=wlen(dst),b=wlen(tail);if(a+b>=cap)return false;for(int j=0;j<=b;j++)dst[a+j]=tail[j];return true;}
#include "VelaAutoUpdate.h"

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
static constexpr U32 PROFILE_MAGIC=0x41564F4B; // VELA
static U32 profileChecksum(const ProfileData& d){
 const U8*bytes=(const U8*)&d;U32 hash=2166136261u;
 for(U32 i=0;i<(U32)sizeof(ProfileData)-4;i++)hash=(hash^bytes[i])*16777619u;
 return hash;
}
static const wchar_t* defaultProfileNames[4]={L"Preset 1",L"Preset 2",L"Preset 3",L"Preset 4"};
// Keeps the existing .vela preset binary layout backward-compatible.
// This optional sidecar is written only on explicit Save Preset.
struct SavedGameBinding{U32 magic,version;I32 enabled;wchar_t executable[128];U32 checksum;};
static U32 bindingHash(const SavedGameBinding& d){
 const U8* ptr=(const U8*)&d;U32 hash=2166136261u;
 for(U32 i=0;i<sizeof(d)-4;i++)hash=(hash^ptr[i])*16777619u;
 return hash;
}


struct State{
 HWND hwnd,overlay,editWidth,editHeight,editProfileName; HBRUSH editBrush,numberBrush; HICON icon;
 wchar_t profileDirectory[512],profileFiles[4][512],profilePNGs[4][512],profileGames[4][512];
 ProfileData storedProfiles[4];bool profileExists[4];int activeProfile;
 void* previewImage; void* previewStream; void* brandImage;void* brandStream; HDC memoryDC; HBITMAP memoryBitmap; HGDIOBJ oldBitmap;
 HFONT fontTitle,fontBody,fontSmall,fontTiny,fontSection,fontSplash;
 ULONG_PTR gdipToken; int w,h, tab, hoverNav,hoverTile,hoverControl,downSlider,dragY,winX,winY;
 int preset,crosshairPage,size,thickness,gap,opacity,swatch,color;
 // Native hover-icon animations: rotating target, blinking eye, scanning
 // display and rotating settings gear. No hover backgrounds or moving tiles.
 int navHoverFade[4];float navPhase[4];U32 navCycleStart[4],navLastTick;
 int pickerHue,pickerSat,pickerValue,pickerDrag;
 int saturation,vibrance,highlights,blackLevel;
 bool magReady,magSupported,magModified;
 float originalMatrix[25];
 DWORD lastColorApply,lastGammaApply; int introPhase,introProgress,introFade,introHoverTarget,introHover;
 U32 introStageTick, mainFadeTick; int mainFadeAlpha;
 int customW,customH,red,green,blue,temperature,shadows;
 bool desktopColorOn,minimizeToTray,initializingEditors;
 int brightness,contrast,gamma,colorPreset;
 bool overlayOn,trayShown,mouseDown,customPicker, gammaSupported,hotkeyOK,keepPending,modeChanged,isExpanded,trackingMouse;
 int selectedMode,remaining,resW,resH,desktopW,desktopH,desktopHz;
 U32 modeTimestamp; DEVMODEW startMode,nativeMode,priorMode,pendingMode;
 U16 originalGamma[3][256]; bool savedRamp;
 wchar_t modulePath[512],customPath[512]; wchar_t status[220],updateStatus[160];
 int gameFilter;
 U32 introStartTick; int logoHover,logoHoverTarget;
 // Transition buffer: previous page is blended over the incoming page for 230ms.

 // No hooking/injection: monitor the foreground executable and conditionally
 // apply the existing Windows desktop colour matrix/gamma effects.
 bool gameFocusOnly,gameFocusActive,gameBindPending;
 wchar_t gameProcess[128];U32 gameBindAt;

 void* importedImage; int importedW,importedH,importCacheW,importCacheH;bool importedReady;
} g={};

// Imported images are user-owned library assets. The active selection remains
// session-only: reopening Vela always starts from the default built-in preset.
constexpr int VELA_MAX_IMPORTS=24;
struct VelaLibraryPNG {wchar_t path[512];void* image;UINT width,height;bool ready;};
static VelaLibraryPNG libraryPNGs[VELA_MAX_IMPORTS]={};
static int libraryCount=0;
static wchar_t libraryFolder[512]={};

// Art direction: measured against the supplied 1120x634 reference frame.
constexpr int BASE_W=1120,BASE_H=688,SPLASH_W=600,SPLASH_H=334;
constexpr U32 C_WINDOW=0xFF101113,C_RAIL=0xFF151719,C_HEADER=0xFF151719;
constexpr U32 C_MAIN=0xFF0D0F11,C_TILE=0xFF1C1E22,C_TILE_HOVER=0xFF282B30;
constexpr U32 C_WHITE=0xFFF1F2F3,C_DIM=0xFFB8C0C4,C_LINE=0xFF303235;
constexpr U32 C_PREVIEW=0xFF6E7075;
// Vivid crosshair palette; final circle opens the embedded HSV picker.
static U32 swatches[7]={0xFFFF5555,0xFFFFDB40,0xFF65FF63,0xFF25F5FF,0xFF5189FF,0xFFE05CFF,0xFFFFFFFF};
static U32 pickerField[200*124], pickerHueStrip[200*14];
static U32 importedPixels[128*128],candidatePixels[128*128];
struct ResChoice{int w,h;const wchar_t* name;const wchar_t* note;};
static const ResChoice resolutions[15]={
 {0,0,L"Native",L"Monitor default"},
 {1280,960,L"1280 x 960",L"4:3  /  standard"},
 {1024,768,L"1024 x 768",L"4:3  /  classic"},
 {1440,1080,L"1440 x 1080",L"4:3  /  high detail"},
 {1152,864,L"1152 x 864",L"4:3  /  balanced"},
 {1280,1024,L"1280 x 1024",L"5:4  /  compact"},
 {1600,1200,L"1600 x 1200",L"4:3  /  high detail"},
 {1600,1080,L"1600 x 1080",L"Custom  /  medium"},
 {1720,1080,L"1720 x 1080",L"Custom  /  mild"},
 {1728,1080,L"1728 x 1080",L"Custom  /  mild"},
 {1680,1050,L"1680 x 1050",L"16:10  /  subtle"},
 {1750,1080,L"1750 x 1080",L"Custom  /  subtle"},
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
 {2560,1440,L"2560 x 1440",L"16:9 / high detail"},
 {960,720,L"960 x 720",L"4:3 / compact"},
 {1280,864,L"1280 x 864",L"40:27 / stretched"},
 {1350,1080,L"1350 x 1080",L"5:4 / stretched"},
 {1536,1152,L"1536 x 1152",L"4:3 / high detail"},
 {1440,960,L"1440 x 960",L"3:2 / balanced"},
 {1620,1080,L"1620 x 1080",L"3:2 / high detail"},
 {1800,1080,L"1800 x 1080",L"5:3 / mild"},
 {1800,1440,L"1800 x 1440",L"5:4 / high detail"},
 {2160,1440,L"2160 x 1440",L"3:2 / high detail"}
};
constexpr int lastMode=15+sizeof(additionalResolutions)/sizeof(additionalResolutions[0]);
static const int modePages[][12]={
 {0,1,2,3,4,5,6,7,9,10,12,14},
 {28,29,30,31,32,33,34,35,36,22,26,18},
 {8,11,13,16,17,19,20,21,23,24,25,27}
};
static int displayPage=0;
static void revealMode(int id){
 for(int page=0;page<3;++page)for(int mode:modePages[page])if(id==mode){displayPage=page;return;}
}
static const ResChoice& getResChoice(int i){return i>=16&&i<=lastMode?additionalResolutions[i-16]:resolutions[clamp(i,0,14)];}
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


static U32 contentRevision=1;
static void repaint(){if(g.hwnd)InvalidateRect(g.hwnd,nullptr,0);}
static void invalidate(){++contentRevision;repaint();}
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
static void makePNGSlotPath(int index,wchar_t* out){
 wcopy(out,libraryFolder,512);wchar_t suffix[]=L"\\crosshair_00.png";
 suffix[11]=(wchar_t)(L'0'+(index+1)/10);suffix[12]=(wchar_t)(L'0'+(index+1)%10);
 wappend(out,512,suffix);
}
static bool loadPNGSlot(int index){
 if(index<0||index>=VELA_MAX_IMPORTS)return false;
 VelaLibraryPNG& png=libraryPNGs[index];
 if(png.image){GdipDisposeImage(png.image);png.image=nullptr;}
 void* img=nullptr;
 if(GdipCreateBitmapFromFile(png.path,&img)!=0||!img){png.ready=false;return false;}
 UINT w=0,h=0;
 if(GdipGetImageWidth(img,&w)!=0||GdipGetImageHeight(img,&h)!=0||!w||!h||w>4096||h>4096){
  GdipDisposeImage(img);png.ready=false;return false;
 }
 png.image=img;png.width=w;png.height=h;png.ready=true;return true;
}
static void loadPNGLibrary(){
 wcopy(libraryFolder,g.profileDirectory,512);wappend(libraryFolder,512,L"\\Crosshairs");
 CreateDirectoryW(g.profileDirectory,nullptr);CreateDirectoryW(libraryFolder,nullptr);
 for(int i=0;i<VELA_MAX_IMPORTS;i++){
  makePNGSlotPath(i,libraryPNGs[i].path);
  if(loadPNGSlot(i))libraryCount=i+1;
 }
}
// Activating a user PNG generates the overlay raster cache. No file I/O occurs
// while gaming; changes occur only when the user selects another crosshair.
static bool activatePNG(int index){
 if(index<0||index>=libraryCount||!libraryPNGs[index].ready)return false;
 if(g.importedImage){GdipDisposeImage(g.importedImage);g.importedImage=nullptr;g.importedReady=false;}
 if(!CopyFileW(libraryPNGs[index].path,g.customPath,0)||!loadImportedPNG()){
  setStatus(L"Could not activate this PNG. Try importing it again.");return false;
 }
 g.preset=61+index;
 if(g.overlay)InvalidateRect(g.overlay,nullptr,0);
 invalidate();return true;
}
static void importCrosshair(){
 if(libraryCount>=VELA_MAX_IMPORTS){setStatus(L"Crosshair library full (24 PNGs). Remove old files from AppData\\Kova\\Crosshairs to add more.");return;}
 wchar_t path[1024]={};
 OPENFILENAMEW dialog={};dialog.lStructSize=sizeof(dialog);dialog.hwndOwner=g.hwnd;
 dialog.lpstrFilter=L"Transparent PNG (*.png)\0*.png\0All files (*.*)\0*.*\0\0";
 dialog.lpstrFile=path;dialog.nMaxFile=1024;dialog.lpstrTitle=L"Import a transparent crosshair PNG";
 dialog.lpstrDefExt=L"png";dialog.Flags=0x00080000|0x00001000|0x00000008;
 if(!GetOpenFileNameW(&dialog))return;
 int target=libraryCount;
 // Validation is done before activating. Invalid PNGs do not create tiles.
 if(!CopyFileW(path,libraryPNGs[target].path,1)){
  setStatus(L"Import failed: could not copy the PNG into your local library.");return;
 }
 if(!loadPNGSlot(target)){
  DeleteFileW(libraryPNGs[target].path);
  setStatus(L"Could not open that PNG. Use a valid image under 4096 pixels per side.");return;
 }
 libraryCount++;
 if(!activatePNG(target)){
  if(libraryPNGs[target].image)GdipDisposeImage(libraryPNGs[target].image);
  libraryPNGs[target].image=nullptr;libraryPNGs[target].ready=false;libraryCount--;
  DeleteFileW(libraryPNGs[target].path);
  return;
 }
 g.crosshairPage=(60+target)/5;
 setStatus(L"PNG added to your crosshair library. Select it again any time from the last pages.");
}
static void applyColours(bool force);
static void applyOverlay();
static void revertDisplay();
static bool customTrial();

static void refreshGameFocus();
static void configureGameMonitor();
static bool coloursAllowed();
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
  wcopy(g.profileGames[i],g.profileDirectory,512);
  wchar_t gameSuffix[24]=L"\\Preset_1.game";
  gameSuffix[8]=(wchar_t)(L'1'+i);
  wappend(g.profileGames[i],512,gameSuffix);
 }
}
// Preset 12 is reserved for imported PNGs in previously saved profiles.
// Built-in IDs 0..11 and 13..60 form five pages of twelve presets.
constexpr int VELA_CUSTOM_PNG=12; // Legacy manual profile PNG ID.
static int totalCrosshairPages(){return (60+libraryCount+4)/5;}
static int crosshairAt(int page,int slot){
 int index=page*5+slot;
 if(index<0||index>=60+libraryCount)return -1;
 return index<12?index:index<60?index+1:61+index-60;
}
static int pageForCrosshair(int id){
 if(id>=61)return (60+id-61)/5;
 return (id==12?0:(id<=11?id:id-1)/5);
}

static bool validProfile(const ProfileData& d){
 return d.magic==PROFILE_MAGIC&&d.version==1&&d.bytes==sizeof(ProfileData)&&
  d.name[31]==0&&d.checksum==profileChecksum(d)&&
  d.preset>=0&&d.preset<=84&&d.size>=1&&d.size<=48&&
  d.thickness>=1&&d.thickness<=12&&d.gap>=0&&d.gap<=24&&
  d.opacity>=10&&d.opacity<=100&&d.selectedMode>=0&&d.selectedMode<=lastMode&&
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
static void saveGameBinding(int slot){
 if(!g.gameProcess[0]){DeleteFileW(g.profileGames[slot]);return;}
 SavedGameBinding b={};b.magic=0x454D4147u;b.version=1;
 b.enabled=g.gameFocusOnly?1:0;wcopy(b.executable,g.gameProcess,128);
 b.checksum=bindingHash(b);
 HANDLE file=CreateFileW(g.profileGames[slot],GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(file==(HANDLE)-1)return;
 DWORD wrote=0;WriteFile(file,&b,sizeof(b),&wrote,nullptr);CloseHandle(file);
}
static void loadGameBinding(int slot){
 g.gameFocusOnly=false;g.gameFocusActive=false;g.gameProcess[0]=0;
 SavedGameBinding b={};DWORD count=0;
 HANDLE file=CreateFileW(g.profileGames[slot],GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(file!=(HANDLE)-1){BOOL ok=ReadFile(file,&b,sizeof(b),&count,nullptr);CloseHandle(file);
  if(ok&&count==sizeof(b)&&b.magic==0x454D4147u&&b.version==1&&b.executable[127]==0&&
   b.checksum==bindingHash(b)){
   wcopy(g.gameProcess,b.executable,128);g.gameFocusOnly=(b.enabled!=0&&g.gameProcess[0]!=0);
  }
 }
 configureGameMonitor();refreshGameFocus();
}
static void saveSelectedProfile(){
 int i=g.activeProfile;
 // A custom PNG must travel with its saved slot so another import cannot overwrite it.
 if(!CreateDirectoryW(g.profileDirectory,nullptr)&&GetLastError()!=183){
  setStatus(L"Cannot create the preset directory. Check folder permissions.");return;
 }
 if(g.preset==12||g.preset>=61){
  if(!g.importedReady||!CopyFileW(g.customPath,g.profilePNGs[i],0)){
   setStatus(L"Could not save the imported PNG with this preset.");return;
  }
 }
 ProfileData d=captureProfile();
 HANDLE f=CreateFileW(g.profileFiles[i],GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(f==(HANDLE)-1){setStatus(L"Preset could not be saved. Check your AppData permissions.");return;}
 DWORD written=0;BOOL ok=WriteFile(f,&d,sizeof(d),&written,nullptr);CloseHandle(f);
 if(!ok||written!=sizeof(d)){setStatus(L"Preset save failed. Try again.");return;}
 if(d.preset!=12&&d.preset<61)DeleteFileW(g.profilePNGs[i]);
 g.storedProfiles[i]=d;g.profileExists[i]=true;
 saveGameBinding(i);
 setStatus(L"Preset saved. Vela will not load it automatically at startup.");
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
 if(d.preset==12||d.preset>=61){
  // GDI+ keeps Bitmap::FromFile open until disposed; release it before overwrite.
  if(g.importedImage){GdipDisposeImage(g.importedImage);g.importedImage=nullptr;g.importedReady=false;}
  if(!CopyFileW(g.profilePNGs[i],g.customPath,0))customReady=false;
  else customReady=loadImportedPNG();
 }
 g.preset=((d.preset==12||d.preset>=61)&&!customReady)?1:d.preset;
 if(g.preset>=61&&g.preset>=61+libraryCount)g.preset=12;
 g.crosshairPage=pageForCrosshair(g.preset);
 g.size=d.size;g.thickness=d.thickness;g.gap=d.gap;g.opacity=d.opacity;
 g.color=d.color&0xffffff;g.swatch=clamp(d.swatch,-1,6);rgbToHSV();
 g.selectedMode=d.selectedMode;revealMode(g.selectedMode);g.customW=d.customW;g.customH=d.customH;
 g.gameFilter=d.gameFilter;g.saturation=d.saturation;g.gamma=d.gamma;
 g.shadows=d.shadows;g.brightness=d.brightness;g.contrast=d.contrast;
 g.colorPreset=clamp(d.colorPreset,-1,2);
 g.desktopColorOn=d.desktopColorOn!=0;g.overlayOn=d.overlayOn!=0;
 g.minimizeToTray=d.minimizeToTray!=0;
 loadGameBinding(i);
 editResolutionFields();applyColours(true);applyOverlay();
 if(!customReady)setStatus(L"Preset loaded, but its custom PNG is missing. Default crosshair selected.");
 else setStatus(L"Preset loaded. To change display resolution, press Apply on the Display tab.");
}
static void deleteSelectedProfile(){
 int i=g.activeProfile;if(!g.profileExists[i]){setStatus(L"This preset slot is already empty.");return;}
 if(MessageBoxW(g.hwnd,L"Delete this saved preset? The current session will not change.",L"Vela - Delete preset",0x00000004|0x00000030)!=6)return;
 if(!DeleteFileW(g.profileFiles[i])){setStatus(L"Couldn't delete the preset file.");return;}
 DeleteFileW(g.profilePNGs[i]);DeleteFileW(g.profileGames[i]);g.profileExists[i]=false;
 memset(&g.storedProfiles[i],0,sizeof(ProfileData));syncProfileEditor();
 setStatus(L"Saved preset deleted. Current session settings were kept.");
}
static void resetSession(){
 if(g.keepPending)revertDisplay();
 g.preset=1;g.crosshairPage=0;g.size=19;g.thickness=5;g.gap=1;g.opacity=100;g.color=0x29B7FF;
 g.swatch=-1;rgbToHSV();g.selectedMode=0;displayPage=0;g.customW=1440;g.customH=1080;
 g.gameFilter=0;g.saturation=100;g.gamma=100;g.shadows=0;
 g.brightness=100;g.contrast=100;g.colorPreset=0;g.desktopColorOn=false;
 g.overlayOn=false;g.minimizeToTray=false;
 g.gameFocusOnly=false;g.gameFocusActive=false;g.gameProcess[0]=0;g.gameBindPending=false;
 configureGameMonitor();
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
static int contentOffset=0;
constexpr int sidebarExtra=0;
static void text(HDC dc,HFONT font,const wchar_t*s,int x,int y,int w,int h,U32 argb,UINT align=DT_LEFT){
 (void)dc;if(!s||textCount>=160)return;
 TextCommand &t=queuedText[textCount++];t.font=font;t.x=x+contentOffset;t.y=y;t.w=w;t.h=h;t.argb=argb;t.align=align;wcopy(t.value,s,256);
}
static void drawQueuedText(HDC dc){
 for(int i=0;i<textCount;i++){
  TextCommand &t=queuedText[i];SelectObject(dc,t.font);SetBkMode(dc,1);
  SetTextColor(dc,((t.argb&255)<<16)|(t.argb&0x00FF00)|((t.argb>>16)&255));
  RECT r={uiPixels(t.x),uiPixels(t.y),uiPixels(t.x+t.w),uiPixels(t.y+t.h)};
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
 // Use precisely the SAME embedded image as Vela.ico / taskbar branding.
 if(g.brandImage){GdipDrawImageRectI(p.gfx,g.brandImage,x-14,y-14,28,28);return;}
 // Fallback if image decoding was unavailable.
 roundRect(p,x-13,y-13,26,26,5,0xFF17191C);
 line(p,x-7,y-10,x-7,y+10,C_WHITE,3.8f);
 line(p,x+8,y-10,x-2,y,C_WHITE,3.8f);
 line(p,x-2,y,x+8,y+10,C_WHITE,3.8f);
 circle(p,x+10,y-11,2,0xFF25D486);
}
// The four navigation glyphs are traced as outlines, not generic placeholder circles.
// Paths are deliberately narrow (1.3 px strokes) and share one optical center,
// matching the supplied reference screenshot.
static const float kVelaPadOutline[42][2]={{-10.00f,-9.00f},{10.00f,-9.00f},{11.58f,-8.76f},{12.77f,-8.04f},{13.67f,-6.84f},{14.38f,-5.16f},{15.00f,-3.00f},{15.56f,-1.63f},{16.15f,0.33f},{16.62f,2.62f},{16.85f,5.00f},{16.69f,7.21f},{16.00f,9.00f},{14.80f,10.34f},{13.60f,10.87f},{12.40f,10.73f},{11.20f,10.06f},{10.00f,9.00f},{5.00f,5.50f},{3.70f,4.92f},{1.96f,4.57f},{0.00f,4.45f},{-1.96f,4.57f},{-3.70f,4.92f},{-5.00f,5.50f},{-10.00f,9.00f},{-11.20f,10.06f},{-12.40f,10.73f},{-13.60f,10.87f},{-14.80f,10.34f},{-16.00f,9.00f},{-16.69f,7.21f},{-16.85f,5.00f},{-16.62f,2.62f},{-16.15f,0.33f},{-15.56f,-1.63f},{-15.00f,-3.00f},{-14.38f,-5.16f},{-13.67f,-6.84f},{-12.77f,-8.04f},{-11.58f,-8.76f},{-10.00f,-9.00f}};
static const float kVelaGearOutline[48][2]={{-3.69f,-9.62f},{-2.49f,-9.99f},{-2.83f,-13.30f},{2.83f,-13.30f},{2.49f,-9.99f},{3.69f,-9.62f},{4.19f,-9.41f},{5.30f,-8.83f},{7.41f,-11.41f},{11.41f,-7.41f},{8.83f,-5.30f},{9.41f,-4.19f},{9.62f,-3.69f},{9.99f,-2.49f},{13.30f,-2.83f},{13.30f,2.83f},{9.99f,2.49f},{9.62f,3.69f},{9.41f,4.19f},{8.83f,5.30f},{11.41f,7.41f},{7.41f,11.41f},{5.30f,8.83f},{4.19f,9.41f},{3.69f,9.62f},{2.49f,9.99f},{2.83f,13.30f},{-2.83f,13.30f},{-2.49f,9.99f},{-3.69f,9.62f},{-4.19f,9.41f},{-5.30f,8.83f},{-7.41f,11.41f},{-11.41f,7.41f},{-8.83f,5.30f},{-9.41f,4.19f},{-9.62f,3.69f},{-9.99f,2.49f},{-13.30f,2.83f},{-13.30f,-2.83f},{-9.99f,-2.49f},{-9.62f,-3.69f},{-9.41f,-4.19f},{-8.83f,-5.30f},{-11.41f,-7.41f},{-7.41f,-11.41f},{-5.30f,-8.83f},{-4.19f,-9.41f}};
static void navOutline(Painter&p,const float coords[][2],int n,int x,int y,U32 col,float thickness){
 void* path=nullptr;GdipCreatePath(0,&path);if(!path)return;
 for(int i=0;i<n;i++){
  int next=(i+1)%n;
  GdipAddPathLine(path,(float)x+coords[i][0],(float)y+coords[i][1],
                      (float)x+coords[next][0],(float)y+coords[next][1]);
 }
 GdipClosePathFigure(path);
 void* pen=nullptr;GdipCreatePen1(col,thickness,2,&pen);
 if(pen){GdipDrawPath(p.gfx,pen,path);GdipDeletePen(pen);}
 GdipDeletePath(path);
}
// ItsHover-inspired motion language redrawn natively for Vela: no React,
// CSS, animated SVG runtime, or extra dependencies. Each glyph has its own
// intentional animation and remains inside its fixed 45px navigation hit area.
static float navEase(float t){t=fclamp(t,0.0f,1.0f);return t*t*(3.0f-2.0f*t);}
static float navAbs(float t){return t<0?-t:t;}
static void navStroke(Painter&p,float x1,float y1,float x2,float y2,U32 color,float thickness){
 void*pen=nullptr;GdipCreatePen1(color,thickness,2,&pen);
 if(pen){GdipDrawLine(p.gfx,pen,x1,y1,x2,y2);GdipDeletePen(pen);}
}
static void navCurve(Painter&p,float ax,float ay,float bx,float by,float cx,float cy,float dx,float dy,U32 color,float thickness){
 void*pen=nullptr;GdipCreatePen1(color,thickness,2,&pen);
 if(pen){GdipDrawBezier(p.gfx,pen,ax,ay,bx,by,cx,cy,dx,dy);GdipDeletePen(pen);}
}
// ItsHover-inspired, per-glyph micro-interactions. None of the icons rotate,
// scale, jump, or create bright hover plates. Only internal strokes change.
// Hover animations are independent and smoothly return to rest on mouse exit.
static float navPulse(float phase){
 phase=fclamp(phase,0.0f,1.0f);
 float triangle=phase<0.5f?phase*2.0f:(1.0f-phase)*2.0f;
 return navEase(triangle);
}
static void navIcon(Painter&p,int type,int x,int y,U32 c,float motion=0.0f,float progress=0.0f){
 motion=navEase(motion);
 progress=fclamp(progress,0.0f,1.0f);
 if(type==0){ // Crosshair: ticks converge and relax around a fixed target.
  float converge=motion*navPulse(progress)*2.8f;
  ring(p,x,y,10,c,1.5f);
  circle(p,x,y,2+(int)(motion*navPulse(progress)*0.75f),c);
  float outer=17.0f-converge,inner=12.4f-converge;
  navStroke(p,(float)x,(float)y-outer,(float)x,(float)y-inner,c,1.6f);
  navStroke(p,(float)x+inner,(float)y,(float)x+outer,(float)y,c,1.6f);
  navStroke(p,(float)x,(float)y+inner,(float)x,(float)y+outer,c,1.6f);
  navStroke(p,(float)x-outer,(float)y,(float)x-inner,(float)y,c,1.6f);
 }else if(type==1){ // Eye: fluid blink with a gently shifting pupil.
  float blink=0.0f;
  if(progress>0.41f&&progress<0.58f){
   blink=navEase(fclamp(1.0f-navAbs(progress-0.495f)/0.085f,0,1))*motion;
  }
  float open=1.0f-0.94f*blink;
  navCurve(p,x-14.5f,y,x-7.5f,y-9.6f*open,x+7.5f,y-9.6f*open,x+14.5f,y,c,1.5f);
  navCurve(p,x-14.5f,y,x-7.5f,y+9.6f*open,x+7.5f,y+9.6f*open,x+14.5f,y,c,1.5f);
  if(open>0.3f){
   float pupilX=(float)x+motion*(progress<0.5f?progress*3.2f:(1.0f-progress)*3.2f)-motion*0.8f;
   ring(p,(int)pupilX,y,4,c,1.5f);
   circle(p,(int)pupilX,y,1,c);
  }
 }else if(type==2){ // Monitor: a faint scan line sweeps down the screen.
  strokedRoundRect(p,x-13,y-11,26,20,3,c,1.45f);
  navStroke(p,(float)x,(float)y+9,(float)x,(float)y+13,c,1.45f);
  navStroke(p,(float)x-7,(float)y+13,(float)x+7,(float)y+13,c,1.45f);
  if(motion>0.01f){
   float scanY=(float)y-8.0f+progress*14.0f;
   U8 alpha=(U8)(motion*135.0f);
   navStroke(p,(float)x-9.3f,scanY,(float)x+9.3f,scanY,
     (c&0x00FFFFFF)|((U32)alpha<<24),1.3f);
  }
 }else if(type==3){ // Settings: stable cog; the inner tuning mark pulses.
  navOutline(p,kVelaGearOutline,
    (int)(sizeof(kVelaGearOutline)/sizeof(kVelaGearOutline[0])),x,y,c,1.4f);
  ring(p,x,y,4,c,1.4f);
  float pulse=motion*navPulse(progress);
  // A small adjustment indicator slides inside the hub; the cog never turns.
  float sx=(float)x-1.4f+pulse*2.8f;
  navStroke(p,sx,(float)y-2.2f,sx,(float)y+2.2f,c,1.45f);
  if(pulse>0.05f){
    U8 alpha=(U8)(motion*55.0f);
    ring(p,x,y,6,(c&0x00FFFFFF)|((U32)alpha<<24),1.0f);
  }
 }
}

// Shared geometry ensures page 2-5 presets match between preview and overlay.
struct CrossShape {int kind,x1,y1,x2,y2,r,t;}; // 0 line, 1 dot, 2 outline ring
static void shLine(CrossShape* o,int&n,int x1,int y1,int x2,int y2,int t){if(n<64)o[n++]={0,x1,y1,x2,y2,0,t};}
static void shDot(CrossShape* o,int&n,int x,int y,int r){if(n<64)o[n++]={1,x,y,0,0,r,0};}
static void shRing(CrossShape* o,int&n,int x,int y,int r,int t){if(n<64)o[n++]={2,x,y,0,0,r,t};}
static int makeExtraCross(int preset,int size,int thick,int gap,bool miniature,CrossShape* o){
 int index=preset-13,group=index/12,kind=index%12,n=0;
 int l=miniature?16:clamp(size*2,5,80),m=miniature?9:clamp(size,4,38);
 int g=miniature?5:clamp(gap+4,2,32),t=miniature?2:clamp(thick,1,12);
 int d=miniature?3:clamp(thick,2,7),a=miniature?10:clamp(size+3,5,45);
 if(group==0){ // Dot patterns: small targets that don't require four cross arms.
  if(kind==0||kind==2||kind==4){shDot(o,n,-g,0,d);shDot(o,n,g,0,d);}
  if(kind==1||kind==2||kind==4){shDot(o,n,0,-g,d);shDot(o,n,0,g,d);}
  if(kind==3||kind==4||kind==5){for(int x=-1;x<=1;x+=2)for(int y=-1;y<=1;y+=2)shDot(o,n,x*g,y*g,d);}
  if(kind==5||kind==9)shDot(o,n,0,0,d);
  if(kind==6||kind==7){for(int i=-1;i<=1;i++)shDot(o,n,kind==6?i*(g+d):0,kind==7?i*(g+d):0,d);}
  if(kind==8){for(int i=-1;i<=1;i++)shDot(o,n,i*(g+d),i*(g+d),d);}
  if(kind==9){shRing(o,n,0,0,a,t);}
  if(kind==10){for(int i=0;i<4;i++)shLine(o,n, (i%2?1:-1)*g, (i/2?1:-1)*g,(i%2?1:-1)*(g+m/2),(i/2?1:-1)*g,t);}
  if(kind==11){shLine(o,n,0,-l,0,-g,t);shLine(o,n,0,g,0,l,t);shDot(o,n,0,0,d);}
 }else if(group==1){ // Directional angles, chevrons and diagonal sights.
  if(kind<=3){for(int x=-1;x<=1;x+=2)for(int y=-1;y<=1;y+=2)shLine(o,n,x*l,y*l,x*g,y*g,t);
    if(kind==0){shLine(o,n,-g,-g,g,g,t);shLine(o,n,g,-g,-g,g,t);}
    if(kind==2)shRing(o,n,0,0,a/2,t);
    if(kind==3)shDot(o,n,0,0,d);
  }
  if(kind>=4&&kind<=7){int dir=kind-4;
   if(dir==4-4){shLine(o,n,-l,m/2,0,-m,t);shLine(o,n,0,-m,l,m/2,t);}
   if(dir==1){shLine(o,n,-l,-m/2,0,m,t);shLine(o,n,0,m,l,-m/2,t);}
   if(dir==2){shLine(o,n,m/2,-l,-m,0,t);shLine(o,n,-m,0,m/2,l,t);}
   if(dir==3){shLine(o,n,-m/2,-l,m,0,t);shLine(o,n,m,0,-m/2,l,t);}
  }
  if(kind==8||kind==9){int sign=kind==8?-1:1;
   shLine(o,n,0,sign*l,-l,-sign*m,t);shLine(o,n,0,sign*l,l,-sign*m,t);shLine(o,n,-l,-sign*m,l,-sign*m,t);}
  if(kind==10){shLine(o,n,-l,-m,l,m,t);shLine(o,n,l,-m,-l,m,t);shLine(o,n,-l,-m,l,-m,t);shLine(o,n,-l,m,l,m,t);}
  if(kind==11){shLine(o,n,0,-l,l,0,t);shLine(o,n,l,0,0,l,t);shLine(o,n,0,l,-l,0,t);shLine(o,n,-l,0,0,-l,t);shDot(o,n,0,0,d);}
 }else if(group==2){ // Broken frames: tactical sights, small brackets and boxes.
  if(kind==0||kind==1||kind==7||kind==10){for(int x=-1;x<=1;x+=2)for(int y=-1;y<=1;y+=2){
    shLine(o,n,x*g,y*a,x*a,y*a,t);shLine(o,n,x*a,y*g,x*a,y*a,t);}}
  if(kind==1)shDot(o,n,0,0,d);
  if(kind==2||kind==3){int sign=kind==2?1:-1;shLine(o,n,-l,-sign*m,-l,sign*m,t);shLine(o,n,-l,sign*m,l,sign*m,t);shLine(o,n,l,sign*m,l,-sign*m,t);}
  if(kind==4||kind==5){int sign=kind==4?1:-1;
    shLine(o,n,-l,-sign*l,-g,-sign*l,t);shLine(o,n,-l,-sign*l,-l,-sign*g,t);
    shLine(o,n,l,sign*l,g,sign*l,t);shLine(o,n,l,sign*l,l,sign*g,t);}
  if(kind==6){shLine(o,n,-l,-m,l,-m,t);shLine(o,n,-l,m,l,m,t);}
  if(kind==7)shRing(o,n,0,0,a/2,t);
  if(kind==8){shLine(o,n,-l,-m,-g,-m,t);shLine(o,n,g,-m,l,-m,t);shLine(o,n,-l,m,-g,m,t);shLine(o,n,g,m,l,m,t);}
  if(kind==9){shLine(o,n,-m,-m,m,-m,t);shLine(o,n,m,-m,m,m,t);shLine(o,n,m,m,-m,m,t);shLine(o,n,-m,m,-m,-m,t);shDot(o,n,0,0,d);}
  if(kind==10)shDot(o,n,0,0,d);
  if(kind==11){for(int i=-1;i<=1;i+=2)for(int j=-1;j<=1;j+=2){shDot(o,n,i*m,j*m,d);shLine(o,n,i*m,j*l,i*l,j*l,t);}}
 }else{ // Hybrid designs: compact combinations of lines, dots and rings.
  if(kind==0||kind==1||kind==2||kind==3||kind==4||kind==9||kind==10){
   shLine(o,n,-l,0,-g,0,t);shLine(o,n,g,0,l,0,t);
   shLine(o,n,0,-l,0,-g,t);shLine(o,n,0,g,0,l,t);
  }
  if(kind==0||kind==4||kind==7)shRing(o,n,0,0,a,t);
  if(kind==1||kind==3||kind==6||kind==10)shDot(o,n,0,0,d);
  if(kind==2||kind==8||kind==9)for(int x=-1;x<=1;x+=2)for(int y=-1;y<=1;y+=2)shLine(o,n,x*m,y*m,x*g,y*g,t);
  if(kind==3||kind==9)for(int x=-1;x<=1;x+=2)for(int y=-1;y<=1;y+=2)shDot(o,n,x*m,y*m,d);
  if(kind==5){shRing(o,n,0,0,a,t);shRing(o,n,0,0,imax(3,a/2),t);}
  if(kind==6){shRing(o,n,0,0,a+2,t);shRing(o,n,0,0,imax(3,a*2/3),t);shRing(o,n,0,0,imax(2,a/3),t);}
  if(kind==7){for(int x=-1;x<=1;x+=2)for(int y=-1;y<=1;y+=2){
   shLine(o,n,x*g,y*l,x*m,y*l,t);shLine(o,n,x*l,y*g,x*l,y*m,t);}}
  if(kind==8)shDot(o,n,0,0,d);
  if(kind==10)for(int x=-1;x<=1;x+=2)for(int y=-1;y<=1;y+=2){shLine(o,n,x*m,y*l,x*l,y*l,t);}
  if(kind==11){shRing(o,n,0,0,a,t);shLine(o,n,-l,0,-a-3,0,t);shLine(o,n,a+3,0,l,0,t);shLine(o,n,0,-l,0,-a-3,t);shLine(o,n,0,a+3,0,l,t);shDot(o,n,0,0,d);}
 }
 return n;
}
static void drawExtraCross(Painter&p,int style,int x,int y,int size,int thick,int gap,U32 color,bool miniature){
 CrossShape ops[64];int count=makeExtraCross(style,size,thick,gap,miniature,ops);
 for(int i=0;i<count;i++){
  const CrossShape& a=ops[i];
  if(a.kind==0)line(p,x+a.x1,y+a.y1,x+a.x2,y+a.y2,color,(float)a.t);
  else if(a.kind==1)circle(p,x+a.x1,y+a.y1,a.r,color);
  else ring(p,x+a.x1,y+a.y1,a.r,color,(float)a.t);
 }
}
static void drawCross(Painter&p,int style,int x,int y,int size,int thick,int gap,U32 color,bool miniature){
 if(style>=13&&style<=60){drawExtraCross(p,style,x,y,size,thick,gap,color,miniature);return;}
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

// Directional light across a translucent surface and its independently lit rim.
DLL int API GdipCreateLineBrushI(const POINT*,const POINT*,U32,U32,int,void**);
DLL int API GdipSetLinePresetBlend(void*,const U32*,const float*,int);
DLL int API GdipCreatePen2(void*,float,int,void**);
static void glass(Painter& p,int x,int y,int w,int h,int radius,U32 light,U32 shade,bool lifted=false){
 if(lifted){
  roundRect(p,x,y+4,w,h,radius,0x14000000);
  roundRect(p,x,y+2,w,h,radius,0x23000000);
 }
 void* path=nullptr;rrPath(&path,(float)x,(float)y,(float)w,(float)h,(float)radius);
 if(!path)return;
 POINT a={x+w/5,y},b={x+w*2/3,y+h};
 void* brush=nullptr;
 if(!GdipCreateLineBrushI(&a,&b,light,shade,3,&brush)){
  GdipFillPath(p.gfx,brush,path);GdipDeleteBrush(brush);
 }
 POINT c={x,y+h},d={x+w,y};
 if(!GdipCreateLineBrushI(&c,&d,0x637F898E,0x769CA5AD,3,&brush)){
  void* pen=nullptr;
  const U32 edge[]={0x4A666D78,0x308693A9,0x25474C65,0x467D849A};
  const float stops[]={0.0f,0.21f,0.72f,1.0f};
  GdipSetLinePresetBlend(brush,edge,stops,4);
  if(!GdipCreatePen2(brush,0.8f,2,&pen)){GdipDrawPath(p.gfx,pen,path);GdipDeletePen(pen);}
  GdipDeleteBrush(brush);
 }
 GdipDeletePath(path);
}
static void panel(Painter& p,int x,int y,int w,int h){
 glass(p,x,y,w,h,18,0xF5181B21,0xF20E1014,false);
}
// The low-frequency light field is rasterized once and reused by every page.
static U32 lightPixels[300*160];
static void* lightImage=nullptr;
static float glow(float x,float y,float cx,float cy,float rx,float ry){
 float dx=(x-cx)/rx,dy=(y-cy)/ry;
 float f=1+dx*dx+dy*dy;return 1/(f*f*f);
}
static void backdrop(Painter& p){
 if(!lightImage){
  for(int y=0;y<160;++y)for(int x=0;x<300;++x){
   float mist=glow(x,y,162,45,160,90);
   lightPixels[y*300+x]=RGBX((U8)(5+6*mist),(U8)(7+6*mist),(U8)(10+9*mist));
  }
  GdipCreateBitmapFromScan0(300,160,1200,0x26200A,(BYTE*)lightPixels,&lightImage);
 }
 if(lightImage)GdipDrawImageRectI(p.gfx,lightImage,0,0,g.w,g.h);
 else solidRect(p,0,0,g.w,g.h,0xFF202638);
}
static BOOL motionEnabled=1;
struct MotionValue{float current,target,from;U32 start;bool ready;};
static bool frameRequested=false,paintingContent=false,contentMoving=false;
static void requestFrame(){
 if(!frameRequested&&motionEnabled){frameRequested=true;SetTimer(g.hwnd,12,16,nullptr);}
}
// Retarget from the visible position, and stop the frame clock after settling.
static float animate(MotionValue& value,float target,float duration=185.0f){
 U32 now=GetTickCount();
 if(!value.ready||!motionEnabled){value={target,target,target,now,true};return target;}
 if(value.target!=target){value.from=value.current;value.target=target;value.start=now;}
 float u=fclamp((now-value.start)/duration,0,1),v=1-u;
 value.current=target+(value.from-target)*v*v*v;
 if(u<1&&value.from!=target){if(paintingContent)contentMoving=true;requestFrame();}
 return value.current;
}
static void toggle(Painter& p,int x,int y,int width,bool on,MotionValue& value){
 float position=animate(value,on?1.0f:0.0f);
 glass(p,x,y,width,26,13,on?tint(234):0xBB50565F,on?tint(174):0xDD272B31);
 circle(p,x+13+(int)((width-26)*position),y+13,9,0xFFEEF4F0);
}
static bool manualUpdateBusy=false;
static int focusedTab=-1;
static MotionValue switches[4],railPosition;
static void buildColourMatrix(float* m);
static MotionValue thumbMotion[2][5],modeMotion;
static int visY(int control){return 182+control*74;}
constexpr int visX=139,visWidth=397,modeX=638,modeY=89,segmentWidth=141;
static void slider(Painter&p,HDC dc,const wchar_t*label,int value,int min,int max,int x,int y,int width){
 text(dc,g.fontBody,label,x,y,width-55,24,C_WHITE);
 numText(dc,g.fontBody,value,x,y,width,24,C_DIM);
 int trackY=y+41, sliderW=width;
 roundRect(p,x,trackY,sliderW,3,2,0x66070A0E);
 int index=g.tab==2?(y-visY(0))/74:(y-253)/65;
 int target=(value-min)*sliderW/(max-min);
 MotionValue& thumbState=thumbMotion[g.tab==2][clamp(index,0,4)];
 bool dragging=g.mouseDown&&index==g.downSlider;
 if(dragging)thumbState={(float)target,(float)target,(float)target,GetTickCount(),true};
 int thumb=x+(dragging?target:(int)animate(thumbState,(float)target));
 roundRect(p,x,trackY,imax(2,thumb-x),3,2,tint(240));
 circle(p,thumb,trackY+2,8,0x40090D11);
 circle(p,thumb,trackY+1,6,0xFFF0F9FF);
}
static int pointerX=-999,pointerY=-999;
struct HoverArea{int x,y,w,h,r;};
constexpr int tabCount=7;
static HoverArea areas[tabCount][48];static int areaCount[tabCount]={};
static int hoveredArea=-1;
static void addArea(int x,int y,int w,int h,int r){
 int& count=areaCount[g.tab];if(count<48)areas[g.tab][count++]={x,y,w,h,r};
}
static int areaAt(int x,int y){
 for(int i=areaCount[g.tab]-1;i>=0;--i){
  const HoverArea& a=areas[g.tab][i];if(hit(x,y,a.x,a.y,a.w,a.h))return i;
 }
 return -1;
}
static void button(Painter&p,HDC dc,int x,int y,int w,int h,const wchar_t*s,bool active=false,bool danger=false){
 U32 base=active?0xFF77D5A6:0xFF292E34;
 if(danger)base=0xFF37292C;
 glass(p,x,y,w,h,h/2,active?tint(215):0xDB242932,active?tint(158):0xEE14171B,true);
 addArea(x,y,w,h,h/2);
 text(dc,g.fontSmall,s,x+10,y,w-20,h,active?0xFFFFFFFF:C_WHITE,DT_CENTER);
}
// The compact launch window uses only low-cost, deterministic animations.
// 64-point orbit vectors are precomputed: no math runtime or shaders needed.
static const float kOrbX[64]={1.0000000f,0.9951847f,0.9807853f,0.9569403f,0.9238795f,0.8819213f,0.8314696f,0.7730105f,0.7071068f,0.6343933f,0.5555702f,0.4713967f,0.3826834f,0.2902847f,0.1950903f,0.0980171f,0.0000000f,-0.0980171f,-0.1950903f,-0.2902847f,-0.3826834f,-0.4713967f,-0.5555702f,-0.6343933f,-0.7071068f,-0.7730105f,-0.8314696f,-0.8819213f,-0.9238795f,-0.9569403f,-0.9807853f,-0.9951847f,-1.0000000f,-0.9951847f,-0.9807853f,-0.9569403f,-0.9238795f,-0.8819213f,-0.8314696f,-0.7730105f,-0.7071068f,-0.6343933f,-0.5555702f,-0.4713967f,-0.3826834f,-0.2902847f,-0.1950903f,-0.0980171f,-0.0000000f,0.0980171f,0.1950903f,0.2902847f,0.3826834f,0.4713967f,0.5555702f,0.6343933f,0.7071068f,0.7730105f,0.8314696f,0.8819213f,0.9238795f,0.9569403f,0.9807853f,0.9951847f};
static const float kOrbY[64]={0.0000000f,0.0980171f,0.1950903f,0.2902847f,0.3826834f,0.4713967f,0.5555702f,0.6343933f,0.7071068f,0.7730105f,0.8314696f,0.8819213f,0.9238795f,0.9569403f,0.9807853f,0.9951847f,1.0000000f,0.9951847f,0.9807853f,0.9569403f,0.9238795f,0.8819213f,0.8314696f,0.7730105f,0.7071068f,0.6343933f,0.5555702f,0.4713967f,0.3826834f,0.2902847f,0.1950903f,0.0980171f,0.0000000f,-0.0980171f,-0.1950903f,-0.2902847f,-0.3826834f,-0.4713967f,-0.5555702f,-0.6343933f,-0.7071068f,-0.7730105f,-0.8314696f,-0.8819213f,-0.9238795f,-0.9569403f,-0.9807853f,-0.9951847f,-1.0000000f,-0.9951847f,-0.9807853f,-0.9569403f,-0.9238795f,-0.8819213f,-0.8314696f,-0.7730105f,-0.7071068f,-0.6343933f,-0.5555702f,-0.4713967f,-0.3826834f,-0.2902847f,-0.1950903f,-0.0980171f};
constexpr int enterY=246,enterWidth=136,enterHeight=40;
static bool enterHit(int x,int y){return hit(x,y,(g.w-enterWidth)/2,enterY-enterHeight/2,enterWidth,enterHeight);}
static void enterDashboard(){
 if(g.introPhase!=5)return;
 g.introPhase=2;g.introStageTick=GetTickCount();SetTimer(g.hwnd,5,16,nullptr);invalidate();
}
static void velaMark(Painter& p,int x,int y,int r){
 const int pieces[2][8]={{-12,-9,-7,-9,1,7,-2,12},{8,-10,13,-10,2,12,0,7}};
 void* fill=nullptr;if(GdipCreateSolidFill(C_WHITE,&fill))return;
 for(const auto& poly:pieces){
  void* path=nullptr;if(GdipCreatePath(0,&path))continue;
  for(int i=0;i<4;++i){int j=(i+1)%4;
   GdipAddPathLine(path,x+r*poly[i*2]/24.0f,y+r*poly[i*2+1]/24.0f,x+r*poly[j*2]/24.0f,y+r*poly[j*2+1]/24.0f);
  }
  GdipClosePathFigure(path);GdipFillPath(p.gfx,fill,path);GdipDeletePath(path);
 }
 GdipDeleteBrush(fill);
}
// Drift and travelling reflections share a clock across all entrance stages.
static float launchWave(U32 elapsed,U32 period){
 float step=(elapsed%period)*(64.0f/period);
 int i=(int)step;
 return kOrbX[i%64]+(kOrbX[(i+1)%64]-kOrbX[i%64])*(step-i);
}
static void edgeLights(Painter& p,U32 elapsed){
 for(int edge=0;edge<2;++edge){
  float drift=launchWave(elapsed+edge*1700,7900);
  float peak=.5f+.24f*launchWave(elapsed+1400+edge*1100,5200);
  int left=edge?g.w:0,dir=edge?-1:1;
  POINT start={left,0},end={left+dir*350,0};
  void* brush=nullptr;
  if(GdipCreateLineBrushI(&start,&end,tint(),tint(),3,&brush))continue;
  float stops[]={0,peak-.22f,peak,peak+.22f,1};
  float y=edge?(float)g.h:0,dy=edge?-1.0f:1.0f;
  for(int layer=0;layer<4;++layer){
   const int alpha[]={12,26,58,226};
   const float widths[]={20,10,4,1.45f};
   int a=alpha[layer];
   U32 colours[]={tint(158,1),tint(216,a/5),tint(255,a),tint(216,a/5),tint(158,1)};
   GdipSetLinePresetBlend(brush,colours,stops,5);
   void* pen=nullptr;
   if(!GdipCreatePen2(brush,widths[layer],2,&pen)){
    GdipDrawBezier(p.gfx,pen,left-dir*14.0f,y+dy*(170+drift*20),
      left+dir*65.0f,y+dy*(70+drift*13),left+dir*184.0f,y+dy*(24-drift*10),
      left+dir*358.0f,y-dy*9);GdipDeletePen(pen);
   }
  }
  GdipDeleteBrush(brush);
 }
}
static void drawSplash(Painter& p,HDC dc){
 const int cx=g.w/2;
 backdrop(p);edgeLights(p,motionEnabled?GetTickCount()-g.introStartTick:0);
 strokedRoundRect(p,1,1,g.w-2,g.h-2,14,0x4A8996AF,1);
 line(p,g.w-90,23,g.w-78,23,0xA7DFE6F0,1);
 line(p,g.w-39,17,g.w-28,28,0xA7DFE6F0,1);
 line(p,g.w-28,17,g.w-39,28,0xA7DFE6F0,1);
 U32 now=GetTickCount();
 float title=motionEnabled?fclamp((now-g.introStartTick)/280.0f,0,1):1;
 velaMark(p,cx,103,35);
 text(dc,g.fontSplash,L"V E L A",cx-155,119+(int)(6*(1-title)),310,63,
  RGBX((U8)(69+168*title),(U8)(77+165*title),(U8)(99+150*title)),DT_CENTER);
 float reveal=g.introPhase==0||g.introPhase==1?0:
  g.introPhase==4?fclamp((now-g.introStageTick)/260.0f,0,1):1;
 float v=1-reveal,ease=1-v*v*v;
 int width=202-(int)((202-enterWidth)*ease),height=3+(int)((enterHeight-3)*ease);
 int x=cx-width/2,y=enterY-height/2;
 if(reveal==0){
  text(dc,g.fontTiny,L"I N I T I A L I S I N G",cx-125,y-50,250,30,C_DIM,DT_CENTER);
  numText(dc,g.fontTiny,g.introProgress,x+width+1,y-11,30,24,C_WHITE,DT_RIGHT);text(dc,g.fontTiny,L"%",x+width+32,y-11,18,24,C_WHITE);
  roundRect(p,x,y,width,6,3,0xFF1E242C);
  int fill=width*g.introProgress/100;
  if(fill>1)roundRect(p,x,y,fill,6,3,tint());
 }else{
  static MotionValue hover;
  float h=animate(hover,g.introPhase==5&&g.introHoverTarget?1.0f:0.0f);
  glass(p,x,y,width,height,height/2,tint(112+(int)(24*h),210),tint(64,221));
  if(reveal>0.35f){
   float t=(reveal-.35f)/.65f;U32 ink=RGBX((U8)(90+145*t),(U8)(106+137*t),(U8)(130+120*t));
   text(dc,g.fontBody,L"Enter",cx-47,y,72,height,ink,DT_CENTER);
   int ax=cx+28+(int)(3*h);
   line(p,ax-6,enterY,ax+4,enterY,ink,1.2f);
   line(p,ax,enterY-4,ax+4,enterY,ink,1.2f);line(p,ax,enterY+4,ax+4,enterY,ink,1.2f);
  }
 }
}
static const int kNavTabs[tabCount]={5,0,1,4,2,6,3}; // Target, Game Visibility, Display, Settings.
// Vela ambient green meteor field. No sprite textures, GPU hooks, or input.
// ~11 staggered, moving streaks are visible at a time (rather than ~1).
// Slow outer tails and bright inner cores give an animated "shooting stars"
// impression while staying behind ALL controls, never changing hit regions.
static void drawAmbientStars(Painter&p){
 U32 now=GetTickCount();
 constexpr U32 CYCLE=4200, ACTIVE=1700;
 for(int i=0;i<28;i++){
  // Individual lifetime offset and location are stable between frames.
  U32 phase=(now+(U32)((i*1597u)%CYCLE))%CYCLE;
  if(phase>=ACTIVE)continue;
  float u=(float)phase/(float)ACTIVE;
  float appear=fclamp(u*7.0f,0.0f,1.0f);
  float vanish=fclamp((1.0f-u)*7.0f,0.0f,1.0f);
  float fade=appear*vanish;
  // Prime-ish hash increments prevent row/column patterns.
  int x0=117+((i*173+37)%852);
  int y0=83+((i*137+29)%477);
  int travel=135+(i%6)*25;
  int x=x0+(int)(u*(float)travel);
  int y=y0+(int)(u*(float)(travel*0.36f));
  // Graceful clipping: nothing may draw over the narrow icon rail.
  if(x<104||x>1088||y<66||y>617)continue;
  int a=clamp((int)((59+(i%5)*9)*fade),0,95);
  if(a<2)continue;
  // Three tail layers, from diffused to bright. GDI+ alpha blended.
  int tail=37+(i%5)*8;
  int dY=(int)((float)tail*0.36f);
  line(p,imax(72,x-tail-20),y-dY-7,x,y,ARGB((U8)(a*38/100),27,142,68),1.15f);
  line(p,imax(72,x-tail),y-dY,x,y,ARGB((U8)(a*69/100),72,215,114),1.25f);
  line(p,imax(72,x-tail/2),y-dY/2,x,y,ARGB((U8)a,151,255,181),1.15f);
  circle(p,x,y,1,ARGB((U8)clamp(a+43,0,148),166,255,196));
 }
}
static int railY(int i){return 100+72*i;}
static bool tabHit(int x,int y,int i){return hit(x,y,14,railY(i)-24,56,48);}
DLL int API GdipSetPenLineJoin(void*,int);
DLL int API GdipSetPenLineCap197819(void*,int,int,int);
static void iconPath(Painter& p,void* pen,int x,int y,const float points[][2],int count,bool closed){
 void* path=nullptr;if(GdipCreatePath(0,&path))return;
 for(int i=1;i<count;++i)GdipAddPathLine(path,x+points[i-1][0],y+points[i-1][1],x+points[i][0],y+points[i][1]);
 if(closed)GdipClosePathFigure(path);
 GdipDrawPath(p.gfx,pen,path);GdipDeletePath(path);
}
static void sectionIcon(Painter& p,int id,int x,int y,U32 colour){
 void* pen=nullptr;if(GdipCreatePen1(colour,2.65f,2,&pen))return;
 GdipSetPenLineJoin(pen,2);GdipSetPenLineCap197819(pen,2,2,0);
 if(id==5){
  const float roof[][2]={{-15,-1},{0,-14},{15,-1}};
  const float walls[][2]={{-11,-2},{-11,12},{-3,12},{-3,4},{3,4},{3,12},{11,12},{11,-2}};
  iconPath(p,pen,x,y,roof,3,false);iconPath(p,pen,x,y,walls,8,false);
 }else if(id==6){
  strokedRoundRect(p,x-14,y-12,7,25,2,colour,2.4f);
  strokedRoundRect(p,x-4,y-12,7,25,2,colour,2.4f);
  const float book[][2]={{6,-9},{11,-10},{16,11},{11,12}};
  iconPath(p,pen,x,y,book,4,true);
 }else if(id==0){
  ring(p,x,y,11,colour,2.65f);circle(p,x,y,2,colour);
  for(int d=-1;d<=1;d+=2){
   GdipDrawLineI(p.gfx,pen,x+14*d,y,x+17*d,y);
   GdipDrawLineI(p.gfx,pen,x,y+14*d,x,y+17*d);
  }
 }else if(id==1){
  strokedRoundRect(p,x-16,y-12,32,21,5,colour,2.65f);
  GdipDrawLineI(p.gfx,pen,x,y+10,x,y+15);GdipDrawLineI(p.gfx,pen,x-7,y+15,x+7,y+15);
 }else if(id==4){
  const float bolt[][2]={{5,-15},{-12,3},{-1,3},{-5,15},{12,-4},{2,-4}};
  iconPath(p,pen,x,y,bolt,6,true);
 }else if(id==2){
  const float top[][2]={{-15,-7},{0,-14},{15,-7},{0,0}};
  const float seam[][2]={{-14,-1},{0,6},{14,-1}};
  iconPath(p,pen,x,y,top,4,true);iconPath(p,pen,x,y,seam,3,false);iconPath(p,pen,x,y+7,seam,3,false);
 }else{
  float teeth[48][2];
  for(int i=0;i<48;++i)for(int axis=0;axis<2;++axis)teeth[i][axis]=1.14f*kVelaGearOutline[i][axis];
  iconPath(p,pen,x,y,teeth,48,true);ring(p,x,y,5,colour,2.65f);
 }
 GdipDeletePen(pen);
}
static void drawFrame(Painter&p){
 velaMark(p,36,29,20);text(g.memoryDC,g.fontSmall,L"V E L A",55,8,110,42,C_WHITE);
 line(p,84,77,84,g.h-28,0x253B434F,1);
 for(int i=0;i<tabCount;++i)sectionIcon(p,kNavTabs[i],42,railY(i),g.tab==kNavTabs[i]?tint():0xFFBFC7D3);
 line(p,g.w-99,29,g.w-81,29,C_WHITE,1.2f);
 line(p,g.w-41,21,g.w-26,36,C_WHITE,1.4f);
 line(p,g.w-26,21,g.w-41,36,C_WHITE,1.4f);
}
// While captured, preserve the expensive static glass and library in one device bitmap.
static HDC dragBase=nullptr;
static HBITMAP dragBitmap=nullptr;
static HGDIOBJ dragOriginal=nullptr;
static bool buildingDragBase=false,dragBaseReady=false;
static void releaseDragBase(){
 if(dragBase){SelectObject(dragBase,dragOriginal);DeleteDC(dragBase);}
 if(dragBitmap)DeleteObject(dragBitmap);
 dragBase=nullptr;dragBitmap=nullptr;dragBaseReady=false;
}
DLL HBITMAP API CreateCompatibleBitmap(HDC,int,int);
static bool allocateDragBase(){
 if(dragBase)return true;
 dragBase=CreateCompatibleDC(g.memoryDC);
 dragBitmap=CreateCompatibleBitmap(g.memoryDC,uiPixels(g.w),uiPixels(g.h));
 if(!dragBase||!dragBitmap){releaseDragBase();return false;}
 dragOriginal=SelectObject(dragBase,dragBitmap);return true;
}
static void crosshairLive(Painter& p,HDC dc){
 U32 c=ARGB((U8)(g.opacity*255/100),(U8)((g.color>>16)&255),(U8)((g.color>>8)&255),(U8)(g.color&255));
 if((g.preset==12||g.preset>=61)&&g.importedReady){
  int biggest=clamp(g.size*3,6,144),wide=imax(1,biggest*g.importCacheW/imax(1,imax(g.importCacheW,g.importCacheH)));
  int tall=imax(1,biggest*g.importCacheH/imax(1,imax(g.importCacheW,g.importCacheH)));
  GdipDrawImageRectI(p.gfx,g.importedImage,360-wide/2,353-tall/2,wide,tall);
 }else drawCross(p,g.preset,360,353,g.size,g.thickness,g.gap,c,false);
 // Preserve Gap while making all four adjustments compact, thin and quiet.
 int sx=664,sw=383;
 slider(p,dc,L"Size",g.size,1,48,sx,253,sw);
 slider(p,dc,L"Thickness",g.thickness,1,12,sx,318,sw);
 slider(p,dc,L"Gap",g.gap,0,24,sx,383,sw);
 slider(p,dc,L"Opacity",g.opacity,10,100,sx,448,sw);
}
static void drawCrosshairTab(Painter&p,HDC dc){
 text(dc,g.fontTitle,L"Crosshair",115,80,600,49,C_WHITE);
 text(dc,g.fontSmall,L"Create and customise your crosshair",115,130,600,26,C_DIM);
 panel(p,111,180,498,347);
 panel(p,635,153,450,432);
 panel(p,111,546,498,87);
 const int total=totalCrosshairPages();
 for(int i=0;i<5;i++){
  int id=crosshairAt(g.crosshairPage,i);
  if(id<0)continue;
  int tx=653+i*85,ty=171;
  addArea(tx,ty,77,47,10);
  U32 surface=id==g.preset?0xFF222B25:(i==g.hoverTile?C_TILE_HOVER:C_TILE);
  glass(p,tx,ty,77,47,10,id==g.preset?tint(93):0xFF22262C,id==g.preset?tint(54):0xFF13161B);
  if(id==g.preset)strokedRoundRect(p,tx+1,ty+1,75,45,10,tint(),1.2f);
  if(id>=61){
   int index=id-61;
   if(index<libraryCount&&libraryPNGs[index].ready){
    int iw=(int)libraryPNGs[index].width,ih=(int)libraryPNGs[index].height;
    int sw=imax(1,iw*48/imax(iw,ih)),sh=imax(1,ih*48/imax(iw,ih));
    GdipDrawImageRectI(p.gfx,libraryPNGs[index].image,tx+38-sw/2,ty+23-sh/2,sw,sh);
   }
   // Small neutral image badge identifies imported PNGs without large labels.
   strokedRoundRect(p,tx+58,ty+4,12,11,2,0xFF949CA0,1.0f);
   circle(p,tx+62,ty+8,1,0xFFC2C8CA);
  }else drawCross(p,id,tx+38,ty+23,20,3,4,id==g.preset?tint():0xFFF3F4F5,true);
 }
 // Pagination remains usable with all 60 built-ins and every imported PNG.
 text(dc,g.fontSmall,L"<",655,219,34,29,C_DIM,DT_CENTER);
 numText(dc,g.fontTiny,g.crosshairPage+1,693,219,23,29,C_WHITE,DT_CENTER);
 text(dc,g.fontTiny,L"/",715,219,18,29,C_DIM,DT_CENTER);
 numText(dc,g.fontTiny,total,733,219,27,29,C_DIM,DT_CENTER);
 text(dc,g.fontSmall,L">",761,219,34,29,C_DIM,DT_CENTER);
 addArea(956,219,108,30,10);text(dc,g.fontSmall,L"+ Import",956,219,108,30,C_DIM,DT_CENTER);
 if(!buildingDragBase)crosshairLive(p,dc);
 for(int i=0;i<7;i++){
  int cx=673+i*61,cy=548;
  circle(p,cx,cy,10,i==6?(0xFF000000|g.color):swatches[i]);
  if(i==6){ring(p,cx,cy,12,0xFF8B9295,1.0f);line(p,cx-4,cy,cx+4,cy,C_WHITE,1.0f);line(p,cx,cy-4,cx,cy+4,C_WHITE,1.0f);}
  if(g.swatch==i||(i==6&&g.customPicker))ring(p,cx,cy,14,0xFFE6E8EA,1.1f);
 }
 text(dc,g.fontSmall,L"In-game overlay",133,553,225,36,C_WHITE);
 toggle(p,529,562,56,g.overlayOn,switches[0]);
 text(dc,g.fontSmall,g.overlayOn?L"Status: Active":L"Status: Off",133,588,176,28,g.overlayOn?0xFFABECAA:C_WHITE);
 text(dc,g.fontTiny,L"F1 to toggle",478,594,106,28,C_WHITE,DT_CENTER);
}
static void drawPicker(Painter& p,HDC dc){
  roundRect(p,551,274,243,248,11,0xFF222428);
  strokedRoundRect(p,551,274,243,248,11,0xFF52565B,1);
  text(dc,g.fontSection,L"Custom colour",566,285,210,29,C_WHITE);
  static int lastHue=-1;
  if(lastHue!=g.pickerHue){lastHue=g.pickerHue;for(int py=0;py<124;py++)for(int px=0;px<200;px++)pickerField[py*200+px]=hsvToRGB(g.pickerHue,px*100/199,(123-py)*100/123);}
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
  text(dc,g.fontTiny,L"Drag to pick  -  Esc to close",566,501,208,20,C_DIM);
}

DLL HWND API GetFocus();

static void drawResolutionTab(Painter&p,HDC dc){
 text(dc,g.fontTitle,L"Display",115,77,400,56,C_WHITE);
 const wchar_t* groups[]={L"Classic",L"Stretched",L"More modes"};
 for(int i=0;i<3;++i)button(p,dc,115+100*i,135,94,30,groups[i],displayPage==i);
 panel(p,624,73,273,87);panel(p,910,73,159,87);
 text(dc,g.fontTiny,L"RESOLUTION",643,79,180,25,C_DIM);
 numText(dc,g.fontTitle,g.desktopW,639,103,80,47,C_WHITE,DT_CENTER);
 text(dc,g.fontSmall,L"x",722,109,19,32,C_DIM,DT_CENTER);
 numText(dc,g.fontTitle,g.desktopH,743,103,83,47,C_WHITE,DT_CENTER);
 text(dc,g.fontTiny,L"REFRESH RATE",928,79,125,25,C_DIM);
 numText(dc,g.fontTitle,g.desktopHz,926,103,65,47,C_WHITE,DT_CENTER);
 text(dc,g.fontBody,L"Hz",995,112,46,30,C_DIM);
 panel(p,99,173,986,316); panel(p,99,504,986,119);
 for(int i=0;i<12;i++){
  int id=modePages[displayPage][i];const ResChoice& r=getResChoice(id);
  int x=115+(i%3)*322,y=188+(i/3)*75;
  addArea(x,y,305,65,14);bool active=g.selectedMode==id;
  glass(p,x,y,305,65,14,active?tint(108,201):0x63535A60,active?tint(78,153):0x8024292F);
  if(active)strokedRoundRect(p,x+1,y+1,303,63,9,tint(215),1.0f);
  text(dc,g.fontSection,r.name,x+18,y+7,260,34,C_WHITE);
  if(id==0)text(dc,g.fontTiny,L"Your monitor's preferred mode",x+18,y+37,269,20,C_DIM);
  else text(dc,g.fontTiny,r.note,x+18,y+37,264,20,C_DIM);
 }
 line(p,115,503,1069,503,0xFF34373B,1);
 text(dc,g.fontSection,g.selectedMode==15?L"Custom":L"Selected",115,514,147,35,C_WHITE);
 if(g.selectedMode!=15)text(dc,g.fontSmall,getResChoice(g.selectedMode).name,246,514,280,35,C_DIM);
 for(int field=0;field<2;++field){
  int x=536+field*210;
  HWND editor=field?g.editHeight:g.editWidth;
  roundRect(p,x,513,188,46,12,0xFF171B21);
  strokedRoundRect(p,x,513,188,46,12,GetFocus()==editor?tint():0xFF343B46,1.2f);
  text(dc,g.fontTiny,field?L"Height":L"Width",x+13,513,53,46,C_DIM);
  text(dc,g.fontTiny,L"px",x+158,513,23,46,C_DIM);
 }
 text(dc,g.fontSmall,L"\u00D7",724,513,22,46,C_DIM,DT_CENTER);
 button(p,dc,115,568,216,41,L"Apply resolution",true);
 button(p,dc,345,568,216,41,L"Reset to native");
 if(g.keepPending){
  text(dc,g.fontSection,g.remaining?L"Keep this display mode?":L"Retrying display recovery...",599,562,390,28,C_WHITE);
  void* pen=nullptr;
  ring(p,1034,572,16,0xFF36403D,2);
  if(GdipCreatePen1(0xFF63D19A,2,2,&pen)==0){
   float remaining=fclamp(1-(GetTickCount()-g.modeTimestamp)/15000.0f,0,1);
   GdipDrawArcI(p.gfx,pen,1018,556,32,32,-90,360*remaining);GdipDeletePen(pen);
  }
  numText(dc,g.fontSmall,g.remaining,1016,557,36,31,C_WHITE,DT_CENTER);
  button(p,dc,601,591,207,29,customTrial()?L"Keep and save":L"Keep",true);
  button(p,dc,826,591,226,29,L"Revert");
 }else if(g.status[0])text(dc,g.fontTiny,g.status,578,580,491,31,C_DIM);
 else text(dc,g.fontTiny,L"15-second recovery timer on every change",593,580,477,31,C_DIM);
 text(dc,g.fontSection,L"High refresh mode",115,630,215,42,C_WHITE);
 text(dc,g.fontSmall,L"Fastest supported rate at your current resolution",333,630,488,42,C_DIM);
 button(p,dc,860,631,207,39,L"Use highest Hz");

}
DLL int API GdipGetImageGraphicsContext(void*,void**);
static void* previewCrop=nullptr;
// Resample the original scene once. Subsequent frames only colour the adjusted half.
static void* visibilityScene(){
 if(previewCrop||!g.previewImage)return previewCrop;
 UINT w=0,h=0;GdipGetImageWidth(g.previewImage,&w);GdipGetImageHeight(g.previewImage,&h);
 int cw=imin(w,(int)h*429/188),ch=cw*188/429;
 if(cw<=1||ch<=0)return nullptr;
 void* image=nullptr;void* gfx=nullptr;
 if(GdipCreateBitmapFromScan0(429,188,0,0xE200B,nullptr,&image))return nullptr;
 if(GdipGetImageGraphicsContext(image,&gfx)){GdipDisposeImage(image);return nullptr;}
 int status=GdipDrawImageRectRectI(gfx,g.previewImage,0,0,429,188,((int)w-cw)/2,((int)h-ch)/2,cw,ch,2,nullptr,nullptr,nullptr);
 GdipDeleteGraphics(gfx);
 if(status){GdipDisposeImage(image);return nullptr;}
 return previewCrop=image;
}
constexpr int compareX=619,compareY=176,compareW=429,compareH=188;
static int compareSplit=compareW/2;
static bool scrubbing=false;
static void moveComparison(int x){
 int position=clamp(x-compareX,0,compareW);
 if(position==compareW-1)position=compareW;
 if(position!=compareSplit){compareSplit=position;invalidate();}
}
static void visibilityLive(Painter& p,HDC dc){
 void* image=visibilityScene();
 if(image){
  constexpr int x=compareX,y=compareY,pw=compareW,ph=compareH;
  void* attributes=nullptr;float matrix[25]={};buildColourMatrix(matrix);
  if(!GdipCreateImageAttributes(&attributes)){
   void* mask=nullptr;rrPath(&mask,x,y,pw,ph,13);
   if(mask)GdipSetClipPath(p.gfx,mask,0);
   GdipSetImageAttributesColorMatrix(attributes,1,1,matrix,nullptr,0);
   int left=compareSplit,right=pw-left;
   if(left)GdipDrawImageRectRectI(p.gfx,image,x,y,left,ph,0,0,left,ph,2,nullptr,nullptr,nullptr);
   if(right)GdipDrawImageRectRectI(p.gfx,image,x+left,y,right,ph,left,0,right,ph,2,attributes,nullptr,nullptr);
   GdipDisposeImageAttributes(attributes);
   int mid=y+ph/2,handle=x+clamp(left,12,pw-12);
   if(left&&right)line(p,x+left,y,x+left,y+ph,0xDDF8F5EF,1.5f);
   circle(p,handle,mid,12,0xE8444342);ring(p,handle,mid,12,0xDAF4EFE6,1);
   for(int side=-1;side<=1;side+=2){
    line(p,handle+side*3,mid-3,handle+side*6,mid,C_WHITE,1);
    line(p,handle+side*6,mid,handle+side*3,mid+3,C_WHITE,1);
   }
   if(mask){GdipResetClip(p.gfx);GdipDeletePath(mask);}
   if(left>=82){
    roundRect(p,x+9,y+ph-31,65,22,11,0xCA171A1D);
    text(dc,g.fontTiny,L"Original",x+9,y+ph-31,65,22,C_WHITE,DT_CENTER);
   }
   if(right>=90){
    roundRect(p,x+pw-82,y+ph-31,73,22,11,0xCA171A1D);
    text(dc,g.fontTiny,L"Adjusted",x+pw-82,y+ph-31,73,22,C_WHITE,DT_CENTER);
   }
  }
 }
 const wchar_t* labels[]={L"Digital vibrance",L"Midtones",L"Shadow detail",L"Brightness",L"Contrast"};
 int values[]={g.saturation,g.gamma,g.shadows,g.brightness,g.contrast};
 const int low[]={0,75,0,70,70},high[]={200,160,100,130,130};
 for(int i=0;i<5;++i)slider(p,dc,labels[i],values[i],low[i],high[i],visX,visY(i),visWidth);
}
static void drawColorTab(Painter&p,HDC dc){
 text(dc,g.fontTitle,L"Visibility",115,83,660,50,C_WHITE);
 panel(p,107,155,464,410);
 panel(p,599,155,469,410);
 glass(p,modeX-4,modeY-4,segmentWidth*3+8,44,22,0x683C4249,0x9623262B);
 const wchar_t* names[]={L"Original",L"Competitive",L"Night boost"};
 if(g.colorPreset>=0){
  int x=modeX+(int)animate(modeMotion,(float)(g.colorPreset*segmentWidth));
  glass(p,x,modeY,segmentWidth,36,18,0xC5677075,0xCD3D454B);
 }
 for(int i=0;i<3;++i)addArea(modeX+i*segmentWidth,modeY,segmentWidth,36,18);
 for(int i=0;i<3;++i)text(dc,g.fontSmall,names[i],modeX+i*segmentWidth,modeY,segmentWidth,36,
  i==g.colorPreset?C_WHITE:0xFFC7CDD0,DT_CENTER);
 if(!buildingDragBase)visibilityLive(p,dc);
 text(dc,g.fontBody,L"Desktop colours",626,384,300,32,C_WHITE);
 toggle(p,993,387,48,g.desktopColorOn,switches[2]);
 line(p,627,428,1040,428,0x25F0F5F8,1);
 text(dc,g.fontBody,L"Game focus",626,440,300,32,C_WHITE);
 toggle(p,993,443,48,g.gameFocusOnly,switches[1]);
 text(dc,g.fontSmall,g.gameProcess[0]?g.gameProcess:L"No linked game",627,498,248,32,C_DIM);
 button(p,dc,893,494,147,38,L"Bind game  (5s)");
 const wchar_t* status=g.gameBindPending?L"Switch to your game now":g.status[0]?g.status:
  g.gameFocusOnly?(g.gameFocusActive?L"Game in focus":L"Waiting for linked game"):L"";
 text(dc,g.fontSmall,status,109,582,960,30,C_DIM);
}
static bool quietVela=true,animateWallpaper=true;
static MotionValue performanceSwitches[2];
struct MemoryReading{DWORD bytes,load;U64 amounts[7];};
DLL BOOL API GlobalMemoryStatusEx(MemoryReading*);
DLL BOOL API GetSystemTimes(U64*,U64*,U64*);
extern "C" void initGpuMeter(const wchar_t*);
extern "C" int sampleGpuMeter(U64*,U64*);
DLL DWORD API GetActiveProcessorCount(WORD);
static int cpuLoad=-1,ramLoad=-1,gpuLoad=-1;
static U64 ramUsed,ramTotal,videoUsed,videoTotal;
static DWORD logicalProcessors;
static void sampleSystem(){
 static U64 lastIdle,lastTotal;U64 idle=0,kernel=0,user=0;
 if(GetSystemTimes(&idle,&kernel,&user)){
  U64 total=kernel+user,dt=total-lastTotal;
  if(lastTotal&&dt&&idle-lastIdle<=dt)cpuLoad=(int)(100*(dt-(idle-lastIdle))/dt);
  lastTotal=total;lastIdle=idle;
 }
 MemoryReading m={};m.bytes=sizeof(m);if(GlobalMemoryStatusEx(&m)){
  ramLoad=(int)m.load;ramTotal=m.amounts[0];ramUsed=ramTotal-m.amounts[1];
 }
 gpuLoad=sampleGpuMeter(&videoUsed,&videoTotal);
}
static void meter(Painter& p,HDC dc,int cx,int y,int value,const wchar_t* label){
 ring(p,cx,y,33,0xFF2C3139,4);
 void* pen=nullptr;
 if(value>0&&!GdipCreatePen1(tint(),4,2,&pen)){
  GdipDrawArcI(p.gfx,pen,cx-33,y-33,66,66,-90,value*3.6f);GdipDeletePen(pen);
 }
 text(dc,g.fontTiny,label,cx-32,y-19,64,21,C_DIM,DT_CENTER);
 if(value>=0){wchar_t v[6]={};int n=0;if(value>=100)v[n++]=L'1';if(value>=10)v[n++]=L'0'+(value/10)%10;v[n++]=L'0'+value%10;v[n++]=L'%';text(dc,g.fontSection,v,cx-31,y,62,25,C_WHITE,DT_CENTER);}
 else text(dc,g.fontTiny,L"...",cx-31,y,62,24,C_DIM,DT_CENTER);
}
static void performanceRow(Painter& p,HDC dc,int y,const wchar_t* name,const wchar_t* detail,const wchar_t* action){
 text(dc,g.fontSection,name,125,y,640,27,C_WHITE);
 text(dc,g.fontSmall,detail,125,y+29,action?426:706,24,C_DIM);
 if(action)button(p,dc,558,y+6,154,36,action);
}
#include "DriverProfile.h"
static bool tuningPage=false;
static const int caps[]={-1,0,60,120,144,165,240};
static const wchar_t* capNames[]={L"Default",L"Off",L"60",L"120",L"144",L"165",L"240"};
static const wchar_t* powerNames[]={L"Driver default",L"Prefer maximum performance"};
static void drawGameTuning(Painter& p,HDC dc){
 panel(p,99,155,986,102);
 text(dc,g.fontSection,gameExe[0]?gameExe:L"Choose a game executable",122,167,678,34,C_WHITE);
 text(dc,g.fontSmall,profileLabel[0]?profileLabel:L"NVIDIA game profile",122,207,665,28,C_DIM);
 button(p,dc,867,183,190,39,gameExe[0]?L"Change game":L"Choose game");
 panel(p,99,274,986,154);
 text(dc,g.fontSection,L"Frame rate limit",122,289,420,30,C_WHITE);
 text(dc,g.fontSmall,L"Maximum frames per second for this game.",122,325,790,26,C_DIM);
 for(int i=0;i<7;++i)button(p,dc,122+132*i,367,117,37,capNames[i],profileReady&&overrides[0]==caps[i]);
 panel(p,99,445,986,154);
 text(dc,g.fontSection,L"GPU power mode",122,461,420,30,C_WHITE);
 text(dc,g.fontSmall,L"Prefer higher GPU clocks during play. This can increase power use and heat.",122,497,925,25,C_DIM);
 button(p,dc,122,539,180,37,powerNames[0],profileReady&&overrides[1]==-1);
 button(p,dc,318,539,290,37,powerNames[1],profileReady&&overrides[1]==1);
 text(dc,g.fontSmall,g.status,122,618,940,40,C_DIM);
}
static void tuningClick(int x,int y){
 if(hit(x,y,867,183,190,39)){
  wchar_t file[512]={};OPENFILENAMEW dialog={};dialog.lStructSize=sizeof(dialog);dialog.hwndOwner=g.hwnd;
  dialog.lpstrFile=file;dialog.nMaxFile=512;dialog.lpstrFilter=L"Game executables (*.exe)\0*.exe\0";dialog.Flags=0x1808;
  if(!GetOpenFileNameW(&dialog))return;
  const wchar_t* name=file;for(const wchar_t* it=file;*it;++it)if(*it==L'\\')name=it+1;
  wcopy(gameExe,name,512);profileLabel[0]=0;
  const wchar_t* error=profileAction();setStatus(error?error:L"Changes apply to the selected NVIDIA profile.");return;
 }
 int key=-1,value=-1;
 for(int i=0;i<7;++i)if(hit(x,y,122+132*i,367,117,37)){key=0;value=caps[i];}
 if(hit(x,y,122,539,180,37))key=1;
 if(hit(x,y,318,539,290,37)){key=1;value=1;}
 if(key<0)return;
 if(!gameExe[0]){setStatus(L"Choose a game executable first.");return;}
 const wchar_t* error=profileAction(key,value);
 setStatus(error?error:L"Saved in the game profile. Relaunch the game to use the setting.");
}
static void drawPerformanceTab(Painter& p,HDC dc){
 text(dc,g.fontTitle,L"Performance",115,73,700,55,C_WHITE);
 button(p,dc,690,84,170,38,L"System",!tuningPage);
 button(p,dc,874,84,194,38,L"Game tuning",tuningPage);
 if(tuningPage){drawGameTuning(p,dc);return;}
 panel(p,99,155,636,285);panel(p,752,155,333,285);
 text(dc,g.fontSection,L"System",773,175,262,29,C_WHITE);
 meter(p,dc,815,275,cpuLoad,L"CPU");meter(p,dc,919,275,gpuLoad,L"GPU");meter(p,dc,1023,275,ramLoad,L"RAM");
 text(dc,g.fontSmall,L"Display refresh",777,362,167,32,C_DIM);numText(dc,g.fontSection,g.desktopHz,941,362,64,32,C_WHITE);text(dc,g.fontSmall,L"Hz",1013,362,42,32,C_DIM);
 performanceRow(p,dc,157,L"Display latency",L"Try the fastest rate at this resolution.",L"Try highest Hz");
 performanceRow(p,dc,228,L"Lower GPU workload",L"Reduce pixels for games using desktop resolution.",L"Try lower resolution");
 performanceRow(p,dc,299,L"Frame rate limit",L"Set a per-game FPS cap in the NVIDIA driver.",L"Set FPS limit");
 performanceRow(p,dc,370,L"GPU power mode",L"Choose how your GPU manages clocks in a game.",L"Set power mode");
 panel(p,99,457,986,147);
 performanceRow(p,dc,472,L"Low-overhead Vela",L"Reduce wallpaper redraws and lower Vela's CPU priority when you switch away.",nullptr);
 toggle(p,1002,484,51,quietVela,performanceSwitches[0]);
 performanceRow(p,dc,539,L"Animated wallpaper",L"Turn off background motion for an idle dashboard.",nullptr);
 toggle(p,1002,550,51,animateWallpaper,performanceSwitches[1]);
 if(g.keepPending){
  text(dc,g.fontSection,L"Keep this display mode?",122,622,270,36,C_WHITE);
  numText(dc,g.fontSection,g.remaining,398,622,60,36,C_WHITE,DT_CENTER);
  button(p,dc,692,622,166,38,customTrial()?L"Keep and save":L"Keep",true);button(p,dc,879,622,176,38,L"Revert");
 }else text(dc,g.fontSmall,g.status,123,617,928,48,C_WHITE);
}
DLL DWORD API GetPrivateProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,DWORD,LPCWSTR);
DLL BOOL API WritePrivateProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR);
static wchar_t appearanceIni[512];
static void loadAppearance(){
 wcopy(appearanceIni,g.profileDirectory,512);wappend(appearanceIni,512,L"\\Appearance.ini");
 wchar_t name[32]={};
 GetPrivateProfileStringW(L"Appearance",L"Accent",L"Ice",name,32,appearanceIni);
 for(int i=0;i<accentCount;++i){
  int j=0;while(name[j]&&name[j]==accents[i].name[j])++j;
  if(name[j]==accents[i].name[j]){accentIndex=i;break;}
 }
}
struct AdapterInfo{DWORD size;wchar_t name[32],description[128];DWORD flags;wchar_t id[128],key[128];};
DLL BOOL API EnumDisplayDevicesW(LPCWSTR,DWORD,AdapterInfo*,DWORD);
extern "C" void __cpuid(int*,int);
#pragma intrinsic(__cpuid)
static wchar_t cpuModel[64]=L"Processor",gpuModel[128]=L"Display adapter";
static void readHardware(){
 logicalProcessors=GetActiveProcessorCount(0xffff);
 int regs[4];__cpuid(regs,0x80000000);
 if((U32)regs[0]>=0x80000004){
  char name[49]={};for(int leaf=0;leaf<3;++leaf){
   __cpuid(regs,(int)(0x80000002u+leaf));
   for(int j=0;j<16;++j)name[16*leaf+j]=((char*)regs)[j];
  }
  int start=0;while(name[start]==' ')++start;
  if(name[start]){int j=0;for(;name[start+j];++j)cpuModel[j]=(BYTE)name[start+j];cpuModel[j]=0;}
 }
 for(DWORD i=0;;++i){
  AdapterInfo info={};info.size=sizeof(info);if(!EnumDisplayDevicesW(nullptr,i,&info,0))break;
  if(info.flags&4){wcopy(gpuModel,info.description,128);initGpuMeter(info.name);break;}
 }
}
static void appendNumber(wchar_t* out,int capacity,U32 number){
 wchar_t digits[12]={};int i=11;
 do{digits[--i]=L'0'+number%10;number/=10;}while(number);
 wappend(out,capacity,digits+i);
}
static void memoryDetail(wchar_t* out,U64 used,U64 total,const wchar_t* unit){
 if(!total)return;
 for(int i=0;i<2;++i){
  U64 tenths=((i?total:used)+50000000ull)/100000000ull;
  appendNumber(out,64,(U32)(tenths/10));wappend(out,64,L".");appendNumber(out,64,(U32)(tenths%10));
  if(!i)wappend(out,64,L" / ");
 }
 wappend(out,64,unit);
}
static MotionValue homeLevels[3];
static void homeUsage(Painter& p,int x,int value,MotionValue& level){
 int width=206;
 roundRect(p,x,553,width,5,2,0xFF333942);
 if(value<0){text(g.memoryDC,g.fontBody,L"Unavailable",x,511,width,34,C_WHITE);return;}
 float shown=animate(level,(float)value,280);
 int fill=(int)(width*shown/100);
 if(fill>0)roundRect(p,x,553,fill,5,2,tint());
 wchar_t percent[6]={};int n=0;
 if(value==100)percent[n++]=L'1';if(value>=10)percent[n++]=L'0'+value/10%10;
 percent[n++]=L'0'+value%10;percent[n]=L'%';
 text(g.memoryDC,g.fontSection,percent,x,511,width,34,C_WHITE);
}
#include "GameLibrary.h"
static GameList* games=nullptr;
static bool scanningGames=false;
static int libraryPage=0;
static void refreshLibrary(){
 if(scanningGames)return;
 scanningGames=scanGames(g.hwnd);
 if(!scanningGames)setStatus(L"Could not start the game scan.");
 invalidate();
}
static void drawLibrary(Painter& p,HDC dc){
 text(dc,g.fontTitle,L"Game library",114,74,500,58,C_WHITE);
 button(p,dc,762,85,126,38,L"Refresh");
 button(p,dc,902,85,166,38,L"+ Add game",true);
 const int count=games?games->count:0;
 if(!count){
  panel(p,111,164,957,263);
  text(dc,g.fontSection,scanningGames?L"Finding installed games...":L"Add your first game",145,232,890,40,C_WHITE,DT_CENTER);
  text(dc,g.fontSmall,L"Steam and Epic games appear here. You can also add a game executable or shortcut.",142,283,895,36,C_DIM,DT_CENTER);
 }else{
  for(int i=0;i<6&&libraryPage*6+i<count;++i){
   const LibraryGame& game=games->games[libraryPage*6+i];int y=151+i*73;
   panel(p,111,y,957,65);
   text(dc,g.fontSection,game.name,132,y+4,660,32,C_WHITE);
   text(dc,g.fontTiny,game.source==1?L"Steam":game.source==2?L"Epic Games":L"Added by you",133,y+32,620,24,C_DIM);
   if(game.source==1)button(p,dc,713,y+14,190,36,L"Use current account");
   if(!game.source)button(p,dc,800,y+14,103,36,L"Remove");
   button(p,dc,917,y+14,126,36,L"Launch",true);
  }
 }
 if(count>6){
  button(p,dc,788,598,114,36,L"Previous");button(p,dc,913,598,132,36,L"Next");
  wchar_t page[64]=L"Page ";appendNumber(page,64,libraryPage+1);wappend(page,64,L" of ");appendNumber(page,64,(count+5)/6);
  text(dc,g.fontSmall,page,121,598,220,36,C_DIM);
 }
 text(dc,g.fontSmall,g.status,121,643,930,30,C_DIM);
}
static void libraryClick(int x,int y){
 if(hit(x,y,762,85,126,38)){refreshLibrary();return;}
 if(hit(x,y,902,85,166,38)){
  if(scanningGames){setStatus(L"Finishing the game scan. Try again in a moment.");return;}
  wchar_t path[1024]={};OPENFILENAMEW dlg={};dlg.lStructSize=sizeof(dlg);dlg.hwndOwner=g.hwnd;
  dlg.lpstrTitle=L"Add a game or launcher shortcut";dlg.lpstrFile=path;dlg.nMaxFile=1024;
  dlg.lpstrFilter=L"Games and shortcuts (*.exe;*.lnk;*.url)\0*.exe;*.lnk;*.url\0";dlg.Flags=0x101808;
  if(GetOpenFileNameW(&dlg)){
   if(!storeGame(path,false)){setStatus(L"Could not save this game. Choose an EXE or launcher shortcut.");return;}
   setStatus(L"Game added to your library.");refreshLibrary();
  }
  return;
 }
 if(!games)return;
 if(hit(x,y,788,598,114,36)||hit(x,y,913,598,132,36)){
  libraryPage=clamp(libraryPage+(x>=913?1:-1),0,imax(0,(games->count-1)/6));invalidate();return;
 }
 for(int i=0;i<6&&libraryPage*6+i<games->count;++i){
  const LibraryGame& game=games->games[libraryPage*6+i];int top=165+i*73;
  if(hit(x,y,917,top,126,36)){
   setStatus(requestLibraryLaunch(&game,g.hwnd));return;
  }
  if(game.source==1&&hit(x,y,713,top,190,36)){setStatus(assignActiveAccount(&game)?L"Current Steam account saved for this game.":L"Sign in to Steam first, then try again.");return;}
  if(!game.source&&hit(x,y,800,top,103,36)&&!scanningGames){
   if(!storeGame(game.target,true)){setStatus(L"Could not remove the shortcut.");return;}
   setStatus(L"Shortcut removed.");refreshLibrary();return;
  }
 }
}
static void drawHome(Painter& p,HDC dc){
 text(dc,g.fontBody,L"Welcome back",115,82,500,30,C_DIM);
 text(dc,g.fontSplash,L"V E L A",112,117,550,66,C_WHITE);
 const wchar_t* names[]={L"Crosshair",L"Display",L"Performance",L"Tweaks"};
 int destinations[]={0,1,4,2};
 for(int i=0;i<4;++i){
  int x=111+i*246;panel(p,x,213,228,136);if(i==0)strokedRoundRect(p,x,213,228,136,18,tint(248,223),1);addArea(x,213,228,136,14);
  sectionIcon(p,destinations[i],x+114,256,i?C_WHITE:tint());
  text(dc,g.fontSection,names[i],x+15,294,198,32,C_WHITE,DT_CENTER);
 }
 button(p,dc,892,128,185,40,L"Game library");
 text(dc,g.fontSection,L"System info",115,370,400,32,C_WHITE);
 panel(p,111,415,966,172);
 text(dc,g.fontTiny,L"Processor",133,429,450,23,C_DIM);
 text(dc,g.fontSection,cpuModel,133,454,450,30,C_WHITE);
 text(dc,g.fontTiny,L"Graphics",620,429,420,23,C_DIM);
 text(dc,g.fontSection,gpuModel,620,454,430,30,C_WHITE);
 text(dc,g.fontTiny,L"CPU usage",133,492,220,22,C_DIM);
 text(dc,g.fontTiny,L"GPU usage",365,492,220,22,C_DIM);
 text(dc,g.fontTiny,L"Memory usage",597,492,220,22,C_DIM);
 text(dc,g.fontTiny,L"Display refresh",848,492,240,22,C_DIM);
 numText(dc,g.fontSection,g.desktopHz,848,511,32,34,C_WHITE,DT_LEFT);
 text(dc,g.fontSmall,L"Hz",884,511,40,34,C_DIM);
 text(dc,g.fontSmall,g.status,115,602,920,35,C_DIM);
}
static void drawSettingsTab(Painter&p,HDC dc){
 text(dc,g.fontTitle,L"Settings",115,83,700,51,C_WHITE);
 panel(p,99,160,986,329);panel(p,99,501,986,121);

 text(dc,g.fontSection,L"Your presets",115,187,295,27,C_WHITE);
 for(int i=0;i<4;i++){
  int x=115+(i%2)*486,y=223+(i/2)*72;
  addArea(x,y,469,61,14);
  U32 tile=i==g.activeProfile?0xFF2A2D2F:C_TILE;
  glass(p,x,y,469,61,14,i==g.activeProfile?tint(108,201):0x65535B62,i==g.activeProfile?tint(78,153):0x8824292E);
  if(i==g.activeProfile)strokedRoundRect(p,x+1,y+1,467,59,9,tint(215),1.0f);
  U32 dot=g.profileExists[i]?0xFF47C782:0xFF5B6065;
  circle(p,x+21,y+30,5,dot);
  const wchar_t* name=g.profileExists[i]?g.storedProfiles[i].name:defaultProfileNames[i];
  text(dc,g.fontSection,name,x+41,y+8,405,27,C_WHITE);
  text(dc,g.fontTiny,g.profileExists[i]?L"Saved profile":L"Empty slot",x+42,y+34,330,20,C_DIM);
 }
 text(dc,g.fontSmall,L"Preset name",115,379,128,27,C_DIM);
 roundRect(p,241,374,332,36,8,0xFF384255);
 strokedRoundRect(p,241,374,332,36,8,0x88758AA2,1);
 button(p,dc,115,432,219,43,L"Save Preset",true);
 button(p,dc,346,432,219,43,L"Load Preset");
 button(p,dc,578,432,219,43,L"Delete");
 button(p,dc,810,432,259,43,L"Reset Session");
 line(p,115,499,1069,499,C_LINE,1);
 text(dc,g.fontSection,L"Minimize to tray",115,509,320,32,C_WHITE);
 toggle(p,983,515,57,g.minimizeToTray,switches[3]);
#ifdef VELA_LOCAL_PREVIEW
 button(p,dc,836,557,205,36,L"Vela releases");
#endif
 text(dc,g.fontTiny,g.updateStatus,115,583,695,20,0xFF80AD93);
 panel(p,99,630,986,48);
 text(dc,g.fontSection,L"Accent colour",115,634,225,38,C_WHITE);
 for(int i=0;i<accentCount;++i){
  int x=500+i*113;
  addArea(x,635,108,38,12);
  if(accentIndex==i){
   roundRect(p,x,635,108,38,12,tint(125,64));
   strokedRoundRect(p,x,635,108,38,12,tint(200),1);
  }
  circle(p,x+17,654,5,0xFF000000|accents[i].rgb);
  text(dc,g.fontSmall,accents[i].name,x+32,635,73,38,C_WHITE);
 }
 if(g.status[0])text(dc,g.fontTiny,g.status,115,597,954,25,C_WHITE);
}
struct PageSurface{
 HDC dc;HBITMAP bitmap;HGDIOBJ original;U32 revision;
 void reset(){
  if(dc&&original)SelectObject(dc,original);
  if(bitmap)DeleteObject(bitmap);
  if(dc)DeleteDC(dc);dc=nullptr;bitmap=nullptr;original=nullptr;revision=0;
 }
 bool prepare(){
  if(dc)return true;
  dc=CreateCompatibleDC(g.memoryDC);
  if(dc)bitmap=CreateCompatibleBitmap(g.memoryDC,uiPixels(g.w),uiPixels(g.h));
  if(!bitmap){reset();return false;}
  original=SelectObject(dc,bitmap);return true;
 }
};
static PageSurface pageSurfaces[tabCount],backgroundSurface,decoration;
static int decoratedTab=-1;static U32 decoratedTime;
static bool restoreBackground(){
 if(!backgroundSurface.prepare())return false;
 if(!backgroundSurface.revision){
  void* gfx=nullptr;if(GdipCreateFromHDC(backgroundSurface.dc,&gfx))return false;
  GdipScaleWorldTransform(gfx,uiDpi/96.0f,uiDpi/96.0f,0);
  Painter p={gfx};backdrop(p);GdipDeleteGraphics(gfx);backgroundSurface.revision=1;
 }
 return BitBlt(g.memoryDC,0,0,uiPixels(g.w),uiPixels(g.h),backgroundSurface.dc,0,0,SRCCOPY)!=0;
}
static void clearSurfaces(){decoration.reset();backgroundSurface.reset();for(auto& surface:pageSurfaces)surface.reset();}
// Premultiplied soft light textures. Drawing these over cached pages avoids rebuilding the interface.
DLL int API SaveDC(HDC);
DLL BOOL API RestoreDC(HDC,int);
DLL int API ExcludeClipRect(HDC,int,int,int,int);
struct AmbientLight{
 HDC dc;HBITMAP bitmap;HGDIOBJ previous;
 bool prepare(int red,int green,int blue){
  if(dc)return true;
  constexpr int w=384,h=260;
  BITMAPINFO info={};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
  info.bmiHeader.biWidth=w;info.bmiHeader.biHeight=-h;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
  void* data=nullptr;bitmap=CreateDIBSection(g.memoryDC,&info,0,&data,nullptr,0);
  if(!bitmap)return false;
  dc=CreateCompatibleDC(g.memoryDC);if(!dc){DeleteObject(bitmap);bitmap=nullptr;return false;}
  previous=SelectObject(dc,bitmap);
  for(int y=0;y<h;++y)for(int x=0;x<w;++x){
   float dx=(2*x+1-w)/(float)w,dy=(2*y+1-h)/(float)h;
   float f=fclamp(1-dx*dx-dy*dy,0,1);int a=(int)(115*f*f*f);
   ((U32*)data)[y*w+x]=ARGB((U8)a,(U8)(red*a/255),(U8)(green*a/255),(U8)(blue*a/255));
  }
  return true;
 }
 void draw(int x,int y,int w,int h){
  BLEND_CONST blend={0,0,255,1};
  AlphaBlend(g.memoryDC,uiPixels(x),uiPixels(y),uiPixels(w),uiPixels(h),dc,0,0,384,260,blend);
 }
 void release(){if(dc){SelectObject(dc,previous);DeleteDC(dc);DeleteObject(bitmap);dc=nullptr;}}
};
static AmbientLight amberLight,peachLight;
static U32 ambientTime=0,ambientTick=0;
static bool appActive=false,ambientRunning=false;
DLL BOOL API SetPriorityClass(HANDLE,DWORD);
DLL DWORD API GetPriorityClass(HANDLE);
DLL HINSTANCE API ShellExecuteW(HWND,LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR,int);
static bool loweredPriority=false;
static void updateOwnPriority(){
 HANDLE self=(HANDLE)-1;
 if(quietVela&&!appActive){
  if(GetPriorityClass(self)==0x20&&SetPriorityClass(self,0x4000))loweredPriority=true;
 }else if(loweredPriority){
  if(GetPriorityClass(self)!=0x4000||SetPriorityClass(self,0x20))loweredPriority=false;
 }
}
static void openWindowsSetting(const wchar_t* uri){
 if((S64)ShellExecuteW(g.hwnd,L"open",uri,nullptr,nullptr,1)<=32)
  setStatus(L"Windows could not open this settings page.");
}
static void syncAmbient(bool visible){
 static bool sampling=false;
 bool wantSamples=g.hwnd&&visible&&appActive&&g.introPhase==3&&!g.trayShown&&!IsIconic(g.hwnd)&&(g.tab==4||g.tab==5);
 if(wantSamples!=sampling){
  if(wantSamples){sampleSystem();pageSurfaces[4].revision=0;sampling=SetTimer(g.hwnd,18,1200,nullptr)!=0;}
  else{KillTimer(g.hwnd,18);sampling=false;}
 }
 bool run=g.hwnd&&visible&&appActive&&animateWallpaper&&motionEnabled&&g.introPhase==3&&!g.trayShown&&!IsIconic(g.hwnd);
 if(run==ambientRunning)return;
 if(run){ambientTick=GetTickCount();ambientRunning=SetTimer(g.hwnd,14,quietVela?100:40,nullptr)!=0;}
 else{KillTimer(g.hwnd,14);ambientRunning=false;}
}
static float wave(U32 period,U32 offset){
 float phase=(float)((ambientTime%period+offset)%period)*64/period;
 int i=(int)phase;float fraction=phase-i;
 return kOrbX[i%64]+(kOrbX[(i+1)%64]-kOrbX[i%64])*fraction;
}
static void paintAmbient(){
 if(!motionEnabled||!animateWallpaper)return;
 U32 a=tint(89),b=tint(54);
 if(!amberLight.prepare((a>>16)&255,(a>>8)&255,a&255)||!peachLight.prepare((b>>16)&255,(b>>8)&255,b&255))return;
 int saved=SaveDC(g.memoryDC);if(!saved)return;
 // Preserve exact comparison pixels and the neutral crosshair studio.
 if(g.tab==0)ExcludeClipRect(g.memoryDC,uiPixels(111+sidebarExtra),uiPixels(180),uiPixels(609+sidebarExtra),uiPixels(527));
 if(g.tab==2)ExcludeClipRect(g.memoryDC,uiPixels(compareX+sidebarExtra),uiPixels(compareY),uiPixels(compareX+compareW+sidebarExtra),uiPixels(compareY+compareH));
 amberLight.draw(330+(int)(295*wave(20000,1000)),-130+(int)(146*wave(29000,4000)),850,575);
 peachLight.draw(-215+(int)(279*wave(27000,7000)),190+(int)(172*wave(23000,2000)),830,570);
 RestoreDC(g.memoryDC,saved);
}
static void paintFeedback(){
 void* gfx=nullptr;if(GdipCreateFromHDC(g.memoryDC,&gfx))return;
 GdipScaleWorldTransform(gfx,uiDpi/96.0f,uiDpi/96.0f,0);
 GdipSetSmoothingMode(gfx,4);Painter p={gfx};
 int row=0;while(row<tabCount-1&&kNavTabs[row]!=g.tab)++row;
 int selectedY=(int)animate(railPosition,(float)railY(row),135.0f);
 
 roundRect(p,16,selectedY-24,52,48,12,tint(190,64));roundRect(p,9,selectedY-10,3,20,2,tint());
 static MotionValue nav[tabCount],hover[tabCount][48];
 for(int i=0;i<tabCount;++i){
  float f=animate(nav[i],i==g.hoverNav?1.0f:0.0f);
  if(f>.01f)roundRect(p,16,railY(i)-24,52,48,12,tint(255,(int)(32*f)));
 }
 if(focusedTab>=0)strokedRoundRect(p,16,railY(focusedTab)-24,52,48,12,0xA4B1C9DE,1);
 if(g.hoverControl>=200)roundRect(p,g.hoverControl==200?g.w-118:g.w-61,7,55,43,6,0x388AA4B8);
 for(int i=0;i<areaCount[g.tab];++i){
  const HoverArea& a=areas[g.tab][i];float f=animate(hover[g.tab][i],hoveredArea==i?1.0f:0.0f);
  if(f>.01f)roundRect(p,a.x+sidebarExtra,a.y,a.w,a.h,a.r,tint(255,(int)(37*f)));
 }
 GdipDeleteGraphics(gfx);
}
static void render(HDC hdc){
 if(!g.memoryDC||!g.memoryBitmap)return;
 textCount=0;
 PageSurface& surface=pageSurfaces[g.tab];
 bool canCache=g.introPhase==3&&!g.mouseDown&&!scrubbing&&!g.pickerDrag&&surface.prepare();
 bool cached=canCache&&surface.revision==contentRevision;
 bool decorated=cached&&decoration.revision==contentRevision&&decoratedTab==g.tab&&decoratedTime==ambientTime;
 if(cached){
  BitBlt(g.memoryDC,0,0,uiPixels(g.w),uiPixels(g.h),decorated?decoration.dc:surface.dc,0,0,SRCCOPY);
 }else{
 bool drag=g.introPhase==3&&(g.mouseDown||scrubbing)&&(g.tab==0||g.tab==2)&&allocateDragBase();
 bool backgroundReady=(drag&&dragBaseReady)||g.introPhase!=3||restoreBackground();
 contentMoving=false;paintingContent=true;
 void*gfx=nullptr;if(GdipCreateFromHDC(g.memoryDC,&gfx)!=0)return;
 GdipSetSmoothingMode(gfx,4);GdipSetPixelOffsetMode(gfx,4);
 GdipScaleWorldTransform(gfx,uiDpi/96.0f,uiDpi/96.0f,0);
 Painter p={gfx};
 if(!backgroundReady)backdrop(p);
 if(!drag)dragBaseReady=false;
 if(!drag||!dragBaseReady){
  if(g.introPhase!=3)drawSplash(p,g.memoryDC);
  else{
   areaCount[g.tab]=0;buildingDragBase=drag;drawFrame(p);
   GdipTranslateWorldTransform(gfx,(float)sidebarExtra,0,0);contentOffset=sidebarExtra;
   if(g.tab==0)drawCrosshairTab(p,g.memoryDC);
   if(g.tab==1)drawResolutionTab(p,g.memoryDC);
   if(g.tab==2)drawColorTab(p,g.memoryDC);
   if(g.tab==3)drawSettingsTab(p,g.memoryDC);
   if(g.tab==4)drawPerformanceTab(p,g.memoryDC);
   if(g.tab==6)drawLibrary(p,g.memoryDC);
   if(g.tab==5)drawHome(p,g.memoryDC);
   buildingDragBase=false;
  }
  contentOffset=0;GdipDeleteGraphics(gfx);gfx=nullptr;drawQueuedText(g.memoryDC);
  if(drag){BitBlt(dragBase,0,0,uiPixels(g.w),uiPixels(g.h),g.memoryDC,0,0,SRCCOPY);dragBaseReady=true;}
 }
 if(drag){
  if(gfx)GdipDeleteGraphics(gfx);
  BitBlt(g.memoryDC,0,0,uiPixels(g.w),uiPixels(g.h),dragBase,0,0,SRCCOPY);
  if(GdipCreateFromHDC(g.memoryDC,&gfx)==0){
   GdipSetSmoothingMode(gfx,4);GdipSetPixelOffsetMode(gfx,4);
   GdipScaleWorldTransform(gfx,uiDpi/96.0f,uiDpi/96.0f,0);
   GdipTranslateWorldTransform(gfx,(float)sidebarExtra,0,0);p.gfx=gfx;contentOffset=sidebarExtra;
   if(g.tab==0)crosshairLive(p,g.memoryDC);else visibilityLive(p,g.memoryDC);
   contentOffset=0;GdipDeleteGraphics(gfx);drawQueuedText(g.memoryDC);
  }
 }
 paintingContent=false;
 if(canCache){
  BitBlt(surface.dc,0,0,uiPixels(g.w),uiPixels(g.h),g.memoryDC,0,0,SRCCOPY);
  surface.revision=contentMoving?0:contentRevision;
 }
 }
 if(g.introPhase==3){
  if(!decorated){
   paintAmbient();
   if(canCache && surface.revision==contentRevision && decoration.prepare()){
    BitBlt(decoration.dc,0,0,uiPixels(g.w),uiPixels(g.h),g.memoryDC,0,0,SRCCOPY);
    decoration.revision=contentRevision;decoratedTab=g.tab;decoratedTime=ambientTime;
   }else decoration.revision=0;
  }
  paintFeedback();
  if(g.tab==5||(g.tab==0&&g.customPicker)){
   void* gfx=nullptr;if(!GdipCreateFromHDC(g.memoryDC,&gfx)){
    GdipScaleWorldTransform(gfx,uiDpi/96.0f,uiDpi/96.0f,0);GdipSetSmoothingMode(gfx,4);Painter overlay={gfx};
    if(g.tab==5){homeUsage(overlay,133,cpuLoad,homeLevels[0]);homeUsage(overlay,365,gpuLoad,homeLevels[1]);homeUsage(overlay,597,ramLoad,homeLevels[2]);
     wchar_t cpu[64]={},video[64]={},ram[64]={};
     appendNumber(cpu,64,logicalProcessors);wappend(cpu,64,L" logical processors");
     memoryDetail(video,videoUsed,videoTotal,L" GB VRAM");memoryDetail(ram,ramUsed,ramTotal,L" GB RAM");
     text(g.memoryDC,g.fontTiny,cpu,133,561,214,22,C_DIM);
     text(g.memoryDC,g.fontTiny,video,365,561,224,22,C_DIM);
     text(g.memoryDC,g.fontTiny,ram,597,561,224,22,C_DIM);}
    else drawPicker(overlay,g.memoryDC);
    GdipDeleteGraphics(gfx);drawQueuedText(g.memoryDC);
   }
  }
 }
 BitBlt(hdc,0,0,uiPixels(g.w),uiPixels(g.h),g.memoryDC,0,0,SRCCOPY);
}
static void renewBitmap(){
 clearSurfaces();
 releaseDragBase();
 
 if(!g.hwnd)return;RECT r;GetClientRect(g.hwnd,&r);g.w=logical(r.right);g.h=logical(r.bottom);
 HDC target=GetDC(g.hwnd);if(!g.memoryDC)g.memoryDC=CreateCompatibleDC(target);
 if(g.memoryBitmap){SelectObject(g.memoryDC,g.oldBitmap);DeleteObject(g.memoryBitmap);}
 BITMAPINFO info={};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=uiPixels(g.w);
 info.bmiHeader.biHeight=-uiPixels(g.h);info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;
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
 if(style==12||style>=61){
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
 if(style>=13&&style<=60){
  CrossShape ops[64];int count=makeExtraCross(style,g.size,g.thickness,g.gap,false,ops);
  for(int i=0;i<count;i++){
   CrossShape& a=ops[i];
   if(a.kind==0)softwareLine(b,cx+a.x1,cy+a.y1,cx+a.x2,cy+a.y2,(float)a.t,col);
   else if(a.kind==1)softwareCircle(b,cx+a.x1,cy+a.y1,(float)a.r,0,true,col);
   else softwareCircle(b,cx+a.x1,cy+a.y1,(float)a.r,(float)a.t,false,col);
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
 if(!coloursAllowed()||g.gamma==100){
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
static void buildColourMatrix(float* m){
 // The fullscreen colour matrix supports real chroma mixing, brightness,
 // and linear shadow lift. It is not a non-linear gamma curve, so the gamma
 // portion is a midtone approximation when Windows rejects hardware ramps.
 float sat=fclamp((float)g.saturation/100.0f,0,2.0f);
 float ct=fclamp((float)g.contrast/100.0f,0.7f,1.3f);
 float brightness=(g.brightness-100)/220.0f;
 float lift=(float)g.shadows/430.0f;
 float midtone=(g.gamma-100)/580.0f;
 const float luma[3]={0.2126f,0.7152f,0.0722f};

 for(int src=0;src<3;src++)for(int dst=0;dst<3;dst++){
  m[src*5+dst]=ct*((1.0f-sat)*luma[src]+(src==dst?sat:0.0f));
 }
 m[18]=1.0f;m[24]=1.0f;
 // Fullscreen color transform uses matrix row 5 for RGB translation.
 float bias=0.5f*(1-ct)+brightness+lift+midtone;
 for(int dst=0;dst<3;dst++)m[20+dst]=bias;
}

static void setColorMatrix(){
 if(!g.magReady||!g.magSupported)return;
 if(!coloursAllowed()||(g.saturation==100&&g.shadows==0&&g.brightness==100&&g.contrast==100&&g.gamma==100)){
  restoreColorMatrix();return;
 }
 float m[25]={};buildColourMatrix(m);
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
static wchar_t asciiLower(wchar_t ch){return ch>=L'A'&&ch<=L'Z'?ch+32:ch;}
static bool exeEquals(const wchar_t* a,const wchar_t* b){
 for(int i=0;i<128;i++){wchar_t x=asciiLower(a[i]),y=asciiLower(b[i]);
  if(x!=y)return false;if(x==0)return true;
 }return false;
}
static bool getForegroundExe(wchar_t* out,int cap){
 out[0]=0;HWND foreground=GetForegroundWindow();
 if(!foreground||foreground==g.hwnd||foreground==g.overlay)return false;
 DWORD pid=0;GetWindowThreadProcessId(foreground,&pid);
 if(!pid||pid==GetCurrentProcessId())return false;
 HANDLE process=OpenProcess(0x1000u,0,pid); // PROCESS_QUERY_LIMITED_INFORMATION
 if(!process)return false;
 wchar_t path[512]={};DWORD n=511;
 BOOL ok=QueryFullProcessImageNameW(process,0,path,&n);
 CloseHandle(process);if(!ok||!n)return false;
 int start=0;for(DWORD i=0;i<n;i++)if(path[i]==L'\\'||path[i]==L'/')start=(int)i+1;
 if((int)n-start>=cap)return false;
 for(int j=start;j<=(int)n;j++)out[j-start]=asciiLower(path[j]);
 return out[0]!=0;
}
static bool coloursAllowed(){
 return g.desktopColorOn && (!g.gameFocusOnly||g.gameFocusActive);
}
static void refreshGameFocus(){
 bool active=false;
 if(g.gameFocusOnly && g.gameProcess[0] && !g.gameBindPending){
  wchar_t foreground[128]={};
  if(getForegroundExe(foreground,128))active=exeEquals(foreground,g.gameProcess);
 }
 if(active!=g.gameFocusActive){
  g.gameFocusActive=active;
  // Only change system colour APIs when the game-focus state changes.
  applyColours(true);invalidate();
 }
}
static void configureGameMonitor(){
 if(!g.hwnd)return;
 KillTimer(g.hwnd,10);
 if(g.gameFocusOnly&&g.gameProcess[0])SetTimer(g.hwnd,10,350,nullptr);
}
static void bindForegroundGame(){
 // Capture after minimising: user has five seconds to switch to the game.
 if(g.gameBindPending)return;
 g.gameBindPending=true;g.gameBindAt=GetTickCount();
 g.gameFocusActive=false;
 applyColours(true);setStatus(L"Switch to the game you want to bind within 5 seconds...");
 SetTimer(g.hwnd,11,100,nullptr);
 ShowWindow(g.hwnd,SW_MINIMIZE);
}
static void finishBindGame(){
 wchar_t exe[128]={};bool found=getForegroundExe(exe,128);
 g.gameBindPending=false;KillTimer(g.hwnd,11);
 ShowWindow(g.hwnd,SW_RESTORE);
 if(found){
  wcopy(g.gameProcess,exe,128);g.gameFocusOnly=true;
  // Binding a game enables the display effects; no colour change happens
  // until that game's process actually owns the foreground window.
  g.desktopColorOn=true;
  setStatus(L"Game captured. Colours activate when this game is focused.");
 }else{
  g.gameFocusOnly=false;g.gameFocusActive=false;
  setStatus(L"Could not identify the selected game. Try again in borderless mode.");
 }
 configureGameMonitor();refreshGameFocus();applyColours(true);invalidate();
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
 g.startMode=d;g.desktopW=d.dmPelsWidth;g.desktopH=d.dmPelsHeight;g.desktopHz=d.dmDisplayFrequency;g.nativeMode=d;
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
 g.desktopW=d.dmPelsWidth;g.desktopH=d.dmPelsHeight;g.desktopHz=d.dmDisplayFrequency;
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
// NVIDIA's public NVAPI v1 display ABI. The driver calculates the scan timing.
DLL HINSTANCE API LoadLibraryExW(LPCWSTR,HANDLE,DWORD);
struct NvTimingRequest{
 U32 version,width,height;float hz;U32 flags[3],timingPolicy;
};
struct NvScanTiming{
 WORD horizontal[5];BYTE hPolarity;WORD vertical[5];BYTE vPolarity;
 WORD interlaced;U32 pixelClock;U32 driverExtras[16];
};
struct NvCustomMode{
 U32 version,width,height,depth,format;float viewport[4],xRatio,yRatio;
 NvScanTiming timing;U32 hardwareOnly;
};
static_assert(sizeof(NvTimingRequest)==32 && sizeof(NvScanTiming)==96 && sizeof(NvCustomMode)==144,"NVAPI v1 display layouts");
struct NvidiaModes{
 using Query=void*(*)(U32);
 HINSTANCE module;bool initialized,trial;U32 display;
 int (*primary)(U32*);
 int (*timing)(U32,NvTimingRequest*,NvScanTiming*);
 int (*preview)(U32*,U32,NvCustomMode*);
 int (*save)(U32*,U32,U32,U32);
 int (*restore)(U32*,U32);
 bool connect(){
  if(initialized)return true;
  if(!module)module=LoadLibraryExW(L"nvapi64.dll",nullptr,0x800); // System32
  if(!module)return false;
  auto query=(Query)GetProcAddress(module,"nvapi_QueryInterface");
  if(!query)return false;
  auto init=(int(*)())query(0x0150e828);
  primary=(decltype(primary))query(0x1e9d8a31);
  timing=(decltype(timing))query(0x175167e9);
  preview=(decltype(preview))query(0x1f7db630);
  save=(decltype(save))query(0x49882876);
  restore=(decltype(restore))query(0xcbbd40f0);
  initialized=primary&&timing&&preview&&save&&restore&&init&&init()==0;
  return initialized;
 }
 const wchar_t* tryMode(const DEVMODEW& target,const DEVMODEW& previous){
  if(!connect()||primary(&display)!=0)return L"Auto-create requires an NVIDIA-driven primary display.";
  NvTimingRequest request={};request.version=0x10000|sizeof(request);
  request.width=target.dmPelsWidth;request.height=target.dmPelsHeight;
  request.hz=(float)previous.dmDisplayFrequency;request.timingPolicy=1; // driver automatic
  NvCustomMode mode={};mode.version=0x10000|sizeof(mode);
  mode.width=request.width;mode.height=request.height;mode.depth=previous.dmBitsPerPel;
  mode.viewport[2]=mode.viewport[3]=mode.xRatio=mode.yRatio=1.0f;
  if(timing(display,&request,&mode.timing)!=0||!mode.timing.pixelClock)
   return L"NVIDIA could not calculate this mode at your current refresh rate.";
  if(preview(&display,1,&mode)!=0)return L"NVIDIA declined this custom resolution on your monitor.";
  trial=true;return nullptr;
 }
};
static NvidiaModes nvidia={};
static bool customTrial(){return nvidia.trial;}
static void revertDisplay(){
 if(!g.keepPending)return;
 bool driverRestored=nvidia.trial&&nvidia.restore(&nvidia.display,1)==0;
 if(!driverRestored&&ChangeDisplaySettingsExW(nullptr,&g.priorMode,nullptr,0,nullptr)!=0){
  setStatus(L"Windows could not restore the previous display mode. Retrying...");return;
 }
 nvidia.trial=false;g.keepPending=false;KillTimer(g.hwnd,2);g.remaining=0;refreshMode();
 setStatus(L"Display mode reverted to the previous resolution.");
}
static void testAndApplyMode(const DEVMODEW& cur,DEVMODEW target,bool createIfRejected=false){
 if(!SetTimer(g.hwnd,2,1000,nullptr)){setStatus(L"Recovery timer unavailable.");return;}
 g.priorMode=cur;
 LONG result=ChangeDisplaySettingsExW(nullptr,&target,nullptr,CDS_TEST,nullptr);
 if(result==0)result=ChangeDisplaySettingsExW(nullptr,&target,nullptr,0,nullptr);
 if(result!=0){
  const wchar_t* error=createIfRejected?nvidia.tryMode(target,cur):L"Windows rejected this display mode.";
  if(error){KillTimer(g.hwnd,2);setStatus(error);return;}
 }
 tuningPage=false;g.pendingMode=target;g.keepPending=true;g.remaining=15;g.modeTimestamp=GetTickCount();
 refreshMode();setStatus(L"");
}
static void tryHighestRefresh(){
 if(g.keepPending){setStatus(L"Keep or revert the pending display change first.");return;}
 DEVMODEW current={};current.dmSize=sizeof(current);
 if(!EnumDisplaySettingsW(nullptr,ENUM_CURRENT_SETTINGS,&current)){
  setStatus(L"Windows could not read the current display mode.");return;
 }
 DWORD best=current.dmDisplayFrequency;
 for(DWORD i=0;;++i){
  DEVMODEW m={};m.dmSize=sizeof(m);
  if(!EnumDisplaySettingsW(nullptr,i,&m))break;
  if(m.dmPelsWidth==current.dmPelsWidth&&m.dmPelsHeight==current.dmPelsHeight&&
     m.dmBitsPerPel==current.dmBitsPerPel&&m.displayOrPrint.display.dmDisplayOrientation==current.displayOrPrint.display.dmDisplayOrientation&&
     !(m.dmDisplayFlags&2)&&m.dmDisplayFrequency>best)best=m.dmDisplayFrequency;
 }
 if(best==current.dmDisplayFrequency){setStatus(L"No higher refresh rate available at this resolution.");return;}
 DEVMODEW target=current;
 target.dmDisplayFrequency=best;target.dmDisplayFlags=0;
 target.dmFields=DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL|DM_DISPLAYFREQUENCY|0x200000;
 testAndApplyMode(current,target);
}
// Choose the nearest supported mode to 80% linear scale, keeping shape and refresh.
static void tryLowerWorkload(){
 if(g.keepPending){setStatus(L"Keep or revert the pending display change first.");return;}
 DEVMODEW cur={};cur.dmSize=sizeof(cur);
 if(!EnumDisplaySettingsW(nullptr,ENUM_CURRENT_SETTINGS,&cur)){setStatus(L"Could not read the current display mode.");return;}
 DEVMODEW best={};int bestError=2147483647;
 for(DWORD i=0;;++i){
  DEVMODEW m={};m.dmSize=sizeof(m);if(!EnumDisplaySettingsW(nullptr,i,&m))break;
  if(m.dmPelsWidth>=cur.dmPelsWidth||m.dmPelsHeight>=cur.dmPelsHeight||m.dmPelsWidth<1040||m.dmPelsHeight<720||
     m.dmBitsPerPel!=cur.dmBitsPerPel||m.dmDisplayFrequency<cur.dmDisplayFrequency||(m.dmDisplayFlags&2)||
     m.displayOrPrint.display.dmDisplayOrientation!=cur.displayOrPrint.display.dmDisplayOrientation)continue;
  S64 ratio=(S64)m.dmPelsWidth*cur.dmPelsHeight-(S64)m.dmPelsHeight*cur.dmPelsWidth;
  if((ratio<0?-ratio:ratio)*200>(S64)m.dmPelsHeight*cur.dmPelsWidth)continue;
  int distance=absI((int)m.dmPelsWidth*5-(int)cur.dmPelsWidth*4);
  if(distance<bestError||(distance==bestError&&m.dmDisplayFrequency>best.dmDisplayFrequency)){best=m;bestError=distance;}
 }
 if(!best.dmPelsWidth){setStatus(L"No suitable lower resolution preserves your aspect ratio and refresh rate.");return;}
 best.dmFields=DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL|DM_DISPLAYFREQUENCY|0x200000;
 testAndApplyMode(cur,best);
}
static void startDisplayMode(int selected){
 if(g.keepPending){revertDisplay();if(g.keepPending)return;}
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
 testAndApplyMode(cur,target,selected!=0);
}
static void keepDisplay(){
 if(!g.keepPending)return;
 DEVMODEW current={};current.dmSize=sizeof(current);
 if(!EnumDisplaySettingsW(nullptr,ENUM_CURRENT_SETTINGS,&current)){revertDisplay();return;}
 if(nvidia.trial){
  if(current.dmPelsWidth!=g.pendingMode.dmPelsWidth||current.dmPelsHeight!=g.pendingMode.dmPelsHeight){
   revertDisplay();if(!g.keepPending)setStatus(L"Requested size was not active. Previous mode restored.");return;
  }
  if(nvidia.save(&nvidia.display,1,1,1)!=0){
   revertDisplay();if(!g.keepPending)setStatus(L"NVIDIA could not save the mode. Previous mode restored.");return;
  }
  nvidia.trial=false;setStatus(L"Custom resolution saved for this monitor in NVIDIA settings.");
 }else setStatus(L"Display mode kept for this Windows session.");
 g.keepPending=false;KillTimer(g.hwnd,2);refreshMode();
}
static void showTray(bool minimize){
 if(minimize&&!g.minimizeToTray){ShowWindow(g.hwnd,SW_MINIMIZE);return;}
 if(minimize){
  if(!g.trayShown){NOTIFYICONDATAW n={};n.cbSize=sizeof(n);n.hWnd=g.hwnd;n.uID=7;n.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
   n.uCallbackMessage=WM_TRAY;n.hIcon=g.icon;wcopy(n.szTip,L"Vela  |  Click to open",128);
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
  int value=clamp(lo+(x-664)*(hi-lo)/383,lo,hi);
  if(control==0)g.size=value;else if(control==1)g.thickness=value;
  else if(control==2)g.gap=value;else g.opacity=value;
  if(g.overlayOn&&g.overlay)InvalidateRect(g.overlay,nullptr,0);
 }else if(g.tab==2){
  const int lo[5]={0,75,0,70,70};
  const int hi[5]={200,160,100,130,130};
  int sx=visX;
  int value=clamp(lo[control]+(x-sx)*(hi[control]-lo[control])/visWidth,lo[control],hi[control]);
  if(control==0)g.saturation=value;
  else if(control==1)g.gamma=value;
  else if(control==2)g.shadows=value;
  else if(control==3)g.brightness=value;
  else g.contrast=value;
  g.colorPreset=-1; // Preview locally during capture. Commit driver changes once on release.
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
  if(enterHit(x,y))enterDashboard();
  return;
 }
 if(y>=7&&y<=50){
  if(x>=g.w-61){DestroyWindow(g.hwnd);return;}
  if(x>=g.w-120){showTray(true);return;}
 }
 for(int i=0;i<tabCount;i++)if(tabHit(x,y,i)){
  if(g.tab==kNavTabs[i])return;

  if(g.customPicker||g.status[0])++contentRevision;
  g.tab=kNavTabs[i];g.customPicker=false;g.hoverTile=-1;g.status[0]=0;hoveredArea=-1;
  if(g.tab==6&&!games)refreshLibrary();
  syncAmbient(true);
  syncEditors();repaint();return;}
 if(y<59||x<84)return;
 x-=sidebarExtra;
 if(g.tab==6){libraryClick(x,y);return;}
 if(g.tab==5){
  if(hit(x,y,892,128,185,40)){clickWindow(42,railY(5));return;}
  const int rows[]={1,2,3,4};
  for(int i=0;i<4;++i){
   int left=111+246*i;
   if(hit(x,y,left,213,228,136)){clickWindow(42,railY(rows[i]));return;}
  }
  return;
 }
 if(g.tab==0){
  if(g.customPicker){
   if(absI(x-1039)<=14&&absI(y-548)<=15){g.customPicker=false;invalidate();return;}
   if(hit(x,y,566,321,200,124)){g.pickerDrag=1;SetCapture(g.hwnd);pickerMove(x,y);return;}
   if(hit(x,y,566,455,200,14)){g.pickerDrag=2;SetCapture(g.hwnd);pickerMove(x,y);return;}
   if(hit(x,y,551,274,243,248))return;
   g.customPicker=false;
  }
  int pages=totalCrosshairPages();
  if(hit(x,y,652,217,39,33)){g.crosshairPage=clamp(g.crosshairPage-1,0,pages-1);g.hoverTile=-1;invalidate();return;}
  if(hit(x,y,758,217,39,33)){g.crosshairPage=clamp(g.crosshairPage+1,0,pages-1);g.hoverTile=-1;invalidate();return;}
  if(hit(x,y,956,219,108,30)){importCrosshair();invalidate();return;}
  if(hit(x,y,521,553,72,43)){g.overlayOn=!g.overlayOn;applyOverlay();return;}
  for(int i=0;i<5;i++)if(hit(x,y,653+i*85,171,77,47)){
   int id=crosshairAt(g.crosshairPage,i);
   if(id<0)continue;
   if(id>=61){if(activatePNG(id-61))g.preset=id;}
   else g.preset=id;
   if(g.overlay)InvalidateRect(g.overlay,nullptr,0);invalidate();return;
  }
  for(int i=0;i<4;i++)if(hit(x,y,656,278+i*65,399,33)){
   dragBaseReady=false;g.downSlider=i;g.mouseDown=true;SetCapture(g.hwnd);onSlide(x,i);return;
  }
  for(int i=0;i<7;i++)if(absI(x-(673+i*61))<=13&&absI(y-548)<=14){
   if(i==6){g.customPicker=!g.customPicker;if(g.customPicker)rgbToHSV();}
   else{g.swatch=i;g.color=swatches[i]&0xFFFFFF;g.customPicker=false;}
   if(g.overlay)InvalidateRect(g.overlay,nullptr,0);invalidate();return;
  }
  g.customPicker=false;
 }
 if(g.tab==1){
  for(int i=0;i<3;++i)if(hit(x,y,115+100*i,135,94,30)){displayPage=i;invalidate();return;}
  for(int i=0;i<12;i++)if(hit(x,y,115+(i%3)*322,188+(i/3)*75,305,65)){
   g.selectedMode=modePages[displayPage][i];g.status[0]=0;invalidate();return;
  }
  for(int field=0;field<2;++field)if(hit(x,y,536+field*210,513,188,46)){
   SetFocus(field?g.editHeight:g.editWidth);g.selectedMode=15;g.status[0]=0;invalidate();return;
  }
  if(hit(x,y,860,631,207,39)){tryHighestRefresh();return;}
  if(hit(x,y,115,568,216,41)){startDisplayMode(g.selectedMode);invalidate();return;}
  if(hit(x,y,345,568,216,41)){g.selectedMode=0;displayPage=0;startDisplayMode(0);invalidate();return;}
  if(g.keepPending&&hit(x,y,601,591,207,29)){keepDisplay();return;}
  if(g.keepPending&&hit(x,y,826,591,226,29)){revertDisplay();return;}
 }
 if(g.tab==2){
  if(hit(x,y,compareX,compareY,compareW,compareH)){
   scrubbing=true;dragBaseReady=false;SetCapture(g.hwnd);moveComparison(x);return;
  }
  for(int i=0;i<3;i++)if(hit(x,y,modeX+i*segmentWidth,modeY,segmentWidth,36)){chooseColorPreset(i);return;}
  if(hit(x,y,974,379,81,44)){
   g.desktopColorOn=!g.desktopColorOn;applyColours(true);invalidate();return;
  }
  if(hit(x,y,974,435,81,44)){
   if(!g.gameFocusOnly && !g.gameProcess[0]){
    setStatus(L"Bind a game first, then enable Game Focus Mode.");return;
   }
   g.gameFocusOnly=!g.gameFocusOnly;
   if(!g.gameFocusOnly)g.gameFocusActive=false;
   configureGameMonitor();refreshGameFocus();applyColours(true);invalidate();return;
  }
  if(hit(x,y,889,490,155,46)){bindForegroundGame();return;}
  for(int i=0;i<5;i++)if(hit(x,y,visX-8,visY(i)+24,visWidth+16,35)){
   dragBaseReady=false;g.downSlider=i;g.mouseDown=true;SetCapture(g.hwnd);onSlide(x,i);return;
  }
 }
 if(g.tab==4){
  if(hit(x,y,690,84,170,38)||hit(x,y,874,84,194,38)){
   if(g.keepPending){setStatus(L"Keep or revert your display change first.");return;}
   tuningPage=x>=874;g.status[0]=0;invalidate();return;
  }
  if(tuningPage){tuningClick(x,y);return;}
  if(hit(x,y,558,305,154,36)||hit(x,y,558,376,154,36)){
   if(g.keepPending)return;tuningPage=true;g.status[0]=0;invalidate();return;
  }
  if(hit(x,y,558,163,154,36)){tryHighestRefresh();return;}
  if(hit(x,y,558,234,154,36)){tryLowerWorkload();return;}
  if(hit(x,y,985,474,82,48)){
   quietVela=!quietVela;updateOwnPriority();syncAmbient(false);syncAmbient(true);invalidate();return;
  }
  if(hit(x,y,985,538,82,48)){animateWallpaper=!animateWallpaper;syncAmbient(true);invalidate();return;}
  if(g.keepPending&&hit(x,y,692,622,166,38)){keepDisplay();invalidate();return;}
  if(g.keepPending&&hit(x,y,879,622,176,38)){revertDisplay();invalidate();return;}
 }
 if(g.tab==3){
  for(int i=0;i<accentCount;++i)if(hit(x,y,500+i*113,635,108,38)){
   if(i==accentIndex)return;
   CreateDirectoryW(g.profileDirectory,nullptr);
   if(!WritePrivateProfileStringW(L"Appearance",L"Accent",accents[i].name,appearanceIni)){
    setStatus(L"Could not save the accent colour.");return;
   }
   accentIndex=i;amberLight.release();peachLight.release();invalidate();return;
  }
  for(int i=0;i<4;i++)if(hit(x,y,115+(i%2)*486,223+(i/2)*72,469,61)){
   g.activeProfile=i;syncProfileEditor();g.status[0]=0;invalidate();return;
  }
  if(hit(x,y,115,432,219,43)){saveSelectedProfile();invalidate();return;}
  if(hit(x,y,346,432,219,43)){loadSelectedProfile();invalidate();return;}
  if(hit(x,y,578,432,219,43)){deleteSelectedProfile();invalidate();return;}
  if(hit(x,y,810,432,259,43)){resetSession();invalidate();return;}
#ifdef VELA_LOCAL_PREVIEW
  if(hit(x,y,836,557,205,36)){
   if((S64)ShellExecuteW(g.hwnd,L"open",L"https://github.com/fozarma/Vela/releases",nullptr,nullptr,1)<=32)
    setStatus(L"Could not open the upstream releases page.");
   return;
  }
#endif
  if(hit(x,y,973,506,87,48)){g.minimizeToTray=!g.minimizeToTray;invalidate();return;}
 }
}
static void moveMouse(int x,int y){
 if(scrubbing){moveComparison(x-sidebarExtra);return;}
 int nav=-1,tile=-1,control=-1;
 for(int i=0;i<tabCount;i++)if(tabHit(x,y,i))nav=i;
 if(y<50){if(x>=g.w-61)control=202;else if(x>=g.w-120)control=200;}
 x-=sidebarExtra;pointerX=x;pointerY=y;
 int area=nav<0&&!g.customPicker?areaAt(x,y):-1;
 if(nav!=g.hoverNav||control!=g.hoverControl||area!=hoveredArea){
  g.hoverNav=nav;g.hoverControl=control;hoveredArea=area;repaint();
 }
 if(g.pickerDrag)pickerMove(x,y);
 if(g.mouseDown&&g.downSlider>=0)onSlide(x,g.downSlider);
}
static LRESULT API overlayProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
 if(msg==WM_PAINT){overlayPaint(hwnd);return 0;}
 if(msg==WM_NCHITTEST)return -1; // HTTRANSPARENT, never consume game inputs.
 if(msg==WM_ERASEBKGND)return 1;
 return DefWindowProcW(hwnd,msg,wp,lp);
}
static void createFonts();
static LRESULT API mainProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
 if(msg==0x02E0){
  uiDpi=(int)(wp&65535);RECT* r=(RECT*)lp;
  int w=uiPixels(g.introPhase==3?BASE_W:SPLASH_W),h=uiPixels(g.introPhase==3?BASE_H:SPLASH_H);
  SetWindowPos(hwnd,nullptr,r->left,r->top,w,h,SWP_NOACTIVATE);
  SetWindowRgn(hwnd,CreateRoundRectRgn(0,0,w+1,h+1,uiPixels(24),uiPixels(24)),1);
  if(g.editWidth){
   createFonts();
   SetWindowPos(g.editWidth,nullptr,uiPixels(605+sidebarExtra),uiPixels(526),uiPixels(82),uiPixels(21),SWP_NOACTIVATE);
   SetWindowPos(g.editHeight,nullptr,uiPixels(815+sidebarExtra),uiPixels(526),uiPixels(82),uiPixels(21),SWP_NOACTIVATE);
   SetWindowPos(g.editProfileName,nullptr,uiPixels(249+sidebarExtra),uiPixels(378),uiPixels(316),uiPixels(29),SWP_NOACTIVATE);
  }
  invalidate();return 0;
 }
 if(msg==steamLaunchFinished){setStatus((const wchar_t*)lp);return 0;}
 if(msg==libraryLoaded){
  scanningGames=false;GameList* updated=(GameList*)lp;
  if(!updated){setStatus(L"Game scan failed.");return 0;}
  freeGames(games);games=updated;libraryPage=clamp(libraryPage,0,imax(0,(games->count-1)/6));invalidate();return 0;
 }
 if(msg==WM_NCHITTEST){
  // WM_NCHITTEST uses screen coordinates, not client coordinates.
  RECT r={};GetWindowRect(hwnd,&r);int x=(int)(short)(lp&0xffff)-(int)r.left,y=(int)(short)((lp>>16)&0xffff)-(int)r.top;
  x=logical(x);y=logical(y);if(y>=0&&y<56&&x>(g.introPhase==3?70:21)&&x<(g.w-122))return HTCAPTION;
  return HTCLIENT;
 }
 if(msg==6){appActive=(wp&65535)!=0;updateOwnPriority();syncAmbient(true);}
 if(msg==24)syncAmbient(wp!=0);
 if(msg==WM_TIMER&&wp==18){if((g.tab==4||g.tab==5)&&appActive&&!IsIconic(hwnd)&&!g.trayShown){sampleSystem();pageSurfaces[4].revision=0;if(g.tab==4)decoration.revision=0;repaint();}return 0;}
 if(msg==WM_TIMER&&wp==14){
  if(ambientRunning){U32 now=GetTickCount();ambientTime+=now-ambientTick;ambientTick=now;
   if(!g.mouseDown&&!scrubbing)repaint();}
  return 0;
 }
 if(msg==WM_SIZE){if(wp==1&&quietVela){clearSurfaces();releaseDragBase();}syncAmbient(wp!=1);if(wp!=1&&g.memoryDC)renewBitmap();invalidate();return 0;}
 if(msg==WM_CTLCOLOREDIT){
  HDC dc=(HDC)wp;if((HWND)lp==g.editWidth||(HWND)lp==g.editHeight){SetTextColor(dc,0x00F4F1EC);SetBkColor(dc,0x00211B17);return (LRESULT)g.numberBrush;}
  SetTextColor(dc,0x00EEEAE6);SetBkColor(dc,0x00554238);
  return (LRESULT)g.editBrush;
 }
 if(msg==WM_COMMAND){
  int id=(int)(wp&0xFFFF),notification=(int)((wp>>16)&0xFFFF);
  if((id==101||id==102)&&(notification==0x100||notification==0x200))invalidate();
  if(!g.initializingEditors&&(id==101||id==102)&&notification==0x300){
   g.selectedMode=15;readCustomResolution();invalidate();
  }
  return 0;
 }
 if(msg==WM_ERASEBKGND)return 1;
 if(msg==0x0318&&wp){render((HDC)wp);return 0;} // WM_PRINTCLIENT uses the same composited frame as WM_PAINT.
 if(msg==WM_PAINT){PAINTSTRUCT ps={};HDC dc=BeginPaint(hwnd,&ps);render(dc);EndPaint(hwnd,&ps);return 0;}
 if(msg==WM_MOUSEMOVE){
  if(g.introPhase!=3){
   if(!g.trackingMouse){TRACKMOUSEEVENT t={sizeof(t),2,hwnd,0};g.trackingMouse=TrackMouseEvent(&t)!=0;}
   int x=logical((short)(lp&0xFFFF)),y=logical((short)((lp>>16)&0xFFFF));
   int target=(g.introPhase==5&&enterHit(x,y))?10:0;
   if(target!=g.introHoverTarget){g.introHoverTarget=target;invalidate();}
   return 0;
  }if(!g.trackingMouse){TRACKMOUSEEVENT t={sizeof(t),2,hwnd,0};g.trackingMouse=TrackMouseEvent(&t)!=0;}int x=logical((short)(lp&0xFFFF)),y=logical((short)((lp>>16)&0xFFFF));moveMouse(x,y);return 0;}
 if(msg==WM_MOUSELEAVE){pointerX=pointerY=-999;
  if(g.introPhase!=3){g.trackingMouse=false;g.introHoverTarget=0;invalidate();return 0;}
  g.trackingMouse=false;g.hoverNav=-1;g.hoverControl=-1;hoveredArea=-1;repaint();return 0;}
 if(msg==WM_LBUTTONDOWN){focusedTab=-1;int x=logical((short)(lp&0xFFFF)),y=logical((short)((lp>>16)&0xFFFF));clickWindow(x,y);return 0;}
 if(msg==0x0215&&scrubbing){scrubbing=false;invalidate();return 0;}
 if(msg==WM_LBUTTONUP&&scrubbing){
  moveComparison(logical((short)(lp&65535))-sidebarExtra);scrubbing=false;
  ReleaseCapture();invalidate();return 0;
 }
 if(msg==0x0215&&g.mouseDown){
  g.mouseDown=false;g.downSlider=-1;invalidate();
  if(g.tab==2&&g.desktopColorOn)applyColours(true);return 0;
 }
 if(msg==WM_LBUTTONUP){if(g.pickerDrag){g.pickerDrag=0;ReleaseCapture();}
  if(g.mouseDown){
   int x=logical((short)(lp&65535))-sidebarExtra;onSlide(x,g.downSlider);
   g.mouseDown=false;g.downSlider=-1;ReleaseCapture();invalidate();UpdateWindow(hwnd);
   if(g.tab==2&&g.desktopColorOn)applyColours(true);
  }return 0;}
 if(msg==WM_KEYDOWN&&g.introPhase==3){
  if(wp==9||wp==40||wp==38){
   focusedTab=(focusedTab<0?0:(focusedTab+(wp==38?tabCount-1:1))%tabCount);
   invalidate();return 0;
  }
  if((wp==13||wp==32)&&focusedTab>=0){
   clickWindow(42,railY(focusedTab));repaint();return 0;
  }
 }
 if(msg==WM_KEYDOWN&&(wp==0x0D||wp==0x20)&&g.introPhase==5){enterDashboard();return 0;}
 if(msg==WM_KEYDOWN&&wp==0x1B){if(g.customPicker){g.customPicker=false;invalidate();}return 0;}
 if(msg==WM_HOTKEY&&wp==1){if(g.introPhase!=3)return 0;g.overlayOn=!g.overlayOn;applyOverlay();return 0;}
 if(msg==WM_TRAY){if(lp==WM_LBUTTONDBLCLK_TRAY||lp==WM_LBUTTONUP_TRAY){showTray(false);}return 0;}
 if(msg==WM_DISPLAYCHANGE){refreshMode();if(g.introPhase==3&&g.desktopColorOn)SetTimer(hwnd,4,750,nullptr);if(g.overlay){int sx=GetSystemMetrics(SM_CXSCREEN),sy=GetSystemMetrics(SM_CYSCREEN);SetWindowPos(g.overlay,(HWND)-1,sx/2-128,sy/2-128,256,256,SWP_NOACTIVATE);InvalidateRect(g.overlay,nullptr,0);}invalidate();return 0;}
 // Dedicated hover clock. The start screen, background particles and
 // selected-tab state do not depend on mouse movement.
 if(msg==WM_TIMER&&wp==9){
  if(g.introPhase!=3){KillTimer(hwnd,9);return 0;}
  U32 now=GetTickCount();
  U32 dt=now-g.navLastTick;g.navLastTick=now;
  int step=clamp((int)(dt*255u/200u),1,255);
  bool any=false;
  for(int i=0;i<4;i++){
   if(i==g.hoverNav){
    g.navHoverFade[i]=imin(255,g.navHoverFade[i]+step);
    g.navPhase[i]=(float)((now-g.navCycleStart[i])%1800u)/1800.0f;
    any=true; // Only internal strokes animate, with no spin or scaling.
   }else if(g.navHoverFade[i]>0){
    g.navHoverFade[i]=imax(0,g.navHoverFade[i]-step);
    if(g.navHoverFade[i]==0)g.navPhase[i]=0.0f;
    else any=true; // Finish fade-out, then stop repainting.
   }
  }
  if(!any)KillTimer(hwnd,9);
  else invalidate();
  return 0;
 }
 if(msg==WM_TIMER&&wp==10){
  if(!g.gameBindPending)refreshGameFocus();return 0;
 }
 if(msg==WM_TIMER&&wp==11){
  if(g.gameBindPending&&GetTickCount()-g.gameBindAt>=5000)finishBindGame();return 0;
 }
 if(msg==WM_TIMER&&wp==8){
  if(g.logoHover<g.logoHoverTarget)g.logoHover=imin(g.logoHover+2,g.logoHoverTarget);
  else if(g.logoHover>g.logoHoverTarget)g.logoHover=imax(g.logoHover-2,g.logoHoverTarget);
  if(g.logoHover==g.logoHoverTarget)KillTimer(hwnd,8);
  invalidate();return 0;
 }
 // The splash and main UI are updated from a timer, never from hover events.
 // Keep elapsed durations independent of the timer cadence (smooth on slower PCs).
 if(msg==0x001A){SystemParametersInfoW(0x1042,0,&motionEnabled,0);if(g.introPhase==5&&motionEnabled)SetTimer(hwnd,5,32,nullptr);syncAmbient(true);invalidate();}
 if(msg==WM_TIMER&&wp==12){
  KillTimer(hwnd,12);frameRequested=false;
  if(!g.trayShown&&!IsIconic(hwnd))repaint();
  return 0;
 }
 if(msg==WM_TIMER&&wp==5){
  if(g.introPhase==3&&(g.trayShown||IsIconic(hwnd)))return 0;
  if(!motionEnabled&&g.introPhase==3&&g.mainFadeAlpha==255&&!g.keepPending)return 0;
  U32 t=GetTickCount();
  if(g.introPhase==0){
   g.introProgress=clamp((int)((t-g.introStartTick)/11),0,100);
   if(g.introProgress>=100){g.introPhase=1;g.introStageTick=t;}
  }else if(g.introPhase==1){
   if(t-g.introStageTick>=65){g.introPhase=4;g.introStageTick=t;}
  }else if(g.introPhase==4){
   if(t-g.introStageTick>=260){g.introPhase=5;SetTimer(hwnd,5,32,nullptr);}
  }else if(g.introPhase==5){
   if(!motionEnabled){KillTimer(hwnd,5);return 0;}
   if(g.trayShown||IsIconic(hwnd))return 0;
  }else if(g.introPhase==2){
   // Gently fade out the compact launch window before resizing it.
   U32 elapsed=motionEnabled?t-g.introStageTick:185;
   U8 alpha=(U8)clamp(255-(int)(elapsed*255/185),0,255);
   SetLayeredWindowAttributes(hwnd,0,alpha,LWA_ALPHA);
   if(elapsed>=185){
    g.introPhase=3;g.mainFadeTick=t;g.mainFadeAlpha=1;
    SetLayeredWindowAttributes(hwnd,0,1,LWA_ALPHA);
    int screenW=GetSystemMetrics(SM_CXSCREEN),screenH=GetSystemMetrics(SM_CYSCREEN);
    SetWindowPos(hwnd,nullptr,(screenW-uiPixels(BASE_W))/2,(screenH-uiPixels(BASE_H))/2,uiPixels(BASE_W),uiPixels(BASE_H),0);
    HRGN region=CreateRoundRectRgn(0,0,uiPixels(BASE_W)+1,uiPixels(BASE_H)+1,25,25);
    SetWindowRgn(hwnd,region,1);
    syncEditors();syncAmbient(true);if(g.desktopColorOn)applyColours(true);applyOverlay();
   }
  }else if(g.introPhase==3){
   if(g.mainFadeAlpha<255){
    float u=!motionEnabled?1.0f:fclamp((float)(t-g.mainFadeTick)/360.0f,0.0f,1.0f);
    float eased=1.0f-(1.0f-u)*(1.0f-u)*(1.0f-u);
    g.mainFadeAlpha=clamp(1+(int)(254.0f*eased),1,255);
    SetLayeredWindowAttributes(hwnd,0,(U8)g.mainFadeAlpha,LWA_ALPHA);
    // Once the entrance is over, keep just a quiet background animation clock.
    if(g.mainFadeAlpha==255)KillTimer(hwnd,5);
   }
  }
  repaint();
  return 0;
 }
 if(msg==WM_TIMER&&wp==4){KillTimer(hwnd,4);if(g.introPhase==3&&g.desktopColorOn)applyColours(true);return 0;}
 if(msg==WM_TIMER&&wp==2&&g.keepPending){
  // Use elapsed monotonic clock, not timer tick count, to enforce actual 15 s.
  int elapsed=(int)((GetTickCount()-g.modeTimestamp)/1000);
  g.remaining=clamp(15-elapsed,0,15);
  if(g.remaining==0)revertDisplay();invalidate();return 0;
 }
 if(msg==WM_VELA_UPDATE){
  if(wp!=1&&wp!=2&&wp!=5)manualUpdateBusy=false;
  const wchar_t* msgText=wp==1?L"Checking for Vela updates...":
      wp==2?L"A newer Vela release is downloading...":
      wp==3?L"Vela is up to date":
      wp==4?L"Update check unavailable - running installed version":
      wp==5?L"Update verified - restarting Vela...":
      wp==6?L"Update failed - use GitHub Releases to update manually":
      wp==7?L"Update rejected - SHA-256 verification failed":L"";
  wcopy(g.updateStatus,msgText,160);invalidate();return 0;
 }
 if(msg==WM_VELA_UPDATE_CLOSE){DestroyWindow(hwnd);return 0;}
 if(msg==WM_CLOSE){DestroyWindow(hwnd);return 0;}
 if(msg==WM_DESTROY){syncAmbient(false);amberLight.release();peachLight.release();clearSurfaces();releaseDragBase();
  KillTimer(hwnd,9);KillTimer(hwnd,10);KillTimer(hwnd,11);
  KillTimer(hwnd,5);KillTimer(hwnd,6);KillTimer(hwnd,8);if(g.keepPending)revertDisplay();KillTimer(hwnd,4);restoreGamma();restoreColorMatrix();
  if(g.magReady)MagUninitialize();
  UnregisterHotKey(hwnd,1);
  if(g.trayShown){NOTIFYICONDATAW n={};n.cbSize=sizeof(n);n.hWnd=hwnd;n.uID=7;Shell_NotifyIconW(NIM_DELETE,&n);}
  if(g.overlay)DestroyWindow(g.overlay);
  if(lightImage)GdipDisposeImage(lightImage);

 if(previewCrop)GdipDisposeImage(previewCrop);
 if(g.previewImage)GdipDisposeImage(g.previewImage);
  if(g.brandImage)GdipDisposeImage(g.brandImage);
  if(g.brandStream){void*** vp=(void***)g.brandStream;((U32(API*)(void*))((*vp)[2]))(g.brandStream);}
  if(g.importedImage)GdipDisposeImage(g.importedImage);
  for(int i=0;i<VELA_MAX_IMPORTS;i++)if(libraryPNGs[i].image){GdipDisposeImage(libraryPNGs[i].image);libraryPNGs[i].image=nullptr;}
  DeleteFileW(g.customPath);
  if(g.previewStream){void*** vp=(void***)g.previewStream;((U32(API*)(void*))((*vp)[2]))(g.previewStream);}
  if(g.numberBrush)DeleteObject(g.numberBrush);
  if(g.editBrush)DeleteObject(g.editBrush);
  if(g.memoryDC){if(g.memoryBitmap){SelectObject(g.memoryDC,g.oldBitmap);DeleteObject(g.memoryBitmap);}DeleteDC(g.memoryDC);}
  DeleteObject(g.fontTitle);DeleteObject(g.fontBody);DeleteObject(g.fontSmall);DeleteObject(g.fontSection);DeleteObject(g.fontTiny);DeleteObject(g.fontSplash);
  PostQuitMessage(0);return 0;
 }
 return DefWindowProcW(hwnd,msg,wp,lp);
}
static const wchar_t* chooseUIFont(){
 // The linked Codrops demo uses Lato with Calibri as its Windows fallback.
 // Test the actual selected face rather than trusting GDI font substitution.
 HFONT probe=CreateFontW(-18,0,0,0,400,0,0,0,1,0,0,5,0,L"Lato");
 if(!probe)return L"Calibri";
 HDC dc=GetDC(nullptr);
 bool installed=false;
 if(dc){
  HGDIOBJ old=SelectObject(dc,probe);
  wchar_t face[48]={};
  if(GetTextFaceW(dc,48,face)>0){
   installed=((face[0]==L'L'||face[0]==L'l')&&
              (face[1]==L'a'||face[1]==L'A')&&
              (face[2]==L't'||face[2]==L'T')&&
              (face[3]==L'o'||face[3]==L'O')&&
              (face[4]==0||face[4]==L' '));
  }
  SelectObject(dc,old);ReleaseDC(nullptr,dc);
 }
 DeleteObject(probe);
 return installed?L"Lato":L"Calibri";
}
static void createFonts(){
 HFONT old[]={g.fontTitle,g.fontSplash,g.fontBody,g.fontSmall,g.fontTiny,g.fontSection};
 const wchar_t* uiFont=L"Segoe UI";
 g.fontTitle=CreateFontW(-uiPixels(32),0,0,0,400,0,0,0,1,0,0,5,0,uiFont);
 g.fontSplash=CreateFontW(-uiPixels(44),0,0,0,500,0,0,0,1,0,0,5,0,uiFont);
 g.fontBody=CreateFontW(-uiPixels(16),0,0,0,400,0,0,0,1,0,0,5,0,uiFont);
 g.fontSmall=CreateFontW(-uiPixels(14),0,0,0,400,0,0,0,1,0,0,5,0,uiFont);
 g.fontTiny=CreateFontW(-uiPixels(13),0,0,0,400,0,0,0,1,0,0,5,0,uiFont);
 g.fontSection=CreateFontW(-uiPixels(16),0,0,0,400,0,0,0,1,0,0,5,0,uiFont);
 if(g.editWidth){
  HWND editors[]={g.editWidth,g.editHeight,g.editProfileName};
  for(HWND editor:editors)SendMessageW(editor,WM_SETFONT,(WPARAM)g.fontSection,1);
 }
 for(HFONT font:old)if(font)DeleteObject(font);
}
static int launch(){
 SystemParametersInfoW(0x1042,0,&motionEnabled,0);
 HINSTANCE user=GetModuleHandleW(L"user32.dll");
 auto awareness=(BOOL(API*)(void*))GetProcAddress(user,"SetProcessDpiAwarenessContext");
 auto systemDpi=(UINT(API*)())GetProcAddress(user,"GetDpiForSystem");
 if(!awareness||!awareness((void*)-4))SetProcessDPIAware();
 if(systemDpi)uiDpi=systemDpi();
 g.tab=5;g.w=SPLASH_W;g.h=SPLASH_H;g.hoverNav=g.hoverTile=g.hoverControl=g.downSlider=-1;
 g.preset=1;g.crosshairPage=0;g.size=19;g.thickness=5;g.gap=1;g.opacity=100;g.swatch=-1;g.color=0x29B7FF;
 g.brightness=100;g.contrast=100;g.gamma=100;g.selectedMode=0;
 g.customW=1440;g.customH=1080;g.red=g.green=g.blue=g.temperature=100;
 g.shadows=0;g.desktopColorOn=false;g.minimizeToTray=false;
 g.saturation=100;g.vibrance=100;g.highlights=100;g.blackLevel=0;
 GdiplusStartupInput gd={1,nullptr,0,0};if(GdiplusStartup(&g.gdipToken,&gd,nullptr)!=0)return 1;
 findNativeAndCurrent();initGamma();makePath();
 readHardware();sampleSystem();prepareProfileStore();discoverProfiles();loadPNGLibrary();g.activeProfile=0;
 loadAppearance();

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
 createFonts();
 HINSTANCE inst=GetModuleHandleW(nullptr);
 g.icon=LoadIconW(inst,MAKERESOURCE(1));if(!g.icon)g.icon=LoadIconW(nullptr,MAKERESOURCE(32512));
 WNDCLASSEXW klass={};klass.cbSize=sizeof(klass);klass.style=3;klass.lpfnWndProc=mainProc;klass.hInstance=inst;klass.hIcon=g.icon;klass.hIconSm=g.icon;klass.hCursor=LoadCursorW(nullptr,MAKERESOURCE(32512));klass.lpszClassName=L"VelaNativeWindow";
 if(!RegisterClassExW(&klass))return 2;
 klass.lpfnWndProc=overlayProc;klass.lpszClassName=L"VelaClickThroughOverlay";klass.style=0;klass.hIcon=nullptr;
 if(!RegisterClassExW(&klass))return 3;
 int sx=GetSystemMetrics(SM_CXSCREEN),sy=GetSystemMetrics(SM_CYSCREEN);
 g.hwnd=CreateWindowExW(WS_EX_APPWINDOW|WS_EX_LAYERED,L"VelaNativeWindow",L"Vela",WS_POPUP|0x02000000|WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
  (sx-uiPixels(SPLASH_W))/2,(sy-uiPixels(SPLASH_H))/2,uiPixels(SPLASH_W),uiPixels(SPLASH_H),nullptr,nullptr,inst,nullptr);
 if(!g.hwnd)return 4;
 SetLayeredWindowAttributes(g.hwnd,0,255,LWA_ALPHA);
 // Match 20px-radius source reference window on Windows 10 and Windows 11.
 HRGN rgn=CreateRoundRectRgn(0,0,uiPixels(SPLASH_W)+1,uiPixels(SPLASH_H)+1,23,23);SetWindowRgn(g.hwnd,rgn,1);
 renewBitmap();
 g.numberBrush=CreateSolidBrush(0x00211B17);
 g.editBrush=CreateSolidBrush(0x00554238);
 g.editWidth=CreateWindowExW(0,L"EDIT",L"",WS_CHILD|ES_NUMBER|ES_AUTOHSCROLL|WS_TABSTOP,
  uiPixels(605+sidebarExtra),uiPixels(526),uiPixels(82),uiPixels(21),g.hwnd,(void*)101,inst,nullptr);
 g.editHeight=CreateWindowExW(0,L"EDIT",L"",WS_CHILD|ES_NUMBER|ES_AUTOHSCROLL|WS_TABSTOP,
  uiPixels(815+sidebarExtra),uiPixels(526),uiPixels(82),uiPixels(21),g.hwnd,(void*)102,inst,nullptr);
 g.editProfileName=CreateWindowExW(0,L"EDIT",L"",WS_CHILD|ES_AUTOHSCROLL|WS_TABSTOP,
  uiPixels(249+sidebarExtra),uiPixels(378),uiPixels(316),uiPixels(29),g.hwnd,(void*)103,inst,nullptr);
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
  L"VelaClickThroughOverlay",L"",WS_POPUP,sx/2-128,sy/2-128,256,256,nullptr,nullptr,inst,nullptr);
 // UpdateLayeredWindow owns the per-pixel alpha; do not call SetLayeredWindowAttributes here.
 g.hotkeyOK=RegisterHotKey(g.hwnd,1,0,VK_F1)!=0;
 if(!g.hotkeyOK)setStatus(L"F1 hotkey is already in use by another application.");
 // Imported PNG is intentionally NOT loaded at startup. Manual import or profile Load only.
 g.introPhase=motionEnabled?0:5;g.introProgress=motionEnabled?0:100;g.introFade=0;g.mainFadeAlpha=255;g.introStartTick=GetTickCount();g.introStageTick=g.introStartTick;SetTimer(g.hwnd,5,16,nullptr);
 ShowWindow(g.overlay,SW_HIDE);ShowWindow(g.hwnd,SW_SHOW);UpdateWindow(g.hwnd);
 wcopy(g.updateStatus,L"Vela desktop",160);
 MSG msg={};while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}
 GdiplusShutdown(g.gdipToken);return 0;
}
extern "C" void WinMainCRTStartup(){ExitProcess((UINT)launch());}




