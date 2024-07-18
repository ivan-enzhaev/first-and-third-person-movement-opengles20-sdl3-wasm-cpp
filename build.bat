cd build\web
mingw32-make

cd ..\..
set current_dir=%~dp0
copy "%current_dir%build\web\sdl-min.wasm" "%current_dir%public\js"
copy "%current_dir%build\web\sdl-min.js" "%current_dir%public\js"
