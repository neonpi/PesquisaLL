@echo off
g++ -O3 .\instance_factory\*.cpp *.cpp -o a.exe
if errorlevel 1 (
    echo Houve erros durante a compilação.
    pause
    exit /b 1
)
.\a.exe
pause