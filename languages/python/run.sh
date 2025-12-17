#!/usr/bin/env bash
set -euo pipefail
IMG="hello-python"
docker build -t "$IMG" .
docker run --rm "$IMG"
