#!/usr/bin/env bash
set -euo pipefail
IMG="hello-julia"
docker build -t "$IMG" .
docker run --rm "$IMG"
