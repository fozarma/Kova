// VelaAutoUpdate.h -- native WinHTTP updater, no third-party runtime.
// A background worker checks the newest GitHub Release on app launch.
// Only assets named Vela.exe with a GitHub-supplied SHA-256 digest are accepted.
// Releases are produced by the repository's protected main-branch workflow:
//   auto-1, auto-2, ...
// Running binaries cannot be overwritten on Windows: a temporary copy of the
// *current trusted executable* waits for Vela to exit, swaps the verified update
// and relaunches Vela. Updater exits without changing anything if a check fails.
#ifndef VELA_AUTO_UPDATE_H
#define VELA_AUTO_UPDATE_H

#ifndef VELA_BUILD_NUMBER
#define VELA_BUILD_NUMBER 0
#endif

struct VelaStartupInfoW {
 DWORD cb; LPWSTR lpReserved, lpDesktop, lpTitle;
 DWORD dwX,dwY,dwXSize,dwYSize,dwXCountChars,dwYCountChars,dwFillAttribute,dwFlags;
 WORD wShowWindow,cbReserved2;BYTE* lpReserved2;
 HANDLE hStdInput,hStdOutput,hStdError;
};
struct VelaProcessInformation {HANDLE hProcess,hThread;DWORD dwProcessId,dwThreadId;};
static_assert(sizeof(VelaStartupInfoW)==104,"STARTUPINFOW ABI mismatch");
static_assert(sizeof(VelaProcessInformation)==24,"PROCESS_INFORMATION ABI mismatch");

DLL HANDLE API WinHttpOpen(LPCWSTR,DWORD,LPCWSTR,LPCWSTR,DWORD);
DLL HANDLE API WinHttpConnect(HANDLE,LPCWSTR,U16,DWORD);
DLL HANDLE API WinHttpOpenRequest(HANDLE,LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR const*,DWORD);
DLL BOOL API WinHttpSendRequest(HANDLE,LPCWSTR,DWORD,void*,DWORD,DWORD_PTR,DWORD_PTR);
DLL BOOL API WinHttpReceiveResponse(HANDLE,void*);
DLL BOOL API WinHttpReadData(HANDLE,void*,DWORD,DWORD*);
DLL BOOL API WinHttpQueryHeaders(HANDLE,DWORD,LPCWSTR,void*,DWORD*,DWORD*);
DLL BOOL API WinHttpSetTimeouts(HANDLE,int,int,int,int);
DLL BOOL API WinHttpCloseHandle(HANDLE);
DLL LONG API BCryptOpenAlgorithmProvider(HANDLE*,LPCWSTR,LPCWSTR,DWORD);
DLL LONG API BCryptCloseAlgorithmProvider(HANDLE,DWORD);
DLL LONG API BCryptCreateHash(HANDLE,HANDLE*,BYTE*,DWORD,BYTE*,DWORD,DWORD);
DLL LONG API BCryptHashData(HANDLE,BYTE*,DWORD,DWORD);
DLL LONG API BCryptFinishHash(HANDLE,BYTE*,DWORD,DWORD);
DLL LONG API BCryptDestroyHash(HANDLE);
DLL HANDLE API CreateThread(void*,U64,DWORD (API*)(void*),void*,DWORD,DWORD*);
DLL BOOL API CreateProcessW(LPCWSTR,LPWSTR,void*,void*,BOOL,DWORD,void*,LPCWSTR,VelaStartupInfoW*,VelaProcessInformation*);
DLL HANDLE API OpenProcess(DWORD,BOOL,DWORD);
DLL DWORD API WaitForSingleObject(HANDLE,DWORD);
DLL DWORD API GetCurrentProcessId();
DLL DWORD API GetTempPathW(DWORD,LPWSTR);
DLL BOOL API MoveFileExW(LPCWSTR,LPCWSTR,DWORD);
DLL LPWSTR API GetCommandLineW();
DLL LPWSTR* API CommandLineToArgvW(LPCWSTR,int*);
DLL HANDLE API LocalFree(HANDLE);

constexpr UINT WM_VELA_UPDATE = WM_APP+32;
constexpr UINT WM_VELA_UPDATE_CLOSE = WM_APP+33;
constexpr DWORD VELA_MOVE_REPLACE=1, VELA_MOVE_WRITE_THROUGH=8;
constexpr DWORD VELA_SYNCHRONIZE=0x00100000;
static const wchar_t* velaApiHost=L"api.github.com";
static const wchar_t* velaApiRoute=L"/repos/fozarma/Vela/releases/latest";

static bool velaFileExists(LPCWSTR path){HANDLE f=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);if(f==(HANDLE)-1)return false;CloseHandle(f);return true;}
static int velaHexDigit(char c){if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='f')return c-'a'+10;if(c>='A'&&c<='F')return c-'A'+10;return -1;}
static void velaIntToWide(unsigned int n,wchar_t* out){wchar_t rev[20];int j=0;do{rev[j++]=L'0'+(n%10);n/=10;}while(n&&j<19);for(int i=0;i<j;i++)out[i]=rev[j-1-i];out[j]=0;}
static bool velaGetPath(wchar_t* path,int cap){DWORD n=GetModuleFileNameW(nullptr,path,(DWORD)cap);return n>0&&n<(DWORD)cap-1;}
static bool velaStrCopy(wchar_t* dst,int cap,LPCWSTR src){if(wlen(src)>=cap)return false;wcopy(dst,src,cap);return true;}
static bool velaAppend(wchar_t* dst,int cap,LPCWSTR src){return wappend(dst,cap,src);}
static bool velaCopyHexToWide(const char*src,wchar_t*dst,int cap){int i=0;for(;src[i]&&i<cap-1;i++){if(velaHexDigit(src[i])<0)return false;dst[i]=(wchar_t)src[i];}dst[i]=0;return i==64;}
static bool velaQuoteAppend(wchar_t* dst,int cap,LPCWSTR item){
 // All application-generated paths are quoted; embedded quotes are rejected.
 for(int i=0;item[i];i++)if(item[i]==L'"')return false;
 return velaAppend(dst,cap,L"\"")&&velaAppend(dst,cap,item)&&velaAppend(dst,cap,L"\"");
}
static bool velaRun(LPCWSTR exe,LPWSTR commandLine){
 VelaStartupInfoW si={};si.cb=sizeof(si);VelaProcessInformation pi={};
 if(!CreateProcessW(exe,commandLine,nullptr,nullptr,0,0,nullptr,nullptr,&si,&pi))return false;
 CloseHandle(pi.hThread);CloseHandle(pi.hProcess);return true;
}
static bool velaIsPE64(const wchar_t*path){
 HANDLE f=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(f==(HANDLE)-1)return false;BYTE data[4096];DWORD n=0;BOOL ok=ReadFile(f,data,sizeof(data),&n,nullptr);CloseHandle(f);
 if(!ok||n<128||data[0]!='M'||data[1]!='Z')return false;
 U32 peOffset=(U32)data[60]|((U32)data[61]<<8)|((U32)data[62]<<16)|((U32)data[63]<<24);
 if(peOffset+26>=n)return false;
 return data[peOffset]=='P'&&data[peOffset+1]=='E'&&data[peOffset+2]==0&&data[peOffset+3]==0&&
        data[peOffset+4]==0x64&&data[peOffset+5]==0x86&&
        data[peOffset+24]==0x0B&&data[peOffset+25]==0x02; // PE32+ x64
}
static bool velaVerifyFile(const wchar_t* path,const char expectedHash[65]){
 if(!velaIsPE64(path))return false;
 HANDLE file=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(file==(HANDLE)-1)return false;
 HANDLE provider=nullptr,hash=nullptr;bool ok=false;BYTE digest[32]={};
 if(BCryptOpenAlgorithmProvider(&provider,L"SHA256",nullptr,0)>=0&&
    BCryptCreateHash(provider,&hash,nullptr,0,nullptr,0,0)>=0){
  BYTE buffer[65536];DWORD count=0;U64 total=0;ok=true;
  while(true){
   if(!ReadFile(file,buffer,sizeof(buffer),&count,nullptr)){ok=false;break;}
   if(!count)break;
   total+=count;if(total>128ull*1024*1024){ok=false;break;}
   if(BCryptHashData(hash,buffer,count,0)<0){ok=false;break;}
  }
  if(total<16384)ok=false;
  if(ok&&BCryptFinishHash(hash,digest,32,0)<0)ok=false;
  if(ok)for(int i=0;i<32;i++){
   int a=velaHexDigit(expectedHash[i*2]),b=velaHexDigit(expectedHash[i*2+1]);
   if(a<0||b<0||digest[i]!=(BYTE)((a<<4)|b)){ok=false;break;}
  }
 }
 if(hash)BCryptDestroyHash(hash);if(provider)BCryptCloseAlgorithmProvider(provider,0);CloseHandle(file);
 return ok;
}
static bool velaIsNumberChar(char c){return c>='0'&&c<='9';}
static int velaReleaseBuild(const char*tag){
 // Only releases from the trusted build workflow are installable. Example: auto-42.
 const char*prefix="auto-";for(int i=0;i<5;i++)if(tag[i]!=prefix[i])return -1;
 int build=0,count=0;for(int p=5;tag[p];p++){
  if(!velaIsNumberChar(tag[p])||++count>9)return -1;
  build=build*10+(tag[p]-'0');
 }
 return count?build:-1;
}
static bool velaJsonEquals(const char*start,int n,const char*value){int i=0;for(;value[i]&&i<n;i++)if(start[i]!=value[i])return false;return i==n&&value[i]==0;}
static bool velaJsonString(const char*begin,const char*end,const char*key,const char**out,int*length){
 for(const char*p=begin;p<end-4;p++){
  if(*p!='"')continue;
  const char*current=p+1;int len=0;while(current+len<end&&current[len]!='"'&&current[len]!='\\')len++;
  if(current+len>=end||current[len]!='"'){p=current+len;continue;}
  if(!velaJsonEquals(current,len,key)){p=current+len;continue;}
  const char*v=current+len+1;while(v<end&&(*v==' '||*v=='\r'||*v=='\t'||*v=='\n'))v++;
  if(v>=end||*v++!=':')continue;
  while(v<end&&(*v==' '||*v=='\r'||*v=='\t'||*v=='\n'))v++;
  if(v>=end||*v++!='"')continue;
  const char*value=v;while(v<end&&*v!='"'){if(*v=='\\')return false;v++;}
  if(v>=end)return false;*out=value;*length=(int)(v-value);return true;
 }
 return false;
}
static const char* velaFindObjectEnd(const char*start,const char*end){
 int depth=0;bool inString=false,escape=false;
 for(const char*p=start;p<end;p++){
  char ch=*p;
  if(inString){if(escape)escape=false;else if(ch=='\\')escape=true;else if(ch=='"')inString=false;continue;}
  if(ch=='"'){inString=true;continue;}
  if(ch=='{')depth++;
  if(ch=='}'&&--depth==0)return p+1;
 }
 return nullptr;
}
static bool velaParseRelease(const char*data,int bytes,char tag[48],char expected[65]){
 const char*end=data+bytes;const char*value=nullptr;int len=0;
 if(!velaJsonString(data,end,"tag_name",&value,&len)||len<6||len>35)return false;
 for(int i=0;i<len;i++)tag[i]=value[i];tag[len]=0;
 if(velaReleaseBuild(tag)<0)return false;
 const char*assets=nullptr;
 for(const char*p=data;p+8<end;p++)if(p[0]=='"'&&p[1]=='a'&&p[2]=='s'&&p[3]=='s'&&p[4]=='e'&&p[5]=='t'&&p[6]=='s'&&p[7]=='"'){
  const char*q=p+8;while(q<end&&(*q==' '||*q=='\r'||*q=='\n'||*q=='\t'))q++;
  if(q<end&&*q==':'){q++;while(q<end&&(*q==' '||*q=='\r'||*q=='\n'||*q=='\t'))q++;if(q<end&&*q=='[')assets=q+1;}
  break;
 }
 if(!assets)return false;
 for(const char*p=assets;p<end&&*p!=']';p++){
  if(*p!='{')continue;const char*finish=velaFindObjectEnd(p,end);if(!finish)return false;
  if(velaJsonString(p,finish,"name",&value,&len)&&velaJsonEquals(value,len,"Vela.exe")){
   if(!velaJsonString(p,finish,"digest",&value,&len)||len!=71)return false;
   if(!(value[0]=='s'&&value[1]=='h'&&value[2]=='a'&&value[3]=='2'&&value[4]=='5'&&value[5]=='6'&&value[6]==':'))return false;
   for(int i=0;i<64;i++){if(velaHexDigit(value[i+7])<0)return false;expected[i]=value[i+7];}
   expected[64]=0;return true;
  }
  p=finish-1;
 }
 return false;
}
static bool velaHttpsGet(const wchar_t*host,const wchar_t*route,BYTE*buffer,DWORD cap,DWORD*totalOut,const wchar_t*saveTo){
 bool success=false;HANDLE session=nullptr,connection=nullptr,request=nullptr,file=nullptr;
 *totalOut=0;
 session=WinHttpOpen(L"VelaAutoUpdate/1.0",0,nullptr,nullptr,0);
 if(!session)return false;
 WinHttpSetTimeouts(session,4000,4000,8000,20000);
 connection=WinHttpConnect(session,host,443,0);
 if(!connection)goto finished;
 request=WinHttpOpenRequest(connection,L"GET",route,nullptr,nullptr,nullptr,0x00800000);
 if(!request)goto finished;
 {
  const wchar_t*headers=L"Accept: application/vnd.github+json\r\nUser-Agent: Vela-Windows-Updater\r\nX-GitHub-Api-Version: 2022-11-28\r\n";
  if(!WinHttpSendRequest(request,headers,0xFFFFFFFF,nullptr,0,0,0)||!WinHttpReceiveResponse(request,nullptr))goto finished;
 }
 {
  DWORD status=0,size=sizeof(status);
  if(!WinHttpQueryHeaders(request,19|0x20000000,nullptr,&status,&size,nullptr)||status!=200)goto finished;
 }
 if(saveTo){file=CreateFileW(saveTo,GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);if(file==(HANDLE)-1){file=nullptr;goto finished;}}
 {
  BYTE readBuffer[16384];DWORD read=0,total=0;
  while(true){
   if(!WinHttpReadData(request,readBuffer,sizeof(readBuffer),&read))goto finished;
   if(read==0)break;
   if(total+read<total||total+read>cap)goto finished;
   if(file){DWORD n=0;if(!WriteFile(file,readBuffer,read,&n,nullptr)||n!=read)goto finished;}
   else memcpy(buffer+total,readBuffer,read);
   total+=read;
  }
  if(!file&&total<10)goto finished;
  if(!file)buffer[total]=0;
  *totalOut=total;success=true;
 }
finished:
 if(file)CloseHandle(file);
 if(request)WinHttpCloseHandle(request);
 if(connection)WinHttpCloseHandle(connection);
 if(session)WinHttpCloseHandle(session);
 if(!success&&saveTo)DeleteFileW(saveTo);
 return success;
}
static bool velaMakeDownloadRoute(const char*tag,wchar_t*route,int cap){
 if(!velaStrCopy(route,cap,L"/fozarma/Vela/releases/download/"))return false;
 wchar_t wtag[48]={};for(int i=0;tag[i]&&i<47;i++)wtag[i]=(wchar_t)tag[i];
 return velaAppend(route,cap,wtag)&&velaAppend(route,cap,L"/Vela.exe");
}
static bool velaWaitAndApply(const wchar_t*target,const wchar_t*stage,DWORD parentPid,const char expected[65]){
 // This code runs inside a temporary copy of the *original Vela executable*.
 HANDLE parent=OpenProcess(VELA_SYNCHRONIZE,0,parentPid);
 if(parent){DWORD result=WaitForSingleObject(parent,60000);CloseHandle(parent);if(result!=0)return false;}
 // The temporary copy does not possess any special privileges. Avoid overwriting
 // other file types and ALWAYS verify the staged executable once more.
 if(!velaVerifyFile(stage,expected))return false;
 wchar_t backup[640]={};if(!velaStrCopy(backup,640,target)||!velaAppend(backup,640,L".previous"))return false;
 if(!CopyFileW(target,backup,0))return false;
 if(!MoveFileExW(stage,target,VELA_MOVE_REPLACE|VELA_MOVE_WRITE_THROUGH))return false;
 wchar_t cmd[1500]=L"";if(!velaQuoteAppend(cmd,1500,target))return false;
 if(!velaRun(target,cmd)){
  MoveFileExW(backup,target,VELA_MOVE_REPLACE|VELA_MOVE_WRITE_THROUGH);
  return false;
 }
 return true;
}
static int velaMaybeUpdaterProcess(){
 int argc=0;LPWSTR*argv=CommandLineToArgvW(GetCommandLineW(),&argc);
 if(!argv)return -1;
 bool helper=(argc>=2&&argv[1]&&argv[1][0]==L'-'&&argv[1][1]==L'-');
 if(!helper){LocalFree(argv);return -1;}
 const wchar_t*command=L"--vela-apply";int i=0;while(command[i]&&argv[1][i]==command[i])i++;
 if(command[i]||argv[1][i]||argc!=6){LocalFree(argv);return -1;}
 const wchar_t*target=argv[2],*stage=argv[3];
 // Limit the helper to an executable literally named Vela.exe.
 int n=wlen(target);const wchar_t*ending=L"Vela.exe";int endingLen=8;
 bool safe=n>endingLen&&(target[n-endingLen-1]==L'\\'||target[n-endingLen-1]==L'/');
 for(int z=0;safe&&z<endingLen;z++){
  wchar_t a=target[n-endingLen+z],b=ending[z];if(a>=L'A'&&a<=L'Z')a+=32;if(b>=L'A'&&b<=L'Z')b+=32;
  if(a!=b)safe=false;
 }
 DWORD pid=0;for(int j=0;argv[4][j]&&j<11;j++){
  wchar_t c=argv[4][j];if(c<L'0'||c>L'9'){safe=false;break;}pid=pid*10+(DWORD)(c-L'0');
 }
 char hash[65]={};for(int j=0;j<64&&argv[5][j];j++)hash[j]=(char)argv[5][j];
 if(wlen(argv[5])!=64||!pid)safe=false;
 bool success=safe&&velaWaitAndApply(target,stage,pid,hash);
 if(!success)MessageBoxW(nullptr,L"Vela could not finish this update. Your existing Vela.exe was not intentionally replaced. Download the latest Vela.exe from GitHub Releases if needed.",L"Vela updater",0x30);
 LocalFree(argv);return success?0:1;
}
static DWORD API velaUpdateWorker(void* hwndArg){
 HWND hwnd=(HWND)hwndArg;PostMessageW(hwnd,WM_VELA_UPDATE,1,0);
 static BYTE response[98304];DWORD bytes=0;
 if(!velaHttpsGet(velaApiHost,velaApiRoute,response,98303,&bytes,nullptr)){
  PostMessageW(hwnd,WM_VELA_UPDATE,4,0);return 0;
 }
 char tag[48]={},hash[65]={};if(!velaParseRelease((const char*)response,(int)bytes,tag,hash)){
  PostMessageW(hwnd,WM_VELA_UPDATE,4,0);return 0;
 }
 int latest=velaReleaseBuild(tag);if(latest<=VELA_BUILD_NUMBER){PostMessageW(hwnd,WM_VELA_UPDATE,3,0);return 0;}
 PostMessageW(hwnd,WM_VELA_UPDATE,2,0);
 wchar_t target[640]={},staged[640]={},route[256]={};
 if(!velaGetPath(target,640)||!velaStrCopy(staged,640,target)||!velaAppend(staged,640,L".download")||
    !velaMakeDownloadRoute(tag,route,256)){
   PostMessageW(hwnd,WM_VELA_UPDATE,6,0);return 0;
 }
 DWORD size=0;
 if(!velaHttpsGet(L"github.com",route,nullptr,128*1024*1024,&size,staged)){
  PostMessageW(hwnd,WM_VELA_UPDATE,6,0);return 0;
 }
 if(!velaVerifyFile(staged,hash)){
  DeleteFileW(staged);PostMessageW(hwnd,WM_VELA_UPDATE,7,0);return 0;
 }
 wchar_t tempDir[512]={};DWORD n=GetTempPathW(512,tempDir);
 if(!n||n>460){DeleteFileW(staged);PostMessageW(hwnd,WM_VELA_UPDATE,6,0);return 0;}
 DWORD pid=GetCurrentProcessId();wchar_t pidText[24]={};velaIntToWide(pid,pidText);
 wchar_t helper[640]={};velaStrCopy(helper,640,tempDir);
 if(!velaAppend(helper,640,L"Vela-Update-Helper-")||!velaAppend(helper,640,pidText)||!velaAppend(helper,640,L".exe")||
    !CopyFileW(target,helper,0)){
  DeleteFileW(staged);PostMessageW(hwnd,WM_VELA_UPDATE,6,0);return 0;
 }
 wchar_t cmd[1900]=L"";
 if(!velaQuoteAppend(cmd,1900,helper)||!velaAppend(cmd,1900,L" --vela-apply ")||
    !velaQuoteAppend(cmd,1900,target)||!velaAppend(cmd,1900,L" ")||
    !velaQuoteAppend(cmd,1900,staged)||!velaAppend(cmd,1900,L" ")||
    !velaAppend(cmd,1900,pidText)||!velaAppend(cmd,1900,L" ")){
  DeleteFileW(staged);DeleteFileW(helper);PostMessageW(hwnd,WM_VELA_UPDATE,6,0);return 0;
 }
 wchar_t whash[65]={};for(int i=0;i<64;i++)whash[i]=(wchar_t)hash[i];
 velaAppend(cmd,1900,whash);
 if(!velaRun(helper,cmd)){
  DeleteFileW(staged);DeleteFileW(helper);PostMessageW(hwnd,WM_VELA_UPDATE,6,0);return 0;
 }
 PostMessageW(hwnd,WM_VELA_UPDATE,5,0);
 PostMessageW(hwnd,WM_VELA_UPDATE_CLOSE,0,0);
 return 0;
}
static bool velaStartUpdateCheck(HWND hwnd){
 DWORD tid=0;HANDLE h=CreateThread(nullptr,0,velaUpdateWorker,hwnd,0,&tid);
 if(!h)return false;CloseHandle(h);return true;
}
#endif
