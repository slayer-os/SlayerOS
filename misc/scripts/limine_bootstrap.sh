#!/bin/bash

git clone https://github.com/limine-bootloader/limine.git -b v9.x-binary --depth=1 misc/limine
cd misc/limine && make
