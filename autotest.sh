#!/bin/bash
echo '---------------------------------TEST--------------------------------------'
cd ./src
g++ test.cc -o ../bin/test
cd ../bin
./test
