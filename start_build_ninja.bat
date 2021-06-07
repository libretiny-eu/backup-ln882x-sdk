@echo off
set USER_PROJECT=%1
set USER_ACTION=%2
set /A VALID_ARG_NUM=2

if "%USER_PROJECT%" == "" (
    set USER_PROJECT=wifi_mcu_basic_example
    set /A VALID_ARG_NUM=%VALID_ARG_NUM% - 1
)

if "%USER_ACTION%" == "" (
    set USER_ACTION=config
    set /A VALID_ARG_NUM=%VALID_ARG_NUM% - 1
)

@REM current working directory
set cwd=%~dp0
@REM put all output in ./build/
set buildPath=%cwd%build
echo "buildPath --> %buildPath%"


::------------------------------------------------------------------------------
::-------------------------          main            ---------------------------
::------------------------------------------------------------------------------
echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

echo "USER_PROJECT = %USER_PROJECT%"
echo "USER_ACTION  = %USER_ACTION%"

if %VALID_ARG_NUM% == 0 (
    call:action_usage
) ^
else if %USER_ACTION% == clean (
    call:action_clean
) ^
else if %USER_ACTION% == config (
    call:action_config
) ^
else if %USER_ACTION% == build (
    call:action_build
) ^
else if %USER_ACTION% == rebuild (
    call:action_rebuild
) ^
else if %USER_ACTION% == jflash (
    call:action_jflash
) ^
else (
    echo "unknown user action."
)

echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
echo . & goto:eof


::------------------------------------------------------------------------------
::-- action_usage
::------------------------------------------------------------------------------
:action_usage
    echo "******************************  usage  ******************************"
    echo "\$1 -- user project name, like wifi_mcu_basic_example"
    echo "\$2 -- build action, like clean/config/build/rebuild/jflash"
    echo "*********************************************************************"
goto:eof

::------------------------------------------------------------------------------
::-- action_clean
::------------------------------------------------------------------------------
:action_clean
    echo "------  clean directory: %buildPath%  ------"
    if exist %buildPath% (
        del /s /q %buildPath%\\*
    )
goto:eof

::------------------------------------------------------------------------------
::-- action_config
::------------------------------------------------------------------------------
:action_config
    echo "------  config : %USER_PROJECT%  ------"
    if exist %buildPath% (
        del /s /q %buildPath%
    )
    cmake -DUSER_PROJECT=%USER_PROJECT% -S . -B %buildPath%  -G "Ninja"
goto:eof

::------------------------------------------------------------------------------
::-- action_build
::------------------------------------------------------------------------------
:action_build
    echo "------  build: %USER_PROJECT%  ------"
    if not exist %buildPath% (
        echo "Please config first!!!"
        goto:eof
    )
    cmake --build  %buildPath% --parallel 8
goto:eof

::------------------------------------------------------------------------------
::-- action_rebuild
::------------------------------------------------------------------------------
:action_rebuild
    echo "------  rebuild: %USER_PROJECT% ------"
    if exist %buildPath% (
        del /s /q %buildPath%
    )
    cmake -DUSER_PROJECT=%USER_PROJECT% -S . -B %buildPath%  -G "Ninja"
    cmake --build  %buildPath% --parallel 8
goto:eof

::------------------------------------------------------------------------------
::-- action_jflash
::------------------------------------------------------------------------------
:action_jflash
    echo "------  download flashimage.bin via jflash  ------"
    echo "!!! Do NOT save changes to J-Flash project after downloading !!!"
    %cwd%tools/JFlash/JFlash.exe -openprj%cwd%/tools/JFlash/LN881x.jflash ^
        -open%buildPath%/bin/flashimage.bin,0  ^
        -erasechip -programverify -startapp -exit
    echo "------  reset the chip and the code starts running...  ------"
goto:eof
