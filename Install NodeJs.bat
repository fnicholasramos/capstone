@echo off
:: Navigate to the user's Downloads folder
cd %USERPROFILE%\Downloads

:: Download the Node.js installer using PowerShell
echo Downloading Node.js installer...
curl -o nodejs.msi https://nodejs.org/dist/v20.18.0/node-v20.18.0-x64.msi

:: Install Node.js silently
echo Installing Node.js...
msiexec /i nodejs.msi /quiet /norestart

:: Optionally, delete the installer after installation
echo Cleaning up...
del nodejs.msi

echo Node.js installed successfully!
pause
