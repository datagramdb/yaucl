#!/usr/bin/env bash
git rm -rf dist
git submodule update --init --recursive
cd submodules/antlr4/tool
mvn package