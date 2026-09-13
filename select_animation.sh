#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ANIMATION_DIR="$SCRIPT_DIR/animation"
TARGET_DIR="$SCRIPT_DIR/led-controller/animation"

shopt -s nullglob
animations=()
for dir in "$ANIMATION_DIR"/*/; do
	dir="${dir%/}"
	animations+=("${dir##*/}")
done
shopt -u nullglob

if [ ${#animations[@]} -eq 0 ]; then
	echo "No animations found in $ANIMATION_DIR" >&2
	exit 1
fi

PS3="Select: "
select animation in "${animations[@]}"; do
	if [ -n "${animation:-}" ]; then
		break
	else
		for a in "${animations[@]}"; do
			if [ "${REPLY:-}" = "$a" ]; then
				animation="$a"
				break 2
			fi
		done
		echo "Invalid selection. Please try again."
	fi
done

if [ -z "${animation:-}" ]; then
	echo "No animation selected." >&2
	exit 1
fi

mkdir -p "$TARGET_DIR"

if [ -f "$ANIMATION_DIR/$animation/data.h" ]; then
	cp "$ANIMATION_DIR/$animation/data.h" "$TARGET_DIR/"
fi

echo "Selected: $animation"
echo "Copied data.h to $TARGET_DIR"
