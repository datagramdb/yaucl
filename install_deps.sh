#!/bin/bash
# Setting up the current directory's directories
#mkdir -p antlr4/cpp

# Installing the required packages for antlr4
sudo apt-get install -y uuid-dev maven

## Pulling the repositories
git submodule update --init --recursive
git submodule update --recursive --remote

# Compiling via Maven antlr4
export MAVEN_OPTS="-Xmx1G"
pushd submodules/antlr4
mvn --fail-at-end -U -DskipTests package
popd

java -jar submodules/antlr4/tool/target/antlr4-4.9.3-SNAPSHOT-complete.jar -Dlanguage=Cpp -visitor src/bpm/Traces.g
mv src/bpm/Traces*.h include/yaucl/bpm/
