#!/bin/sh

# $0 script name
# $1 arg1, user project name, like wifi_mcu_basic_example/wifi_mcu_smartconfig_demo
# $2 arg2, user action, such as clean/config/build/rebuild/jflash/gdbserver
USER_PROJECT=""
USER_ACTION=""

if [ $# -eq 2 ]; then
    USER_PROJECT=$1
    USER_ACTION=$2
elif [ $# -eq 1 ]; then
    USER_PROJECT=$1
    USER_ACTION=config
else
    # default user project name
    USER_PROJECT=wifi_mcu_basic_example
    # default user action
    USER_ACTION=config
fi

echo "-----------------------------------------------------"
echo "USER_PROJECT = $USER_PROJECT"
echo "USER_ACTION  = $USER_ACTION"
echo "-----------------------------------------------------"


# NOTE: this script must be run from the top dir of this repository.
rootPath=`pwd`

# put all build output in this $buildPath
buildPath=${rootPath}/build

function action_clean()
{
    echo "start clean"
    if [ ! -d $buildPath ];then
        mkdir $buildPath
        return
    fi
    rm -rf $buildPath/*
}

function action_config()
{
    echo "start config user project = $USER_PROJECT"
    if [ ! -d $buildPath ];then
        mkdir $buildPath
    fi
    cd $buildPath &&  cmake -DUSER_PROJECT=$USER_PROJECT ..
}

function action_build()
{
    echo "start build user project = $USER_PROJECT"
    if [ ! -d $buildPath ];then
        mkdir $buildPath
        cd $buildPath &&  cmake -DUSER_PROJECT=$USER_PROJECT ..
    fi
    # cd $buildPath && make 1>../info.log 2>../warn.log
    cd $buildPath &&  make -j4
}

# config && build
function action_rebuild()
{
    echo "start rebuild user project = $USER_PROJECT"
    if [ ! -d  $buildPath ]; then
        mkdir $buildPath
    fi
    rm -rf $buildPath/*
    cd $buildPath &&  cmake -DUSER_PROJECT=$USER_PROJECT .. && make -j4
}

function action_jflash()
{
    echo "start jflash"
    cd $buildPath &&  make jlink_flash
}

function action_gdbserver()
{
    echo "start gdbserver"
    cd $buildPath &&  make jlink_gdbserver
}

function action_usage()
{
    echo "******************************  usage  ******************************"
    echo "\$1 -- user project name, like wifi_mcu_basic_example"
    echo "\$2 -- build action, like clean/config/build/rebuild"
    echo "*********************************************************************"
}

if [ "$#" -eq 0 ]; then
    action_clean
    action_usage
elif [ "$USER_ACTION" = "clean" ]; then
    action_clean
elif [ "$USER_ACTION" = "config" ]; then
    action_config
elif [ "$USER_ACTION" = "build" ]; then
    action_build
elif [ "$USER_ACTION" = "rebuild" ]; then
    action_rebuild
elif [ "$USER_ACTION" = "jflash" ]; then
    action_jflash
elif [ "$USER_ACTION" = "gdbserver" ]; then
    action_gdbserver
else
    action_usage
fi
