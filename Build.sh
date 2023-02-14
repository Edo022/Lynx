#!/bin/bash




cat ./Page/Lux-Umbra-Languages.html \
    | perl -ne "s/^\s*#include/#include/; print;"                                           `# Remove leading spaces for traditional cpp`               \
    | gcc -I. -E -P -CC -w -xc -traditional-cpp -                                                                                                       \
    | perl -0ne "s/^[\S\s]*?(?=<!DOCTYPE html>)//; print;"                                  `# Remove everything added by cpp before the !DOCTYPE tag`  \
    | perl -0ne "s/(\n(\s+)<!--[\S\s]*?-->)|(<!--[\S\s]*?-->)|(<!--[\S\s]*?$)//g; print;"   `# Remove all comments`                                     \
> Lux-Umbra-Languages.html




cat ./Page/Internal.html \
    | perl -ne "s/^\s*#include/#include/; print;"                                           `# Remove leading spaces for traditional cpp`               \
    | gcc -I. -E -P -CC -w -xc -traditional-cpp -                                                                                                       \
    | perl -0ne "s/^[\S\s]*?(?=<!DOCTYPE html>)//; print;"                                  `# Remove everything added by cpp before the !DOCTYPE tag`  \
    | perl -0ne "s/(\n(\s+)<!--[\S\s]*?-->)|(<!--[\S\s]*?-->)|(<!--[\S\s]*?$)//g; print;"   `# Remove all comments`                                     \
> Internal.html