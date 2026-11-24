@echo off
setlocal EnableExtensions
pushd "%~dp0"

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
  echo Visual Studio C++ Build Tools not found.
  exit /b 1
)
set "VSROOT="
for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSROOT=%%I"
if not defined VSROOT (
  echo Visual Studio C++ toolchain not found.
  exit /b 1
)
call "%VSROOT%\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1

if exist "%ProgramFiles%\LLVM\bin\clang-cl.exe" set "PATH=%ProgramFiles%\LLVM\bin;%PATH%"
if exist "%VSROOT%\VC\Tools\Llvm\x64\bin\clang-cl.exe" set "PATH=%VSROOT%\VC\Tools\Llvm\x64\bin;%PATH%"
where clang-cl >nul 2>&1 || (echo clang-cl not found. Install LLVM for Visual Studio. & exit /b 1)
where lld-link >nul 2>&1 || (echo lld-link not found. Install LLVM linker. & exit /b 1)
where clang >nul 2>&1 || (echo clang not found. Install LLVM clang.exe. & exit /b 1)
python build_icon.py || exit /b 1
python icon_resource.py || exit /b 1
for %%F in (*.def) do lib /nologo /machine:x64 /def:"%%F" /out:"%%~nF.lib" || exit /b 1
if not defined VELA_BUILD_NUMBER set "VELA_BUILD_NUMBER=2"
echo Building Vela build %VELA_BUILD_NUMBER%
clang-cl --target=x86_64-pc-windows-msvc /O2 /GS- /GR- /EHs- /Zl /DVELA_BUILD_NUMBER=%VELA_BUILD_NUMBER% /DVELA_LOCAL_PREVIEW %VELA_FLAGS% /c /Fo:Vela.obj Vela.cpp || exit /b 1
clang-cl --target=x86_64-pc-windows-msvc /nologo /O2 /GS- /GR- /Zl /c GpuMeter.cpp || exit /b 1
clang-cl --target=x86_64-pc-windows-msvc /nologo /O2 /GS- /GR- /Zl /c GameLibrary.cpp || exit /b 1
clang --target=x86_64-pc-windows-msvc -c chkstk.s -o chkstk.obj || exit /b 1
lld-link /out:Vela.exe /machine:x64 /subsystem:windows /entry:WinMainCRTStartup /nodefaultlib /opt:ref /opt:icf /incremental:no Vela.obj GpuMeter.obj GameLibrary.obj chkstk.obj Vela.res kernel32.lib user32.lib gdi32.lib gdiplus.lib shell32.lib advapi32.lib ole32.lib msimg32.lib comdlg32.lib magnification.lib winhttp.lib bcrypt.lib || exit /b 1
certutil -hashfile Vela.exe SHA256
popd
