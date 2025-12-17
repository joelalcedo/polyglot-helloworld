#!/usr/bin/env bash
set -euo pipefail
IMG="hello-node"
docker build -t "$IMG" .
docker run --rm "$IMG"
