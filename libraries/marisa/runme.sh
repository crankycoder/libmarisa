#!/bin/sh
rm build/web/*
make build/web/marisa.html
. ./tests/simpleweb.sh &
open http://127.0.0.1:8000/build/web/marisa.html
