#!/bin/sh

PYTHON_VER=${1:-3.6.15}
echo "wget https://www.python.org/ftp/python/${PYTHON_VER}/Python-${PYTHON_VER}.tgz"
echo "tar xf Python-${PYTHON_VER}.tgz"
echo "cd Python-${PYTHON_VER}"
echo "sudo apt-get install libsqlite3-dev"
echo "./configure --enable-optimizations"
echo "sudo make install -j8"
