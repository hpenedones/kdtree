#!/bin/bash
#
# Script to install the clang-format pre-commit hook
#

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
HOOK_SOURCE="$SCRIPT_DIR/pre-commit-clang-format.sh"
HOOK_DEST=".git/hooks/pre-commit"

if [ ! -d ".git" ]; then
    echo "Error: This script must be run from the root of the git repository."
    exit 1
fi

if [ ! -f "$HOOK_SOURCE" ]; then
    echo "Error: pre-commit-clang-format.sh not found in $SCRIPT_DIR"
    exit 1
fi

# Check if pre-commit hook already exists
if [ -f "$HOOK_DEST" ]; then
    echo "Warning: $HOOK_DEST already exists."
    read -p "Do you want to overwrite it? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Installation cancelled."
        exit 0
    fi
fi

# Copy the hook
cp "$HOOK_SOURCE" "$HOOK_DEST"
chmod +x "$HOOK_DEST"

echo "Pre-commit hook installed successfully!"
echo "The hook will check C++ code formatting before each commit."
