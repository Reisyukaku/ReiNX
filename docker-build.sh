#!/usr/bin/env bash
ECHO ""
ECHO "Emptying out /build and /out folders..."
ECHO ""
rm -rf ./build
rm -rf ./out

ECHO ""
ECHO "Building docker image locally..."
ECHO ""
docker build . -t reinx-builder:latest

ECHO ""
ECHO "Running image and generating build..."
ECHO ""
docker run -v $(pwd)/build:/build -v $(pwd)/out:/out reinx-builder:latest