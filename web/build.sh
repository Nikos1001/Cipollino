
cd ..
mkdir web-build
cp web/index.html web-build/index.html
cd web-build
emcmake cmake ..
cmake --build . --target anim_client
