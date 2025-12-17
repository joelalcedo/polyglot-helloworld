#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LANG_DIR="$ROOT_DIR/languages"

# Optional filter: ./run_all.sh python go
FILTERS=("$@")

matches_filter() {
  local name="$1"
  if [ "${#FILTERS[@]}" -eq 0 ]; then return 0; fi
  for f in "${FILTERS[@]}"; do
    if [ "$name" = "$f" ]; then return 0; fi
  done
  return 1
}

GREEN="$(printf '\033[0;32m')"
RED="$(printf '\033[0;31m')"
YELLOW="$(printf '\033[0;33m')"
RESET="$(printf '\033[0m')"

passes=()
fails=()
skips=()

echo "== Polyglot Hello Runner =="

for d in "$LANG_DIR"/*; do
  [ -d "$d" ] || continue
  lang="$(basename "$d")"
  run="$d/run.sh"

  matches_filter "$lang" || continue

  if [ ! -x "$run" ]; then
    echo "${YELLOW}SKIP${RESET}  $lang (missing or not executable: $run)"
    skips+=("$lang")
    continue
  fi

  echo
  echo "---- $lang ----"

  set +e
  output="$((cd "$d" && ./run.sh) 2>&1)"
  status=$?
  set -e

  if [ -n "$output" ]; then
    echo "$output"
  fi

  if [ $status -eq 0 ]; then
    echo "${GREEN}PASS${RESET}  $lang"
    passes+=("$lang")
  else
    echo "${RED}FAIL${RESET}  $lang (exit=$status)"
    fails+=("$lang")
  fi
done

echo
echo "== Summary =="
echo "PASS: ${#passes[@]}  ${passes[*]:-}"
echo "FAIL: ${#fails[@]}  ${fails[*]:-}"
echo "SKIP: ${#skips[@]}  ${skips[*]:-}"

[ "${#fails[@]}" -eq 0 ]
