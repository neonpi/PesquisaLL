@echo off
g++ -O3 *.cpp -o a.exe
if errorlevel 1 (
    echo Houve erros durante a compilação.
    pause
    exit /b 1
)
.\a.exe
pause