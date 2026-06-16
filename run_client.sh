#!/usr/bin/env bash
set -e
ROOT="$(cd "$(dirname "$0")" && pwd)"
# Usage: ./run_client.sh [host]   (default: 127.0.0.1)
HOST="${1:-127.0.0.1}"
exec "$ROOT/build/client/client" "$HOST"
