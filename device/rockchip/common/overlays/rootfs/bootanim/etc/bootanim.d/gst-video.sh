#!/bin/sh

VIDEO_SINK="kmssink force-modesetting=true fullscreen=true"

VIDEO=$(ls /etc/bootanim.d/*.mp4 2>/dev/null)
if [ -z "$VIDEO" ]; then
	echo "No video under /etc/bootanim.d/, using test video source..."
	gst-launch-1.0 videotestsrc ! $VIDEO_SINK >/dev/null 2>/dev/null&
else
	gst-play-1.0 $VIDEO -q --no-interactive --audiosink=fakesink \
		--videosink="$VIDEO_SINK"&
fi
