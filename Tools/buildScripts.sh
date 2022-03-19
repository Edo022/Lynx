#!/bin/bash




./clearScripts.sh


opt=''
[ "$1" = '-O' ] || [ "$1" = '-OO' ] && opt=$1




cd ./Setup
    [ -r ./Setup ]       && rm ./Setup
    [ -r ./UpdateBuild ] && rm ./UpdateBuild
    python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./Setup.py &
    python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./UpdateBuild.py &
cd ../Build
    python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./Utils.py &
    python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./Paths.py &
    wait
cd ../Setup
    [ -r ./dist/Setup ]       && mv ./dist/Setup       ./Setup
    [ -r ./dist/UpdateBuild ] && mv ./dist/UpdateBuild ./UpdateBuild
cd ../Build


    cd Generators
        # [ -r ./GenGlsl ]         && rm ./GenGlsl
        [ -r ./GenInitializers ] && rm ./GenInitializers
        [ -r ./GenInterfaces ]   && rm ./GenInterfaces
        # python3 $opt -m PyInstaller --paths=.. -F --clean -y --log-level=WARN ./GenGlsl.py &
        gcc -std=c11 ./Lux/Lux.c -o ./GenGlsl //TODO update output name //FIXME use make command
        python3 $opt -m PyInstaller --paths=.. -F --clean -y --log-level=WARN ./GenInitializers.py &
        python3 $opt -m PyInstaller --paths=.. -F --clean -y --log-level=WARN ./GenInterfaces.py &
    cd ..
    [ -r ./Build ] && rm ./Build
    python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./Alloy_tmpp.py &
    wait
    cd Generators
        # [ -r ./dist/GenGlsl ]         && mv ./dist/GenGlsl         ./GenGlsl
        [ -r ./dist/GenInitializers ] && mv ./dist/GenInitializers ./GenInitializers
        [ -r ./dist/GenInterfaces ]   && mv ./dist/GenInterfaces   ./GenInterfaces
    cd ..


    python3 $opt -m PyInstaller -F --clean -y --log-level=WARN ./Build.py &
    wait
    [ -r ./dist/Build ] && mv ./dist/Build ./Build
cd ..




./clearScripts.sh