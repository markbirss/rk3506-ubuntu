# The env variables below can be overridden

# HACK for old chromium, see:
# https://bugs.chromium.org/p/chromium/issues/detail?id=1279574
export WL_OUTPUT_VERSION=3

# Comment out this for atomic related functions, e.g. sprites
export WESTON_DISABLE_ATOMIC=1

# Increasing this can help to reduce tearing when dumping low-level DRM buffers
export WESTON_DRM_MIN_BUFFERS=2

# Allow using drm modifier, e.g. ARM AFBC
# export WESTON_ALLOW_GBM_MODIFIERS=1

# Enable black background for fullscreen views
# export WESTON_FULLSCREEN_BLACK_BACKGROUND=1

# Set default layer for the first APP
# export WESTON_FIRST_APP_LAYER=top
# export WESTON_FIRST_APP_LAYER=bottom

# Allow disabling unused CRTCs
# WESTON_DRM_MASTER=1

# Override initial freezing time
# export WESTON_DRM_INITIAL_FREEZE_MS=100

# Override output's resized freezing time
# export WESTON_DRM_RESIZE_FREEZE_MS=1000

# Primary screen
# export WESTON_DRM_PRIMARY=eDP-1

# Single screen
# export WESTON_DRM_SINGLE_HEAD=1

# Fallback to any available connector
# export WESTON_DRM_HEAD_FALLBACK=1

# Connector selecting mode:
# default|primary|internal|external|external-dual
# export WESTON_DRM_HEAD_MODE=external-dual

# Screens layout direction
# horizontal|vertical|same-as
# export WESTON_OUTPUT_FLOW=vertical

# Virtual display size
# export WESTON_DRM_VIRTUAL_SIZE=1024x768

# Comment out these to disable mirror mode
export WESTON_DRM_MIRROR=1
export WESTON_DRM_KEEP_RATIO=1

# Disable DRM plane hardware scale feature
# export WESTON_DRM_DISABLE_PLANE_SCALE=1

# Tag file for freezing weston display
export WESTON_FREEZE_DISPLAY=/tmp/.freeze_weston

# Try to pin views to the assigned output
# export WESTON_OUTPUT_PIN=1

# Set dynamic config file path
# export WESTON_DRM_CONFIG=/tmp/.weston_drm.conf
#
# Dynamic config examples:
# echo "compositor:state:offscreen" > /tmp/.weston_drm.conf # off + input
# echo "compositor:state:sleep" > /tmp/.weston_drm.conf # off + input(wakeable)
# echo "compositor:state:block" > /tmp/.weston_drm.conf # no input
# echo "compositor:state:freeze" > /tmp/.weston_drm.conf # no input + freeze
# echo "compositor:state:off" > /tmp/.weston_drm.conf # no input + off
# echo "compositor:state:on" > /tmp/.weston_drm.conf
# echo "compositor:hotplug:off" > /tmp/.weston_drm.conf
# echo "compositor:hotplug:on" > /tmp/.weston_drm.conf
# echo "compositor:hotplug:force" > /tmp/.weston_drm.conf
# echo "compositor:cursor:hide" > /tmp/.weston_drm.conf
# echo "compositor:cursor:show" > /tmp/.weston_drm.conf
# echo "compositor:output:pin" > /tmp/.weston_drm.conf
# echo "compositor:output:unpin" > /tmp/.weston_drm.conf
# echo "output:DSI-1:off" > /tmp/.weston_drm.conf
# echo "output:eDP-1:freeze" > /tmp/.weston_drm.conf
# echo "output:DSI-1:offscreen" > /tmp/.weston_drm.conf
# echo "output:DSI-1:on" > /tmp/.weston_drm.conf
# echo "output:all:rotate90" > /tmp/.weston_drm.conf
# echo "output:all:rect=<100,20,1636,2068>" > /tmp/.weston_drm.conf
# echo "output:HDMI-A-1:mode=800x600" > /tmp/.weston_drm.conf
# echo "output:HDMI-A-1:pos=100,200" > /tmp/.weston_drm.conf
# echo "output:HDMI-A-1:size=1920x1080" > /tmp/.weston_drm.conf
# echo "output:HDMI-A-1:prefer" > /tmp/.weston_drm.conf
# echo "output:HDMI-A-1:primary" > /tmp/.weston_drm.conf
# echo "output:HDMI-A-1:input=*" > /tmp/.weston_drm.conf
# echo "output:HDMI-A-1:input=" > /tmp/.weston_drm.conf
# echo "output:HDMI-A-1:input=event6" > /tmp/.weston_drm.conf
# echo "output:HDMI-A-1:input=goodix*" > /tmp/.weston_drm.conf
# echo "output:HDMI-A-1:input=goodix-ts" > /tmp/.weston_drm.conf
