#!/usr/bin/env bash
set -euo pipefail
IMG="hello-lua"
docker build -t "$IMG" .
docker run --rm "$IMG"
