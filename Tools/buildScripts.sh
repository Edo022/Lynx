#!/bin/bash

cd ./Setup
    python3 -m PyInstaller -F --clean ./UpdateBuild.py &
    python3 -m PyInstaller -F --clean ./Setup.py &
    pwd
cd ../Build
    python3 -m PyInstaller -F --clean ./Utils.py &
    python3 -m PyInstaller -F --clean ./Paths.py &
    python3 -m PyInstaller -F --clean ./Alloy_tmpp.py &

    python3 -m PyInstaller -F --clean ./Build.py &
    python3 -m PyInstaller -F --clean ./Generators/GenInitializers.py &
    python3 -m PyInstaller -F --clean ./Generators/GenInterfaces.py &
    pwd
cd ..


wait
cd ./Setup
    mv ./dist/Setup ./Setup             &&
    mv ./dist/UpdateBuild ./UpdateBuild &&
    rm -rf ./dist ./build ./*.spec ./__pycache__
cd ../Build
    mv ./dist/Build ./Build                                &&
    mv ./dist/GenInitializers ./Generators/GenInitializers &&
    mv ./dist/GenInterfaces ./Generators/GenInterfaces     &&
    rm -rf ./dist ./build ./*.spec ./__pycache__           &&
    rm -rf ./dist ./build ./*.spec ./Generators/__pycache__
cd ..