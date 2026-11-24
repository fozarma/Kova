@echo off
setlocal
pushd "%~dp0"
for /f "usebackq delims=" %%I in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSROOT=%%I"
if not defined VSROOT if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat" set "VSROOT=%ProgramFiles(x86)%\Microsoft Visual Studio\18\BuildTools"
where cl >nul 2>&1 || call "%VSROOT%\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1
python icon_resource.py || exit /b 1
for %%F in (*.def) do lib /nologo /machine:x64 /def:"%%F" /out:"%%~nF.lib" || exit /b 1
if not defined VELA_BUILD_NUMBER set "VELA_BUILD_NUMBER=2"
cl /nologo /O2 /Oi- /GS- /GR- /Zl /DVELA_BUILD_NUMBER=%VELA_BUILD_NUMBER% /DVELA_LOCAL_PREVIEW %VELA_FLAGS% /c Vela.cpp || exit /b 1
cl /nologo /O2 /GS- /GR- /Zl /c GpuMeter.cpp || exit /b 1
cl /nologo /O2 /GS- /GR- /Zl /c GameLibrary.cpp || exit /b 1
ml64 /nologo /c chkstk.asm || exit /b 1
link /nologo /out:Vela.exe /machine:x64 /subsystem:windows /entry:WinMainCRTStartup /nodefaultlib /opt:ref /opt:icf /incremental:no Vela.obj GpuMeter.obj GameLibrary.obj chkstk.obj Vela.res kernel32.lib user32.lib gdi32.lib gdiplus.lib shell32.lib advapi32.lib ole32.lib msimg32.lib comdlg32.lib magnification.lib winhttp.lib bcrypt.lib || exit /b 1
popd
