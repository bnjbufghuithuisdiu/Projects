echo off
set LOCALHOST=%COMPUTERNAME%
set KILL_CMD="C:\PROGRA~1\ANSYSI~1\ANSYSS~1\v242\fluent/ntbin/win64/winkill.exe"

start "tell.exe" /B "C:\PROGRA~1\ANSYSI~1\ANSYSS~1\v242\fluent\ntbin\win64\tell.exe" Monish 50368 CLEANUP_EXITING
timeout /t 1
"C:\PROGRA~1\ANSYSI~1\ANSYSS~1\v242\fluent\ntbin\win64\kill.exe" tell.exe
if /i "%LOCALHOST%"=="Monish" (%KILL_CMD% 12264) 
if /i "%LOCALHOST%"=="Monish" (%KILL_CMD% 11648) 
if /i "%LOCALHOST%"=="Monish" (%KILL_CMD% 17436)
del "D:\normal_shock\cleanup-fluent-Monish-11648.bat"
