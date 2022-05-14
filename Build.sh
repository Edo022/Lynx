#!/bin/bash
gcc -I. -E -P -CC -w -xc ./Page/Lux_Umbra_Languages.html -o \~Lux_Umbra_Languages.html
cat \~Lux_Umbra_Languages.html | awk 'NR > 35 {print $0}' > Lux.html
rm \~Lux_Umbra_Languages.html
