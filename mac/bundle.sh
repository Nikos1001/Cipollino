
# args: path to mac 11.0 sdk

cd ..
mkdir mac-build
cd mac-build
mkdir build
cd build
cmake ../.. -DCMAKE_OSX_SYSROOT=$1 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 --config Release
make
cd ..
mkdir Cipollino.app
mkdir Cipollino.app/Contents
mkdir Cipollino.app/Contents/MacOS
mkdir Cipollino.app/Contents/Resources
cp build/anim_client Cipollino.app/Contents/MacOS/Cipollino
cp ../mac/Info.plist Cipollino.app/Contents/Info.plist
cp ../res/icon.icns Cipollino.app/Contents/Resources/icon.icns
