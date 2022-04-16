#!/usr/bin/env bash
git submodule update --init --recursive
cd submodules/antlr4/tool
mvn package