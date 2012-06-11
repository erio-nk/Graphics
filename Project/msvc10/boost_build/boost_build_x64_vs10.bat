@echo off
echo Visual Studio コマンドプロンプト(x64版)から実行して下さい。
pause

cd %BOOST_HOME%

bjam --build-dir=build\x64 --stagedir=stage\x64 --toolset=msvc-10.0 address-model=64 --without-python --without-mpi --build-type=complete link=static,shared runtime-link=static,shared release debug stage -j4

echo 終了
pause
