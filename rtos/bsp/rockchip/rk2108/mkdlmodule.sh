#! /bin/bash

MODULE=$1
PARM=$2

DLMODULE_PATH="dlmodules"

if [ -z $MODULE ]; then

    echo
    echo "usage:"
    echo "    build: ./mkdlmodule.sh module_name"
    echo "    clear: ./mkdlmodule.sh module_name -c"
    echo

else

    if [ -z $PARM ]; then
        scons --target=ua -s

        cd ../../../applications/rtthread-apps/

        export RTT_ROOT=../../
        export BSP_ROOT=../../bsp/rockchip/rk2108

        ln -s ../$DLMODULE_PATH/$MODULE $MODULE
        scons --app=$MODULE
        cp $MODULE/*.mo ../../bsp/rockchip/rk2108/resource/userdata/dlmodule/
        rm $MODULE
        cd -
    else
        if [ $PARM = "-c" ]; then
            cd ../../../applications/rtthread-apps/
            ln -s ../$DLMODULE_PATH/$MODULE
            rm $MODULE/*.o
            rm $MODULE/*.mo
            rm $MODULE
            cd -
        fi
    fi
fi