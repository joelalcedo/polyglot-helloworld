#!/usr/bin/env bash
set -euo pipefail
IMG="hello-ruby"
docker build -t "$IMG" .
docker run --rm "$IMG"
