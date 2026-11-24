// Settings and identifiers from NVIDIA's public NVAPI DRS interface.
// Each action loads a fresh transaction and commits only the selected setting.
DLL HINSTANCE API LoadLibraryExW(LPCWSTR,HANDLE,DWORD);
struct DriverApplication{U32 version,predefined;wchar_t name[2048],friendly[2048],launcher[2048];};
struct DriverProfile{U32 version;wchar_t name[2048];U32 gpu,predefined,apps,settings;};
struct DriverSetting{
 U32 version;wchar_t name[2048];U32 id,type,location,predefined,valid;
 U32 defaultData[1025],data[1025];
};
static_assert(sizeof(DriverSetting)==12320 && sizeof(DriverApplication)==12296 && sizeof(DriverProfile)==4116,"NVAPI DRS v1");
static const U32 driverKeys[]={0x10835002,0x1057eb71};
struct DriverApi{
 using Query=void*(*)(U32);
 HINSTANCE module;bool ready;
 int (*create)(void**),(*destroy)(void*),(*load)(void*),(*save)(void*);
 int (*find)(void*,wchar_t*,void**,DriverApplication*);
 int (*newProfile)(void*,DriverProfile*,void**),(*newApp)(void*,void*,DriverApplication*);
 int (*get)(void*,void*,U32,DriverSetting*),(*set)(void*,void*,DriverSetting*);
 int (*reset)(void*,void*,U32),(*info)(void*,void*,DriverProfile*);
 int (*base)(void*,void**),(*global)(void*,void**);
 bool connect(){
  if(ready)return true;
  if(!module)module=LoadLibraryExW(L"nvapi64.dll",nullptr,0x800);
  if(!module)return false;
  auto q=(Query)GetProcAddress(module,"nvapi_QueryInterface");if(!q)return false;
  auto init=(int(*)())q(0x0150e828);
  create=(decltype(create))q(0x0694d52e);destroy=(decltype(destroy))q(0xdad9cff8);
  load=(decltype(load))q(0x375dbd6b);save=(decltype(save))q(0xfcbc7e14);
  find=(decltype(find))q(0xeee566b2);newProfile=(decltype(newProfile))q(0xcc176068);
  newApp=(decltype(newApp))q(0x4347a9de);get=(decltype(get))q(0x73bf8338);
  set=(decltype(set))q(0x577dd202);reset=(decltype(reset))q(0x53f0381e);
  info=(decltype(info))q(0x61cd6fd6);base=(decltype(base))q(0xda8466a0);global=(decltype(global))q(0x617bff9f);
  ready=create&&destroy&&load&&save&&find&&newProfile&&newApp&&get&&set&&reset&&info&&base&&global&&init&&init()==0;
  return ready;
 }
};
static DriverApi drs={};
struct ProfileSession{void* handle;~ProfileSession(){if(handle)drs.destroy(handle);}};
static wchar_t gameExe[512],profileLabel[180];
static int overrides[2]={-1,-1};
static bool profileReady=false;
static const wchar_t* profileAction(int key=-1,int value=-1){
 profileReady=false;
 if(!drs.connect())return L"Per-game driver controls require an NVIDIA driver.";
 ProfileSession tx={};if(drs.create(&tx.handle)||drs.load(tx.handle))return L"Could not read NVIDIA profiles.";
 void* profile=nullptr;DriverApplication app={};app.version=0x10000|sizeof(app);
 int found=drs.find(tx.handle,gameExe,&profile,&app);
 if(found==-166){ // NVAPI_EXECUTABLE_NOT_FOUND
  if(key<0||value<0){profileLabel[0]=0;overrides[0]=overrides[1]=-1;profileReady=true;return nullptr;}
  DriverProfile fresh={};fresh.version=0x10000|sizeof(fresh);
  wcopy(fresh.name,L"Vela - ",2048);wappend(fresh.name,2048,gameExe);
  app={};app.version=0x10000|sizeof(app);wcopy(app.name,gameExe,2048);
  if(drs.newProfile(tx.handle,&fresh,&profile)||drs.newApp(tx.handle,profile,&app))return L"NVIDIA could not create a profile for this executable.";
 }else if(found)return L"NVIDIA could not identify this game profile.";
 void *base=nullptr,*global=nullptr;
 if(drs.base(tx.handle,&base)||drs.global(tx.handle,&global)||profile==base||profile==global)
  return L"Choose a game with its own driver profile.";
 DriverProfile details={};details.version=0x10000|sizeof(details);
 if(drs.info(tx.handle,profile,&details))return L"Could not read the game profile.";
 wcopy(profileLabel,details.name,180);
 if(details.apps>1)wappend(profileLabel,180,L" (shared profile)");
 int current[2]={-1,-1};
 for(int i=0;i<2;++i){
  DriverSetting setting={};setting.version=0x10000|sizeof(setting);
  int result=drs.get(tx.handle,profile,driverKeys[i],&setting);
  if(result && result!=-160)return L"NVIDIA could not read these settings.";
  if(!result&&!setting.location&&!setting.predefined)current[i]=(int)setting.data[0];
 }
 if(key>=0&&value!=current[key]){
  DriverSetting setting={};setting.version=0x10000|sizeof(setting);setting.id=driverKeys[key];setting.data[0]=(U32)value;
  int result=value<0?drs.reset(tx.handle,profile,setting.id):drs.set(tx.handle,profile,&setting);
  if(result||drs.save(tx.handle))return L"NVIDIA declined the change. Check driver permissions.";
  current[key]=value;
 }
 overrides[0]=current[0];overrides[1]=current[1];profileReady=true;return nullptr;
}
