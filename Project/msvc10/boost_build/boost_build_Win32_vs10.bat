@echo off
echo Visual Studio �R�}���h�v�����v�g(x86��)������s���ĉ������B
pause

cd %BOOST_HOME%

bjam --build-dir=build\Win32 --stagedir=stage\Win32 --toolset=msvc-10.0 --without-python --without-mpi --build-type=complete link=static,shared runtime-link=static,shared release debug stage -j4

echo �I��
pause
