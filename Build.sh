#!/bin/bash
cat ./Page/Lux_Umbra_Languages.html |\
    sed -e "s/^\s*#include/#include/g" |\
    gcc -I. -E -P -CC -w -xc -traditional-cpp - |\
    sed -e ':a' -e 'N' -e '$!ba' -e "s/\n/<\!-- \\\\n -->/g" |\
    sed -e "s/^.*\(<\!DOCTYPE html>\)/\1/g" -e "s/<\!-- \\\\n -->/\\n/g" \
> Lux.html

cat ./Page/Internal.html |\
    sed -e "s/^\s*#include/#include/g" |\
    gcc -I. -E -P -CC -w -xc -traditional-cpp - |\
    sed -e ':a' -e 'N' -e '$!ba' -e "s/\n/<\!-- \\\\n -->/g" |\
    sed -e "s/^.*\(<\!DOCTYPE html>\)/\1/g" -e "s/<\!-- \\\\n -->/\\n/g" \
> Internal.html
