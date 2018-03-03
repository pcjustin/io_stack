#!/bin/sh

astyle --suffix=none --indent-preprocessor --delete-empty-lines --align-pointer=type --max-instatement-indent=40 --break-blocks --pad-oper --min-conditional-indent=0 --style=google --indent=tab -j *.c
astyle --suffix=none --indent-preprocessor --delete-empty-lines --align-pointer=type --max-instatement-indent=40 --break-blocks --pad-oper --min-conditional-indent=0 --style=google --indent=tab -j *.h
