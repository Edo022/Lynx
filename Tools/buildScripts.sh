#!/bin/bash

cd ./Setup
    python3 -m PyInstaller -F --clean -y ./UpdateBuild.py &
    python3 -m PyInstaller -F --clean -y ./Setup.py &
    pwd
cd ../Build
    python3 -m PyInstaller -F --clean -y ./Utils.py &
    python3 -m PyInstaller -F --clean -y ./Paths.py &
    python3 -m PyInstaller -F --clean -y ./Alloy_tmpp.py &

    python3 -m PyInstaller -F --clean -y ./Build.py &
    python3 -m PyInstaller -F --clean -y ./Generators/GenInitializers.py &
    python3 -m PyInstaller -F --clean -y ./Generators/GenInterfaces.py &
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