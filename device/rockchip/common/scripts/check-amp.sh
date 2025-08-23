#!/bin/bash -e

# The AMP RTT needs scons support
if [ "$RK_AMP_RTT_TARGET" ]; then
	"$RK_SCRIPTS_DIR/check-package.sh" scons
fi
