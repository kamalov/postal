@rem C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.41.34120\bin\HostX64\x64;
@rem D:\lazarus\fpc\3.2.2\bin\x86_64-win64\fpc.exe _out.pas

@setlocal
@SET PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.41.34120\bin\HostX64\x64;C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\VC\VCPackages;C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\TestWindow;C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\TeamFoundation\Team Explorer;C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\bin\Roslyn;C:\Program Files\Microsoft Visual Studio\2022\Community\Team Tools\DiagnosticsHub\Collector;C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\\x64;C:\Program Files (x86)\Windows Kits\10\bin\\x64;C:\Program Files\Microsoft Visual Studio\2022\Community\\MSBuild\Current\Bin\amd64;C:\Windows\Microsoft.NET\Framework64\v4.0.30319;C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\;C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\;%PATH%;C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\VC\Linux\bin\ConnectionManagerExe;C:\Program Files\Microsoft Visual Studio\2022\Community\VC\vcpkg;
@SET INCLUDE=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.41.34120\include;C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\VS\include;C:\Program Files (x86)\Windows Kits\10\include\10.0.22621.0\ucrt;C:\Program Files (x86)\Windows Kits\10\\include\10.0.22621.0\\um;C:\Program Files (x86)\Windows Kits\10\\include\10.0.22621.0\\shared;C:\Program Files (x86)\Windows Kits\10\\include\10.0.22621.0\\winrt;C:\Program Files (x86)\Windows Kits\10\\include\10.0.22621.0\\cppwinrt
@SET LIB=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.41.34120\lib\x64;C:\Program Files (x86)\Windows Kits\10\lib\10.0.22621.0\ucrt\x64;C:\Program Files (x86)\Windows Kits\10\\lib\10.0.22621.0\\um\x64
@SET LIBPATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.41.34120\lib\x64;C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.41.34120\lib\x86\store\references;C:\Program Files (x86)\Windows Kits\10\UnionMetadata\10.0.22621.0;C:\Program Files (x86)\Windows Kits\10\References\10.0.22621.0;C:\Windows\Microsoft.NET\Framework64\v4.0.30319

cl.exe generated.cpp /EHsc

generated.exe 

@del generated.exe > nul
@del generated.obj > nul