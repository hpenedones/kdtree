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

# Get list of staged C++ files
STAGED_FILES=$(git diff --cached --name-only --diff-filter=ACMR | grep -E '\.(cc|h|cpp|hpp)$')

if [ -z "$STAGED_FILES" ]; then
    # No C++ files staged, nothing to check
    exit 0
fi

echo "Running clang-format on staged C++ files..."

# Check if any files need formatting
NEEDS_FORMAT=0
for FILE in $STAGED_FILES; do
    if [ -f "$FILE" ]; then
        # Run clang-format and check if file would be modified
        clang-format --dry-run --Werror "$FILE" 2>&1
        if [ $? -ne 0 ]; then
            echo "Formatting issues found in: $FILE"
            NEEDS_FORMAT=1
        fi
    fi
done

if [ $NEEDS_FORMAT -eq 1 ]; then
    echo ""
    echo "Code formatting issues detected!"
    echo "Please run the following command to fix formatting:"
    echo ""
    echo "  clang-format -i" $STAGED_FILES
    echo ""
    echo "Then stage the changes and commit again."
    exit 1
fi

echo "All staged C++ files are properly formatted."
exit 0
