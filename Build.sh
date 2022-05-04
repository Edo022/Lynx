#!/bin/bash
gcc -I. -E -P -CC -w -xc ./Page/Languages.html -o \~Languages.html
cat \~Languages.html | awk 'NR > 35 {print $0}' > Lux.html
rm \~Languages.html
