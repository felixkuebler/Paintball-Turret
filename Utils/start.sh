#!/bin/bash

BASE_DIR=$(dirname "$0")

make -C $BASE_DIR clean;
make -C $BASE_DIR run;