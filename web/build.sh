
cd ..
mkdir web-build
cp web/index.html web-build/index.html
cd web-build
emcmake cmake .. -DCMAKE_PROJECT_NAME=anim_server
cmake --build . --target anim_client
