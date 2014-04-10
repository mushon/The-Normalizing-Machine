::echo off

echo create directories
FOR %%G IN (lib, bin, include, bin\Data) DO (if not exist %%G md %%G)

echo copying OpenNI2 %OPENNI2_REDIST%
xcopy /E /Y /D "%OPENNI2_REDIST%*.*" ".\bin\"

if exist data do(
echo copy local Data folder
copy Data bin\Data
)

pause