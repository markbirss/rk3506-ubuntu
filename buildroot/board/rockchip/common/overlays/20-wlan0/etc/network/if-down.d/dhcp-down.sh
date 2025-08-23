#!/bin/sh

# This file is executed by ifupdown in pre-up, post-up, pre-down and
# post-down phases of network interface configuration.

# run this script only for interfaces which have dhcp-down option
[ -z "$IF_DHCP_DOWN" ] && exit 0

$IF_DHCP_DOWN $IFACE

exit 0
