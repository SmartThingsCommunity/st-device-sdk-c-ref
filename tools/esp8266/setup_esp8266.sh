#!/bin/bash

CHIP_NAME=${1}

cd bsp/${CHIP_NAME}

git am ../../patches/${CHIP_NAME}/*.patch
