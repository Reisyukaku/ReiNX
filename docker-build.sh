#!/usr/bin/env bash
echo "Checking reinx-builder image..."

IMAGE=`docker image ls|grep reinx-builder -c`
if [[ "$IMAGE" == 1 ]]; then
  echo "Reinx-builder image is exist"
else
  echo ""
  echo "Building docker image locally..."
  echo ""
  docker build . -t reinx-builder
fi

echo "Checking container...."
CONTAINER=`docker ps -a|grep reinx-builder -c`
echo "Building..."
if [[ "$CONTAINER" == 1 ]]; then
  docker start -a reinx-builder
else
  docker run -a stdout -a stderr --name reinx-builder -v $(pwd):/developer reinx-builder
fi
