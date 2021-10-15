#!/bin/bash



cd ./Setup
    rm -rf ./dist ./build ./*.spec ./__pycache__
cd ../Build
    rm -rf ./dist ./build ./*.spec ./__pycache__ &&
    rm -rf ./dist ./build ./*.spec ./Generators/__pycache__
cd ..


opt=''
file=''

if [ "$1" = '-O' ] || [ "$1" = '-OO' ]
then
    opt=$1
else
    if [ -r $1 ]
    then
        file=$1
    else
        echo "File $1 does not exist"
        exit 1
    fi
    if [ "$2" = '-O' ] || [ "$2" = '-OO' ]
    then
        opt=$2
    fi
fi



if [ -z "$file" ]
then
    cd ./Setup
        python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./UpdateBuild.py &
        python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./Setup.py
    cd ../Build
        python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./Utils.py &
        python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./Paths.py &
        wait

        python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./Generators/GenInitializers.py &
        python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./Generators/GenInterfaces.py
        python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./Alloy_tmpp.py &
        wait

        python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./Build.py &
        wait
    cd ..
else
    exit 2 #FIXME ADD SUPPORT FOR SINGLE SCRIPT COMPILATION
    python3 $opt -m PyInstaller -F --clean -y --log-level=WARN $file
fi



cd ./Setup
    [ -r ./dist/Setup ]       && mv ./dist/Setup ./Setup
    [ -r ./dist/UpdateBuild ] && mv ./dist/UpdateBuild ./UpdateBuild
    rm -rf ./dist ./build ./*.spec ./__pycache__
cd ../Build
    [ -r ./dist/Build ]           && mv ./dist/Build ./Build
    [ -r ./dist/GenInitializers ] && mv ./dist/GenInitializers ./Generators/GenInitializers
    [ -r ./dist/GenInterfaces ]   && mv ./dist/GenInterfaces ./Generators/GenInterfaces
    rm -rf ./dist ./build ./*.spec ./__pycache__ &&
    rm -rf ./dist ./build ./*.spec ./Generators/__pycache__
cd ..