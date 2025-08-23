#!/bin/sh

# This file is executed by ifupdown in pre-up, post-up, pre-down and
# post-down phases of network interface configuration.

# run this script only for interfaces which have dhcp-up option
[ -z "$IF_DHCP_UP" ] && exit 0

$IF_DHCP_UP $IFACE&

exit 0
