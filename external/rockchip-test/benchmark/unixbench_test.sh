#!/bin/bash

# Set the path to UnixBench installation and the log file paths
UNIXBENCH_PATH="/opt/unixbench"
LOG_DIR="/var/log/unixbench"
RESULT_FILE="${LOG_DIR}/unixbench_result_$(date +%Y%m%d_%H%M%S).log"
ERROR_FILE="${LOG_DIR}/unixbench_error_$(date +%Y%m%d_%H%M%S).log"

# Set performance mode
echo performance | tee $(find /sys/ -name *governor) > /dev/null 2>&1 || true

# Check if the UnixBench executable exists
if [ ! -f "${UNIXBENCH_PATH}/Run" ]; then
    echo "UnixBench executable does not exist. Please check the path ${UNIXBENCH_PATH}/Run."
    exit 1
fi

# Ensure the log directory exists
mkdir -p "${LOG_DIR}"

echo "Starting UnixBench tests..."
echo "Results will be saved to ${RESULT_FILE}"
echo "Error log will be saved to ${ERROR_FILE}"
echo -e "${RED}Note: Tests will take time. Please wait...${NC}"

# Execute UnixBench test and redirect output to log files
(cd "${UNIXBENCH_PATH}" && ./Run) >"${RESULT_FILE}" 2>"${ERROR_FILE}"

echo "UnixBench tests completed."
