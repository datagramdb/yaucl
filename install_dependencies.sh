#!/usr/bin/env bash
pushd submodules/zsv
./configure && sudo make install
popd
sudo apt-get install -y uuid-dev maven
export MAVEN_OPTS="-Xmx1G"
pushd submodules/antlr4
mvn --fail-at-end -U -DskipTests package
popd
antlr4_gen.sh