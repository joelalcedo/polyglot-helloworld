#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LANG_DIR="$ROOT_DIR/languages"

VERBOSE=0
FILTERS=()

# Parse flags (supports old bash; no getopt)
while [ $# -gt 0 ]; do
  case "$1" in
    -v|--verbose)
      VERBOSE=1
      shift
      ;;
    *)
      FILTERS+=("$1")
      shift
      ;;
  esac
done

matches_filter() {
  local name="$1"
  if [ "${#FILTERS[@]}" -eq 0 ]; then return 0; fi
  local f
  for f in "${FILTERS[@]}"; do
    if [ "$name" = "$f" ]; then return 0; fi
  done
  return 1
}

# Colors (only if stdout is a TTY)
if [ -t 1 ]; then
  C_COUNT="$(printf '\033[0;36m')"   # cyan
  C_LANG="$(printf '\033[0;35m')"    # magenta
  C_OUT="$(printf '\033[0;32m')"     # green
  C_PASS="$(printf '\033[0;32m')"
  C_FAIL="$(printf '\033[0;31m')"
  C_SKIP="$(printf '\033[0;33m')"
  C_RESET="$(printf '\033[0m')"
else
  C_COUNT=""; C_LANG=""; C_OUT=""; C_PASS=""; C_FAIL=""; C_SKIP=""; C_RESET=""
fi

passes=()
fails=()
skips=()

echo "== Polyglot Hello Runner =="

# Build list of languages first so we can show [i/N]
langs=()
for d in "$LANG_DIR"/*; do
  [ -d "$d" ] || continue
  lang="$(basename "$d")"
  matches_filter "$lang" || continue
  langs+=("$lang")
done

N="${#langs[@]}"
i=0

# mktemp portability (macOS needs a template)
mktemp_file() {
  mktemp "${TMPDIR:-/tmp}/polyglot.XXXXXX"
}

# Strip ANSI + CR, remove empty lines, take last line
last_clean_line() {
  # Reads from a file path argument
  # - remove CR
  # - strip ANSI escapes
  # - drop blank lines
  # - return last remaining line
  sed $'s/\r$//' "$1" \
    | sed -E 's/\x1B\[[0-9;]*[A-Za-z]//g' \
    | sed '/^[[:space:]]*$/d' \
    | tail -n 1
}

idx=0
while [ $idx -lt $N ]; do
  lang="${langs[$idx]}"
  d="$LANG_DIR/$lang"
  run="$d/run.sh"
  i=$((i + 1))

  if [ ! -x "$run" ]; then
    if [ $VERBOSE -eq 1 ]; then
      echo
      echo "---- $lang ----"
      echo "${C_SKIP}SKIP${C_RESET}  $lang (missing or not executable: $run)"
    else
      printf "%s[%d/%d]%s %s%s%s: %sSKIP%s\n" \
        "$C_COUNT" "$i" "$N" "$C_RESET" \
        "$C_LANG" "$lang" "$C_RESET" \
        "$C_SKIP" "$C_RESET"
    fi
    skips+=("$lang")
    idx=$((idx + 1))
    continue
  fi

  if [ $VERBOSE -eq 1 ]; then
    echo
    echo "---- $lang ----"
    set +e
    (cd "$d" && ./run.sh)
    status=$?
    set -e

    if [ $status -eq 0 ]; then
      echo "${C_PASS}PASS${C_RESET}  $lang"
      passes+=("$lang")
    else
      echo "${C_FAIL}FAIL${C_RESET}  $lang (exit=$status)"
      fails+=("$lang")
    fi

    idx=$((idx + 1))
    continue
  fi

  # Pretty mode: capture stdout separately from stderr.
  out_file="$(mktemp_file)"
  err_file="$(mktemp_file)"

  set +e
  (cd "$d" && ./run.sh) >"$out_file" 2>"$err_file"
  status=$?
  set -e

  if [ $status -eq 0 ]; then
    # Durable: prefer stdout only (avoids Nim/Guile/clang/docker warnings, etc.)
    hello="$(last_clean_line "$out_file")"
    if [ -z "${hello:-}" ]; then
      # Fallback: if stdout is empty for some reason, try stderr as last resort.
      hello="$(last_clean_line "$err_file")"
    fi
    printf "%s[%d/%d]%s %s%s%s: %s%s%s\n" \
      "$C_COUNT" "$i" "$N" "$C_RESET" \
      "$C_LANG" "$lang" "$C_RESET" \
      "$C_OUT" "${hello:-}" "$C_RESET"
    passes+=("$lang")
  else
    # On failure: show a short hint line, but don’t spam.
    hint="$(last_clean_line "$err_file")"
    if [ -z "${hint:-}" ]; then hint="$(last_clean_line "$out_file")"; fi
    printf "%s[%d/%d]%s %s%s%s: %sFAIL%s (exit=%d) %s\n" \
      "$C_COUNT" "$i" "$N" "$C_RESET" \
      "$C_LANG" "$lang" "$C_RESET" \
      "$C_FAIL" "$C_RESET" "$status" "${hint:-}"
    fails+=("$lang")
  fi

  rm -f "$out_file" "$err_file"
  idx=$((idx + 1))
done

echo
echo "== Summary =="
echo "PASS: ${#passes[@]}  ${passes[*]:-}"
echo "FAIL: ${#fails[@]}  ${fails[*]:-}"
echo "SKIP: ${#skips[@]}  ${skips[*]:-}"

[ "${#fails[@]}" -eq 0 ]
