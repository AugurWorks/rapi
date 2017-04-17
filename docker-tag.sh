#!/bin/sh

version=`node -e "console.log(require('./package.json').version);"`

echo "Building container v$version$1"
docker build -t rapi .

echo "Tagging version $version$1"
docker tag rapi 274685854631.dkr.ecr.us-east-1.amazonaws.com/rapi:$version$1
docker tag rapi 274685854631.dkr.ecr.us-east-1.amazonaws.com/rapi:latest$1
docker push 274685854631.dkr.ecr.us-east-1.amazonaws.com/rapi:$version$1
docker push 274685854631.dkr.ecr.us-east-1.amazonaws.com/rapi:latest$1
docker rmi rapi 274685854631.dkr.ecr.us-east-1.amazonaws.com/rapi:latest$1
