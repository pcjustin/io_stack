#!/bin/bash

rm -rf .AppleDouble
rm -rf .vscode
rm -rf build

DATE=$(date +%Y%m%d)

tar Jcvf io_stack_${DATE}.tar.xz .