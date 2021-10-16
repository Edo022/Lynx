#!/bin/bash




cd ./Setup
    rm -rf ./dist ./build ./*.spec ./__pycache__

cd ../Build
    cd Generators
        rm -rf ./dist ./build ./*.spec ./__pycache__
    cd ..
    rm -rf ./dist ./build ./*.spec ./__pycache__
cd ..