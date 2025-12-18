#!/usr/bin/env bash
set -euo pipefail
IMG="hello-chip_8"
PLATFORM="${POLYGLOT_PLATFORM:-}"
if [ -n "$PLATFORM" ]; then
  docker build --platform "$PLATFORM" -t "$IMG" .
  docker run --rm --platform "$PLATFORM" "$IMG"
else
  docker build -t "$IMG" .
  docker run --rm "$IMG"
fi
