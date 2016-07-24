# mac install instructions

# install homebrew

# install libfreenect
brew install libfreenect

# get openni from here
http://structure.io/openni

# (verify that sensor works)
your/path/to/OpenNI-MacOSX-x64-2.2/Samples/Bin/SimpleViewer


git clone addons:
 
### ofxJSON
git clone https://github.com/jefftimesten/ofxJSON.git

### ofxProfile
git clone https://github.com/damiannz/ofxProfile


## hack: copy dylib to working dir TNM.app

cp ../../../addons/ofxOpenNI/libs/nite2/lib/osx/libNiTE2.dylib bin/Turing-Normalizing-Machine.app/Contents/MacOS/

cp ../../../addons/ofxOpenNI/libs/openni2/lib/osx/lib*.dylib bin/Turing-Normalizing-Machine.app/Contents/MacOS/

cp ../../../addons/ofxOpenNI/libs/openni2/lib/osx/OpenNI2/Drivers/lib*  bin/Turing-Normalizing-Machine.app/Contents/MacOS/

## hack: cp openni.ini, openni2/ to <binary dir>
cp -R ../../../addons/ofxOpenNI/libs/openni2/lib/osx/OpenNI*  bin/Turing-Normalizing-Machine.app/Contents/MacOS/


## hack: cp Nite.ini, and Nite2/ to ../Resources dir
cp -R ../../../addons/ofxOpenNI/libs/nite2/lib/osx/NiTE*  bin/Turing-Normalizing-Machine.app/Contents/Resources/


# copy camera settings PS1080.ini to <bin dir>

cp bin/OpenNI2/Drivers/PS1080.ini bin/Turing-Normalizing-Machine.app/Contents/MacOS/
