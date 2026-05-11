@echo off
chcp 65001 > nul
title Installing WinDivert...

PUSHD "%~dp0"

if not exist "%~dp0external" (
    mkdir "%~dp0external"
)

REM Enter WinDivert version here
set "WINDIVERT_VER=2.2.2"

set "WINDIVERT_FILENAME=WinDivert-%WINDIVERT_VER%-A"

if exist "%~dp0external\%WINDIVERT_FILENAME%.zip" (
	goto :UNPACKER
)
if exist "%~dp0external\WinDivert" if exist "%~dp0external\WinDivert\include\windivert.h" (
    goto :END
)
if not exist "%~dp0external\WinDivert\include\windivert.h" (
    goto :DOWNLOADER
)
exit

:DOWNLOADER
set "WINDIVERT_DOWNLOAD_PATH=https://github.com/basil00/WinDivert/releases/download/v%WINDIVERT_VER%/%WINDIVERT_FILENAME%.zip"
set "FILEPATH=%~dp0external\"

if "%WINDIVERT_DOWNLOAD_PATH%"=="" (
    exit /b 1
) else if not exist "%FILEPATH%" (
    exit /b 1
)

for %%F in ("%WINDIVERT_DOWNLOAD_PATH%") do set FILE_NAME=%%~nxF

:CURL
cls
color 1
echo Preparing...

if not exist "%Windir%\System32\curl.exe" (
	color c
	echo Error!!! CURL not found. Trying through BITS...
	pause
	goto :BITS
)

echo Downloading required software (may take some time)...

curl -k -L -o "%FILEPATH%\%FILE_NAME%" "%WINDIVERT_DOWNLOAD_PATH%"

if not exist "%FILEPATH%\%FILE_NAME%" (
	color c
	echo Error!!! Unable to download file.
	pause
	exit /b 1
)

ping -n 3 127.0.0.1 > nul
color A
echo.
echo Download... Completed!
goto :UNPACKER

:BITS
cls
color 1
echo Preparing...

bitsadmin /reset > nul
bitsadmin /create progdwnl > nul
bitsadmin /setpriority progdwnl HIGH > nul
bitsadmin /setproxysettings progdwnl NO_PROXY > nul

echo Downloading required software (may take some time)...
bitsadmin /transfer progdwnl "%WINDIVERT_DOWNLOAD_PATH%" "%FILEPATH%\%FILE_NAME%" > nul

if exist "%FILEPATH%\%FILE_NAME%" goto :UNPACKER

echo Error!!! Unable to download file. Trying through default browser...
pause
bitsadmin /cancel progdwnl > nul
goto :BROW

:BROW
cls
color 1
echo Downloading required software (may take some time)...
start "" "%WINDIVERT_DOWNLOAD_PATH%"
echo Wait until the download is completed in the browser...
pause
if exist "%FILEPATH%\%FILE_NAME%" (
	goto :UNPACKER
)
echo.
echo Attempting to move %FILE_NAME% to the BypassKit folder...
if exist "%UserProfile%\Downloads\%FILE_NAME%" (
	move /Y "%UserProfile%\Downloads\%FILE_NAME%" "%FILEPATH%" > nul
	color 2
	echo Moving... Completed!
	goto :UNPACKER
)
if not exist "%UserProfile%\Downloads\%FILE_NAME%" (
	color c
	echo Error!!! A non-standard download folder is used or the antivirus mistakenly deleted the file.
	echo Restore and move %FILE_NAME% to the BypassKit folder manually...
	pause
)

:UNPACKER
cls

color 1
echo.
echo Extracting archive...
echo.
cscript unzip.vbs "%~dp0external\%WINDIVERT_FILENAME%.zip" "%~dp0external\"
del /F /Q "%~dp0external\%WINDIVERT_FILENAME%.zip" > nul
echo.
color 2
echo Extracting archive... Completed!
echo.

:WAIT_UNPACK
if not exist "%~dp0external\%WINDIVERT_FILENAME%" (
    timeout /t 1 > nul
    goto :WAIT_UNPACK
)
ren "%~dp0external\%WINDIVERT_FILENAME%" "WinDivert"

ping -n 6 127.0.0.1 > nul

:END
exit