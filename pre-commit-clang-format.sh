#!/bin/bash
#
# Git pre-commit hook to check C++ code formatting with clang-format
#

# Check if clang-format is installed
if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format is not installed."
    echo "Please install clang-format to use this pre-commit hook."
    exit 1
fi

echo "Running clang-format on staged C++ files..."

# Check if any files need formatting
NEEDS_FORMAT=0
FILES_TO_FORMAT=()

# Use NUL-delimited file list to handle filenames with spaces/special characters
while IFS= read -r -d '' FILE; do
    # Ensure there is a staged blob for this file
    if git cat-file -e :"$FILE" 2>/dev/null; then
        # Run clang-format on the staged contents and check if it would report issues
        if ! git show :"$FILE" | clang-format --dry-run --Werror --assume-filename="$FILE" 2>&1; then
            echo "Formatting issues found in: $FILE"
            NEEDS_FORMAT=1
            FILES_TO_FORMAT+=("$FILE")
        fi
    fi
done < <(git diff --cached --name-only --diff-filter=ACMR -z | grep -zE '\.(cc|h|cpp|hpp)$')

if [ $NEEDS_FORMAT -eq 1 ]; then
    echo ""
    echo "Code formatting issues detected!"
    echo "Please run the following command to fix formatting:"
    echo ""
    echo -n "  clang-format -i"
    printf " '%s'" "${FILES_TO_FORMAT[@]}"
    echo ""
    echo ""
    echo "Then stage the changes and commit again."
    exit 1
fi

echo "All staged C++ files are properly formatted."
exit 0
