#!/bin/bash



cd ./Setup
    rm -rf ./dist ./build ./*.spec ./__pycache__
cd ../Build
    rm -rf ./dist ./build ./*.spec ./__pycache__           &&
    rm -rf ./dist ./build ./*.spec ./Generators/__pycache__
cd ..



#-OO
cd ./Setup
    python3 $1 -m PyInstaller -F --clean -y --log-level=WARN ./UpdateBuild.py &
    python3 $1 -m PyInstaller -F --clean -y --log-level=WARN ./Setup.py
cd ../Build
    python3 $1 -m PyInstaller -F --clean -y --log-level=WARN ./Utils.py &
    python3 $1 -m PyInstaller -F --clean -y --log-level=WARN ./Paths.py &
    wait

    python3 $1 -m PyInstaller -F --clean -y --log-level=WARN ./Generators/GenInitializers.py &
    python3 $1 -m PyInstaller -F --clean -y --log-level=WARN ./Generators/GenInterfaces.py
    python3 $1 -m PyInstaller -F --clean -y --log-level=WARN ./Alloy_tmpp.py &
    wait

    python3 $1 -m PyInstaller -F --clean -y --log-level=WARN ./Build.py &
    wait
cd ..




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