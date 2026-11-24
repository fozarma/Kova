#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include "GameLibrary.h"


static bool join(wchar_t* out,int size,const wchar_t* left,const wchar_t* right){
 if(lstrlenW(left)+lstrlenW(right)>=size)return false;
 lstrcpyW(out,left);lstrcatW(out,right);return true;
}
static const wchar_t* leaf(const wchar_t* path){
 const wchar_t* name=path;for(;*path;++path)if(*path==L'/'||*path==L'\\')name=path+1;return name;
}
static bool manualType(const wchar_t* path){
 int n=lstrlenW(path);
 return n>4&&(!lstrcmpiW(path+n-4,L".exe")||!lstrcmpiW(path+n-4,L".lnk")||!lstrcmpiW(path+n-4,L".url"));
}
static bool shortcutFolder(wchar_t* out){
 wchar_t local[768];DWORD n=GetEnvironmentVariableW(L"LOCALAPPDATA",local,768);
 if(!n||n>=768||!join(out,1024,local,L"\\Kova"))return false;
 CreateDirectoryW(out,nullptr);lstrcatW(out,L"\\Games");CreateDirectoryW(out,nullptr);lstrcatW(out,L"\\");return true;
}
extern "C" bool storeGame(const wchar_t* path,bool remove){
 if(!manualType(path)||lstrlenW(path)>=1024)return false;
 wchar_t file[1024],folded[1024];if(!shortcutFolder(file))return false;
 lstrcpyW(folded,path);CharUpperBuffW(folded,lstrlenW(folded));
 unsigned long long hash=14695981039346656037ull;
 for(const wchar_t* c=folded;*c;++c)hash=(hash^*c)*1099511628211ull;
 wchar_t suffix[24]=L"0000000000000000.game";
 for(int i=15;i>=0;--i){suffix[i]=L"0123456789abcdef"[hash&15];hash>>=4;}
 lstrcatW(file,suffix);
 if(remove)return DeleteFileW(file)!=0;
 wchar_t tmp[1024],dir[1024];if(!shortcutFolder(dir)||lstrlenW(dir)>MAX_PATH-14||!GetTempFileNameW(dir,L"vlg",0,tmp))return false;
 HANDLE f=CreateFileW(tmp,GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,0,nullptr);
 DWORD bytes=(lstrlenW(path)+1)*sizeof(wchar_t),done=0;
 bool ok=f!=INVALID_HANDLE_VALUE&&WriteFile(f,path,bytes,&done,nullptr)&&done==bytes;
 if(f!=INVALID_HANDLE_VALUE)CloseHandle(f);
 if(ok&&MoveFileExW(tmp,file,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))return true;
 DeleteFileW(tmp);return false;
}
static void add(GameList* list,const wchar_t* name,const wchar_t* target,int source,const wchar_t* account=L""){
 if(!name[0]||!target[0]||lstrlenW(target)>=1024)return;
 for(int i=0;i<list->count;++i)if(!lstrcmpiW(list->games[i].target,target))return;
 if(list->count==list->capacity){
  int capacity=list->capacity?list->capacity*2:16;
  SIZE_T bytes=(SIZE_T)capacity*sizeof(LibraryGame);HANDLE heap=GetProcessHeap();
  void* data=list->games?HeapReAlloc(heap,0,list->games,bytes):HeapAlloc(heap,0,bytes);
  if(!data)return;list->games=(LibraryGame*)data;list->capacity=capacity;
 }
 int i=list->count++;
 while(i>0 && lstrcmpiW(list->games[i-1].name,name)>0){list->games[i]=list->games[i-1];--i;}
 auto& game=list->games[i];lstrcpynW(game.name,name,260);lstrcpyW(game.target,target);game.source=source;lstrcpynW(game.lastAccount,account,32);
}
// Launcher metadata is local UTF-8. Only quoted string keys and values are consumed.
static char* readFile(const wchar_t* path){
 HANDLE f=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,0,nullptr);
 if(f==INVALID_HANDLE_VALUE)return nullptr;
 DWORD n=GetFileSize(f,nullptr),got=0;char* data=nullptr;
 if(n&&n<=4*1024*1024){
  data=(char*)HeapAlloc(GetProcessHeap(),0,n+1);
  if(data){if(!ReadFile(f,data,n,&got,nullptr)){HeapFree(GetProcessHeap(),0,data);data=nullptr;}else data[got]=0;}
 }
 CloseHandle(f);return data;
}
static const char* space(const char* p){while(*p && (unsigned char)*p<=32)++p;return p;}
static const char* field(const char* p,const char* key,bool json){
 for(;*p;++p){
  if(*p!='"')continue;
  const char* a=++p;const char* b=key;while(*a&&*b&&*a==*b){++a;++b;}
  if(!*b&&*a=='"'){
   const char* next=space(a+1);
   if(!json)return next;
   if(*next==':')return space(next+1);
  }
  while(*p&&*p!='"'){if(*p=='\\'&&p[1])++p;++p;}if(!*p)break;
 }
 return nullptr;
}
static const char* quoted(const char* p,wchar_t* out,int size){
 if(!p||*p!='"')return nullptr;
 char buf[4096];int n=0;
 for(++p;*p&&*p!='"';++p){
  if(*p=='\\'&&p[1])++p;
  if(n==4095)return nullptr;buf[n++]=*p;
 }
 if(*p!='"')return nullptr;buf[n]=0;
 return MultiByteToWideChar(CP_UTF8,0,buf,-1,out,size)?p+1:nullptr;
}
template<class F> static void eachFile(const wchar_t* folder,const wchar_t* mask,F visit){
 wchar_t pattern[1024];if(!join(pattern,1024,folder,mask))return;
 WIN32_FIND_DATAW file;HANDLE h=FindFirstFileW(pattern,&file);if(h==INVALID_HANDLE_VALUE)return;
 do{wchar_t full[1024];if(!(file.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)&&join(full,1024,folder,file.cFileName))visit(full);}while(FindNextFileW(h,&file));FindClose(h);
}
static void manual(GameList* list){
 wchar_t folder[1024];if(!shortcutFolder(folder))return;
 eachFile(folder,L"*.game",[&](const wchar_t* f){
  HANDLE h=CreateFileW(f,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,0,nullptr);
  if(h==INVALID_HANDLE_VALUE)return;
  wchar_t path[1024]={};DWORD size=GetFileSize(h,nullptr),got=0;
  bool ok=size>=4&&size<=sizeof(path)&&!(size%2)&&ReadFile(h,path,size,&got,nullptr)&&size==got&&path[size/2-1]==0;
  CloseHandle(h);if(!ok||!manualType(path))return;
  wchar_t name[260];lstrcpynW(name,leaf(path),260);int n=lstrlenW(name);if(n>4)name[n-4]=0;add(list,name,path,0);
 });
}
static void steamFolder(GameList* list,const wchar_t* root){
 wchar_t folder[1024];if(!join(folder,1024,root,L"\\steamapps\\"))return;
 eachFile(folder,L"appmanifest_*.acf",[&](const wchar_t* path){
  const wchar_t* filename=leaf(path);
  // Steam installs this shared runtime as an app manifest alongside playable games.
  if(!lstrcmpiW(filename,L"appmanifest_228980.acf"))return;
  const wchar_t* id=filename+12;int n=0;
  while(id[n]>='0'&&id[n]<='9')++n;if(!n||n>10||lstrcmpiW(id+n,L".acf"))return;
  char* data=readFile(path);if(!data)return;
  wchar_t name[260],uri[64]=L"steam://rungameid/";
  if(quoted(field(data,"name",false),name,260)){int k=lstrlenW(uri);for(int i=0;i<n;++i)uri[k++]=id[i];uri[k]=0;wchar_t owner[32]={};quoted(field(data,"LastOwner",false),owner,32);add(list,name,uri,1,owner);}
  HeapFree(GetProcessHeap(),0,data);
 });
}
static void discoverSteam(GameList* list){
 wchar_t root[1024]={};DWORD bytes=sizeof(root);
 if(RegGetValueW(HKEY_CURRENT_USER,L"Software\\Valve\\Steam",L"SteamPath",RRF_RT_REG_SZ,nullptr,root,&bytes)){
  wchar_t base[512];DWORD n=GetEnvironmentVariableW(L"ProgramFiles(x86)",base,512);
  if(!n||n>=512||!join(root,1024,base,L"\\Steam"))return;
 }
 steamFolder(list,root);wchar_t path[1024];if(!join(path,1024,root,L"\\steamapps\\libraryfolders.vdf"))return;
 char* data=readFile(path);if(!data)return;
 const char* cursor=data;while((cursor=field(cursor,"path",false))){
  wchar_t extra[1024];cursor=quoted(cursor,extra,1024);if(!cursor)break;if(lstrcmpiW(extra,root))steamFolder(list,extra);
 }
 HeapFree(GetProcessHeap(),0,data);
}
static void discoverEpic(GameList* list){
 wchar_t base[512],folder[1024];DWORD n=GetEnvironmentVariableW(L"ProgramData",base,512);
 if(!n||n>=512||!join(folder,1024,base,L"\\Epic\\EpicGamesLauncher\\Data\\Manifests\\"))return;
 eachFile(folder,L"*.item",[&](const wchar_t* path){
  char* data=readFile(path);if(!data)return;
  wchar_t id[260],name[260],launch[512];const char* incomplete=field(data,"bIsIncompleteInstall",true);
  if((!incomplete||*incomplete!='t')&&quoted(field(data,"LaunchExecutable",true),launch,512)&&launch[0]&&
     quoted(field(data,"DisplayName",true),name,260)&&quoted(field(data,"AppName",true),id,260)){
   bool safe=id[0]!=0;for(auto* c=id;*c;++c)if(!((*c>='a'&&*c<='z')||(*c>='A'&&*c<='Z')||(*c>='0'&&*c<='9')||*c=='_'||*c=='-'||*c=='.'))safe=false;
   if(safe){wchar_t uri[512]=L"com.epicgames.launcher://apps/";lstrcatW(uri,id);lstrcatW(uri,L"?action=launch&silent=true");add(list,name,uri,2);}
  }
  HeapFree(GetProcessHeap(),0,data);
 });
}
extern "C" void freeGames(GameList* list){
 if(list){if(list->games)HeapFree(GetProcessHeap(),0,list->games);HeapFree(GetProcessHeap(),0,list);}
}
static DWORD WINAPI worker(void* window){
 auto* list=(GameList*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(GameList));
 if(list){manual(list);discoverSteam(list);discoverEpic(list);}
 if(!PostMessageW((HWND)window,libraryLoaded,0,(LPARAM)list))freeGames(list);return 0;
}
extern "C" bool scanGames(void* window){
 HANDLE h=CreateThread(nullptr,0,worker,window,0,nullptr);if(!h)return false;CloseHandle(h);return true;
}
extern "C" bool launchGame(const LibraryGame* game,void* window){
 wchar_t folder[1024]={};
 if(!game->source){
  if(!manualType(game->target)||GetFileAttributesW(game->target)==INVALID_FILE_ATTRIBUTES)return false;
  lstrcpyW(folder,game->target);folder[leaf(game->target)-game->target]=0;
 }
 return (INT_PTR)ShellExecuteW((HWND)window,L"open",game->target,nullptr,*folder?folder:nullptr,SW_SHOWNORMAL)>32;
}
// Steam installation metadata identifies the last recorded account. A user's explicit
// assignment takes priority, including for games available through Steam Families.
static constexpr unsigned long long steamIdBase=76561197960265728ull;
static DWORD localAccountId(const wchar_t* id){
 if(lstrlenW(id)!=17)return 0;
 unsigned long long value=0;
 for(int i=0;i<17;++i){if(id[i]<L'0'||id[i]>L'9')return 0;value=value*10+(id[i]-L'0');}
 return value>steamIdBase&&value-steamIdBase<=MAXDWORD?(DWORD)(value-steamIdBase):0;
}
static DWORD steamNumber(const wchar_t* section,const wchar_t* key){
 DWORD value=0,size=sizeof(value);
 RegGetValueW(HKEY_CURRENT_USER,section,key,RRF_RT_REG_DWORD,nullptr,&value,&size);return value;
}
static DWORD currentSteamAccount(){return steamNumber(L"Software\\Valve\\Steam\\ActiveProcess",L"ActiveUser");}
static bool accountFile(wchar_t* path){
 wchar_t folder[1024];return shortcutFolder(folder)&&join(path,1024,folder,L"steam-accounts.ini");
}
extern "C" bool assignActiveAccount(const LibraryGame* game){
 DWORD active=currentSteamAccount();wchar_t path[1024],id[18]={};
 if(game->source!=1||!active||!accountFile(path))return false;
 unsigned long long value=steamIdBase+active;
 for(int i=16;i>=0;--i){id[i]=L'0'+value%10;value/=10;}
 return WritePrivateProfileStringW(L"Accounts",leaf(game->target),id,path)!=0;
}
static bool steamRoot(wchar_t* root){
 DWORD bytes=1024*sizeof(wchar_t);
 if(!RegGetValueW(HKEY_CURRENT_USER,L"Software\\Valve\\Steam",L"SteamPath",RRF_RT_REG_SZ,nullptr,root,&bytes))return true;
 wchar_t base[512];DWORD n=GetEnvironmentVariableW(L"ProgramFiles(x86)",base,512);
 return n&&n<512&&join(root,1024,base,L"\\Steam");
}
static bool rememberedName(char* data,const wchar_t* id,wchar_t* name){
 char key[18];if(!localAccountId(id))return false;
 for(int i=0;i<18;++i)key[i]=(char)id[i];
 const char* object=field(data,key,false);if(!object||*object!='{')return false;
 char* end=(char*)object+1;bool inString=false;
 for(;*end;++end){
  if(inString&&*end=='\\'&&end[1]){++end;continue;}
  if(*end=='"')inString=!inString;
  if(!inString&&*end=='}')break;
 }
 if(!*end)return false;
 char saved=*end;*end=0;
 bool ok=quoted(field(object,"AccountName",false),name,128)!=nullptr;
 *end=saved;
 if(!ok||!*name)return false;
 for(auto* c=name;*c;++c)if(!((*c>=L'a'&&*c<=L'z')||(*c>=L'A'&&*c<=L'Z')||(*c>=L'0'&&*c<=L'9')||*c==L'_'||*c==L'-'||*c==L'.'||*c==L'@'))return false;
 return true;
}
struct SteamRequest{LibraryGame game;HWND window;};
static volatile LONG launchBusy;
static bool steamCommand(HWND window,const wchar_t* exe,const wchar_t* args){
 return (INT_PTR)ShellExecuteW(window,L"open",exe,args,nullptr,SW_SHOWNORMAL)>32;
}
static const wchar_t* runSteam(SteamRequest& request){
 auto& game=request.game;
 wchar_t id[32]={},path[1024],root[1024],exe[1024],name[128]={};
 if(accountFile(path))GetPrivateProfileStringW(L"Accounts",leaf(game.target),L"",id,32,path);
 if(!id[0])lstrcpynW(id,game.lastAccount,32);
 DWORD desired=localAccountId(id);
 if(!desired||currentSteamAccount()==desired)
  return launchGame(&game,request.window)?L"Steam launch requested.":L"Could not open Steam.";
 if(!steamRoot(root)||!join(exe,1024,root,L"\\steam.exe")||!join(path,1024,root,L"\\config\\loginusers.vdf"))
  return L"Steam installation could not be found.";
 char* metadata=readFile(path);bool remembered=metadata&&rememberedName(metadata,id,name);
 if(metadata)HeapFree(GetProcessHeap(),0,metadata);
 if(!remembered)return L"Account unavailable. Sign in to the owning account and choose Use current account.";
 if(steamNumber(L"Software\\Valve\\Steam",L"RunningAppID"))
  return L"Close the running Steam game before switching accounts.";
 DWORD pid=steamNumber(L"Software\\Valve\\Steam\\ActiveProcess",L"pid");
 if(pid){
  HANDLE process=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
  if(process){
   wchar_t actual[1024];DWORD chars=1024;
   bool matches=QueryFullProcessImageNameW(process,0,actual,&chars)!=0;
   for(wchar_t* c=exe;*c;++c)if(*c==L'/')*c=L'\\';
   if(matches&&!lstrcmpiW(actual,exe)){
    bool exited=steamCommand(request.window,exe,L"-shutdown")&&WaitForSingleObject(process,45000)==WAIT_OBJECT_0;
    CloseHandle(process);if(!exited)return L"Steam is still busy. Finish any Steam prompts and try again.";
   }else CloseHandle(process);
  }
 }
 wchar_t args[160];if(!join(args,160,L"-login ",name)||!steamCommand(request.window,exe,args))return L"Could not start Steam for this account.";
 for(int i=0;i<120;++i){
  if(currentSteamAccount()==desired)return launchGame(&game,request.window)?L"Game launch requested on the selected Steam account.":L"Account selected. Steam could not open the game.";
  Sleep(1000);
 }
 return L"Steam sign-in timed out. Finish signing in and click Launch again.";
}
static DWORD WINAPI steamWorker(void* arg){
 auto* request=(SteamRequest*)arg;const wchar_t* message=runSteam(*request);
 PostMessageW(request->window,steamLaunchFinished,0,(LPARAM)message);
 HeapFree(GetProcessHeap(),0,request);InterlockedExchange(&launchBusy,0);return 0;
}
extern "C" const wchar_t* requestLibraryLaunch(const LibraryGame* game,void* window){
 if(game->source!=1)return launchGame(game,window)?L"Launch requested.":L"Could not open the game. Check its shortcut or launcher.";
 if(InterlockedCompareExchange(&launchBusy,1,0))return L"A Steam launch is in progress. Check Steam for sign-in prompts.";
 auto* req=(SteamRequest*)HeapAlloc(GetProcessHeap(),0,sizeof(SteamRequest));
 if(req){
  req->game=*game;req->window=(HWND)window;
  HANDLE h=CreateThread(nullptr,0,steamWorker,req,0,nullptr);
  if(h){CloseHandle(h);return L"Checking the Steam account. Complete any sign-in prompt in Steam.";}
  HeapFree(GetProcessHeap(),0,req);
 }
 InterlockedExchange(&launchBusy,0);return L"Could not start the Steam launch.";
}
