@echo off

choice /M "Is this XP or Server 2003? (y/n)"
goto os%errorlevel%

rem XP, Server 2003
:os1
setx /f pave-env-2003.txt PAVE_PATH /a 0,0
goto endos

rem Vista, Server 2008, Win7
:os2
setx /f pave-env-2008.txt PAVE_PATH /a 0,0
goto endos

:endos

echo Now, add the string (including %%) %%PAVE_PATH%% to your
echo   PATH environment variable.
echo Then, start a new command prompt to see PAVE_PATH
echo   and PATH.

pause
