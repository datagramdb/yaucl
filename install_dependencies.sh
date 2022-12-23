#!/usr/bin/env bash
sudo apt-get install -y uuid-dev maven
export MAVEN_OPTS="-Xmx1G"
pushd submodules/antlr4
mvn --fail-at-end -U -DskipTests package
popd