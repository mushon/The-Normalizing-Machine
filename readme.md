# install

download of-0.10
https://openframeworks.cc/versions/v0.10.1/of_v0.10.1_osx_release.zip


This script pulls only shallow branch (15M vs. 417M full repo)
builds and run the app (on mac :)
```
cd of-0.10
cd apps/myApps

git clone --single-branch --branch mac_fix --depth 1 git@github.com:mushon/The-Normalizing-Machine.git tnm-thin
cd tnm-thin
make
make RunRelease
```
