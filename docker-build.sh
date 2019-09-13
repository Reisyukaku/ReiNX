#!/usr/bin/env bash
echo "Checking reinx-builder image..."

IMAGE=`docker image ls|grep reinx-builder -c`
if [[ "$IMAGE" == 1 ]]; then
  echo "Image already exists..."
  echo ""
  echo "To force a new builder image, run: "
  echo "docker image rm $(docker image ls -a | grep reinx-builder | awk '{print $1}')"
  echo ""
  echo "Proceeding with current image..."
else
  echo ""
  echo "Building docker image locally..."
  echo ""
  docker build . -t reinx-builder:latest
fi

echo "Checking build container...."
CONTAINER=`docker ps -a|grep reinx-builder -c`

if ! [[ "$CONTAINER" == 0 ]]; then
  echo "Removing old build container..."
  docker rm $(docker ps -a | grep reinx-builder | awk '{print $1}')
else
  echo "No old build container found, proceeding..."
fi

echo "Running build container..."
docker run --rm -a stdout -a stderr --name reinx-builder -v $(pwd):/developer reinx-builder:latest