## copy openNi/Nite dylib to TNM.app dir

cp ../../../addons/ofxOpenNI/libs/nite2/lib/osx/libNiTE2.dylib bin/Turing-Normalizing-Machine.app/Contents/MacOS/
cp ../../../addons/ofxOpenNI/libs/openni2/lib/osx/lib*.dylib bin/Turing-Normalizing-Machine.app/Contents/MacOS/
cp ../../../addons/ofxOpenNI/libs/openni2/lib/osx/OpenNI2/Drivers/lib*  bin/Turing-Normalizing-Machine.app/Contents/MacOS/

## copy data & settings: openni.ini, openni2/ to <binary dir>
cp -R ../../../addons/ofxOpenNI/libs/openni2/lib/osx/OpenNI*  bin/Turing-Normalizing-Machine.app/Contents/MacOS/

## copy Nite.ini, and Nite2/ to ../Resources dir
cp -R ../../../addons/ofxOpenNI/libs/nite2/lib/osx/NiTE*  bin/Turing-Normalizing-Machine.app/Contents/Resources/



