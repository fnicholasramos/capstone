@echo off
:: Change to a temporary directory
cd %USERPROFILE%\Downloads

:: Download Node.js installer using curl
echo Downloading Node.js installer...
curl -o nodejs.msi https://nodejs.org/dist/latest/node-vXX.X.X-x64.msi

:: Install Node.js silently
echo Installing Node.js...
msiexec /i nodejs.msi /quiet /norestart

:: Delete installer file after installation
echo Cleaning up...
del nodejs.msi

echo Node.js installed successfully!
pause
