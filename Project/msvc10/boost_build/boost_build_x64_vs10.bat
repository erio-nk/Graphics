@echo off
echo Visual Studio �R�}���h�v�����v�g(x64��)������s���ĉ������B
pause

cd %BOOST_HOME%

bjam --build-dir=build\x64 --stagedir=stage\x64 --toolset=msvc-10.0 address-model=64 --without-python --without-mpi --build-type=complete link=static,shared runtime-link=static,shared release debug stage -j4

echo �I��
pause
