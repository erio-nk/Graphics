@echo off
echo Visual Studio コマンドプロンプト(x86版)から実行して下さい。
pause

cd %BOOST_HOME%

bjam --build-dir=build\Win32 --stagedir=stage\Win32 --toolset=msvc-10.0 --without-python --without-mpi --build-type=complete link=static,shared runtime-link=static,shared release debug stage -j4

echo 終了
pause
