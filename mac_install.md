# mac install instructions

## install homebrew

## install libfreenect
brew install libfreenect


## optional: verify that sensor works:

### get openni from here
http://structure.io/openni

### (verify that sensor works)
your/path/to/OpenNI-MacOSX-x64-2.2/Samples/Bin/SimpleViewer


# git clone addons:

cd ../../../addons 
git clone https://github.com/jefftimesten/ofxJSON.git
git clone --branch experimental https://github.com/eranws/ofxOpenNI.git # (experimental branch)
git clone https://github.com/damiannz/ofxProfile
git clone https://github.com/rezaali/ofxUI
cd -

# run script to copy libs 
./getNite.sh


# copy camera settings PS1080.ini to <bin dir>

cp bin/OpenNI2/Drivers/PS1080.ini bin/Turing-Normalizing-Machine.app/Contents/MacOS/OpenNI2/Drivers/
