@echo off
echo Compiler:
MathCComp.exe %1
echo.
if %ERRORLEVEL% equ 0 (
	echo --------
	echo Program being run:
	MathCRuntime.exe %1.mathbin
)