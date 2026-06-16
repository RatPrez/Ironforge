#!/usr/bin/env bash
set -e
ROOT="$(cd "$(dirname "$0")" && pwd)"
"$ROOT/build.sh" && "$ROOT/run_server.sh"
