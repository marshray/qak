@echo off
set errorlevel=&rem
cmd /c exit 1
setlocal enableextensions enabledelayedexpansion
if %errorlevel% neq 0 (echo Couldn't enable cmd extensions.& goto :error)

pushd ..\..\..\..\..\msvc-v120\test-exe-out

set atLeastOneFailed=0&rem
set failedTests=&rem
for /f "usebackq" %%d in (`dir /ad /b`) do (
	pushd %%d
	for /f "usebackq" %%f in (`dir *.exe /a-d /b`) do (
		echo vvvvvvvvvvvvvvvvvvvv %%d %%f vvvvvvvvvvvvvvvvvvvv

		%%f
		
		if !errorlevel! neq 0 (
			echo.
			echo FAILED with return code !ERRORLEVEL!.
			set failedTests=!failedTests! %%f&rem
			set atLeastOneFailed=1&rem
		)

		echo ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ %%d %%f ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		echo.
	)
	popd
)

if %atLeastOneFailed% neq 0 (
	echo FAILED tests:!failedTests!
	goto :error
)

:success
echo SUCCESS
set _rc=0
goto :cleanup

:error
set _rc=1

:cleanup
popd
endlocal & exit /b %_rc%
