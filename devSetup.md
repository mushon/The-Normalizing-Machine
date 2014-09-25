clone those addons (from github):

kylemcdonald/ofxCv
ofxOpenCv (included in oF)
eranws/ofxFaceTracker
eranws/ofxOpenNI (experimental branch)


<!--
irrelevant if cloning eranws/ofxFacetracker
rename ofxFaceTracker Tracker.cc/h to 
FTracker.cc/h (or whatever) - this solves Tracker issues with ofxCv's Tracker.cpp/h
-->

Download & Install OpenNI 2.2.x

https://dl.dropboxusercontent.com/u/3685114/OpenNI_Nite_install/OpenNI-Windows-x86-2.2.msi

And Nite
https://dl.dropboxusercontent.com/u/3685114/OpenNI_Nite_install/NiTE-Windows-x86-2.2.msi

Restart Computer (sets paths)

Copy facetracker/model/ directory to bin/data/model/ of the project

