/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <wfd.h>
#include <wfdext.h>
#if defined(__RT_THREAD__)
#include <common/util.h>
#else
#define ARRAY_SIZE(a)    (sizeof(a) / sizeof(a[0]))
#endif

#if 0
#define WFD_PRINT(_str_, ...)                       \
  printf("[PID:%d][TID:%d][%d]" _str_ "\n", getpid(), pthread_self(), __LINE__, ##__VA_ARGS__)
#else
#define WFD_PRINT(_str_, ...)               \
  printf(_str_ "\n", ##__VA_ARGS__)
#endif

#define uintPtr unsigned long long

/* array size macros*/
#define MAX_DEVICES			1
#define MAX_BUFFER_COUNT		2
#define NUMBER_TEST_LOOPS		1
#define SIZE_DEV_ATTRIBS		3
#define DEFAULT_DISPLAY_ID		1

#define BORDER_COLOR			0x00FFFF00 /* (RRGGBBXX) */

#define MAX_PORTS_SUPPORTED		4

#define DEFAULT_RGB_PIXEL_FORMAT	WFD_FORMAT_RGBA8888

#define DEFAULT_YUV_PIXEL_FORMAT	WFD_FORMAT_NV12

struct wfd_display {
	int id;
	WFDDevice dev;
	WFDPort port;
	int width;
	int height;
	int connected;
	int type;
	int pipeline_cnt;
};

struct wfd_win {
	WFDPipeline pipeline;
	int pipeline_id;
	int format;
	int stride_pix;
	int src_x;
	int src_y;
	int src_w;
	int src_h;
	int dst_x;
	int dst_y;
	int dst_w;
	int dst_h;
	WFDEGLImage eglImage;
	win_image_t *image;
	WFDSource source;
};

/*
 * -----------------------------------------------------------------------------
 * Global variables
 * -----------------------------------------------------------------------------
 */
WFDSource source[MAX_BUFFER_COUNT];
WFDEGLImage eglImage[MAX_BUFFER_COUNT];
uint32_t numLoops = 0;
bool test_stop = false;
char ch = 0;

const unsigned hourglass_width = 100;
const unsigned hourglass_height = 100;
const unsigned char hourglass_data[] =
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
" XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX "
"  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  "
"   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX   "
"    XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX    "
"     XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX     "
"      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX      "
"       XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX       "
"        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX        "
"         XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX         "
"          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX          "
"           XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX           "
"            XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX            "
"             XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX             "
"              XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX              "
"               XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX               "
"                XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                "
"                 XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                 "
"                  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                  "
"                   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                   "
"                    XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                    "
"                     XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                     "
"                      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                      "
"                       XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                       "
"                        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                        "
"                         XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                         "
"                          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                          "
"                           XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                           "
"                            XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                            "
"                             XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                             "
"                              XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                              "
"                               XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                               "
"                                XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                                "
"                                 XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                                 "
"                                  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                                  "
"                                   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                                   "
"                                    XXXXXXXXXXXXXXXXXXXXXXXXXXXX                                    "
"                                     XXXXXXXXXXXXXXXXXXXXXXXXXX                                     "
"                                      XXXXXXXXXXXXXXXXXXXXXXXX                                      "
"                                       XXXXXXXXXXXXXXXXXXXXXX                                       "
"                                        XXXXXXXXXXXXXXXXXXXX                                        "
"                                         XXXXXXXXXXXXXXXXXX                                         "
"                                          XXXXXXXXXXXXXXXX                                          "
"                                           XXXXXXXXXXXXXX                                           "
"                                            XXXXXXXXXXXX                                            "
"                                             XXXXXXXXXX                                             "
"                                              XXXXXXXX                                              "
"                                               XXXXXX                                               "
"                                                XXXX                                                "
"                                                 XX                                                 "
"                                                 XX                                                 "
"                                                XXXX                                                "
"                                               XXXXXX                                               "
"                                              XXXXXXXX                                              "
"                                             XXXXXXXXXX                                             "
"                                            XXXXXXXXXXXX                                            "
"                                           XXXXXXXXXXXXXX                                           "
"                                          XXXXXXXXXXXXXXXX                                          "
"                                         XXXXXXXXXXXXXXXXXX                                         "
"                                        XXXXXXXXXXXXXXXXXXXX                                        "
"                                       XXXXXXXXXXXXXXXXXXXXXX                                       "
"                                      XXXXXXXXXXXXXXXXXXXXXXXX                                      "
"                                     XXXXXXXXXXXXXXXXXXXXXXXXXX                                     "
"                                    XXXXXXXXXXXXXXXXXXXXXXXXXXXX                                    "
"                                   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                                   "
"                                  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                                  "
"                                 XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                                 "
"                                XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                                "
"                               XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                               "
"                              XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                              "
"                             XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                             "
"                            XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                            "
"                           XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                           "
"                          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                          "
"                         XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                         "
"                        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                        "
"                       XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                       "
"                      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                      "
"                     XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                     "
"                    XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                    "
"                   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                   "
"                  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                  "
"                 XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                 "
"                XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                "
"               XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX               "
"              XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX              "
"             XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX             "
"            XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX            "
"           XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX           "
"          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX          "
"         XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX         "
"        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX        "
"       XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX       "
"      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX      "
"     XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX     "
"    XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX    "
"   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX   "
"  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  "
" XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX "
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

/**
 ** We won't support more than two window buffers. This enables double-
 ** buffering. Triple buffering is not usefull in our case since we are
 ** using a software rasterizer instead of a GPU.
 **/


/**
 ** Make the sliding vertical blue bar 32 pixels wide. Any number would be
 ** fine here, as long as it is no larger than the width of the window.
 **/

const int barwidth = 32;

/**
 ** The draw functions doe the rendering. They refresh all the dirty pixels
 ** using simple software rendering. The width, height, and stride are provided
 ** as arguments to allow the function to give correct results for most window
 ** sizes. The red, green, blue, and alpha arguments represent the number of
 ** bit shifts to the left necessary to get to the first bit of the red, green,
 ** blue, and alpha components respectively. The rendering is done in three
 ** parts. The first part clears the screen. This is only done once when the
 ** first frame is rendered. The second part animates the vertical blue bar.
 ** The last part draws the hourglass shape.
 **/

static void draw_rgba5551(int16_t *buffer,  /* a pointer to the pbuffer */
			  int width,        /* width of the pbuffer */
			  int height,       /* height of the pbuffer */
			  int stride,       /* number of bytes between rows */
			  int red,          /* shift offset for red component */
			  int green,        /* shift offset for green component */
			  int blue,         /* shift offset for blue component */
			  int alpha,        /* shift offset for alpha component */
			  int *rects,       /* (x, y, width, height) of 2 dirty rects */
			  unsigned char redval,
			  unsigned char greenval,
			  unsigned char blueval,
			  unsigned char alphaval)
{
	static int frame = 0;   /* frame counter used for the position of the bar */
	int16_t bg;             /* background yellow in packed format */
	int16_t bar;            /* blue color in packed format */
	int16_t grey;           /* gray color in packed format */
	int tmp;                /* use to calculate the position of the bar */
	int16_t *p;             /* points to a pixel in the buffer */
	int i;                  /* line counter */
	int j;                  /* pixel counter */
	int16_t *bufferEnd;     /* points to end of buffer */

	/* use stride expressed in pixels */
	stride >>= 1;

	/* The background colour is changeable */
	bg = ((redval & 0x1f) << red) | ((greenval & 0x1f) << green) | ((blueval & 0x1f) << blue) | ((alphaval & 0x01) << alpha);

	/* The bar is blue: alpha=ff, red=0, green=0, blue=ff */
	bar = (0x1f << blue) | (0x1 << alpha);

	/* The hourglass is gray; alpha=ff, red=a0, green=a0, blue=a0 */
	grey = (0x14 << red) | (0x14 << green) | (0x14 << blue) | (0x1 << alpha);

	/**
	 ** Because the buffer contents are preserved by eglSwapBuffers and during
	 ** the mapping process, we only have to clear the window once.
	 **/

	bufferEnd = buffer + (height * stride);
	if (frame == 0) {
		/* Loop through all lines in the buffer */
		for (i = 0, p = buffer; i < height; i++, p += stride - width) {
			/* Skip the pixels that will be covered by the vertical bar */
			p += barwidth;

			/* Do all the remaining pixels in a line */
			for (j = barwidth; j < width; j++, p++)
				/* Write yellow to the buffer */
				*p = bg;
		}
	}

	/**
	 ** We use tmp to make sure the vertical bar wraps when it touches the
	 ** right edge of the window instead of moving into oblivion.
	 **/
	tmp = frame % (width - barwidth);

	/**
	 ** When the bar wraps around, tmp will be 0 except for the first frame.
	 ** In this case, we must clear the previous bar location with yellow and
	 ** redraw and complete new bar at the left edge of the window. When we
	 ** don't wrap around, we can simply erase the column on the left side of
	 ** the bar with yellow and draw the column on the right side of the bar
	 ** blue.
	 **/

	if (tmp != 0) {
		/* Lopp through all lines in the buffer */
		for (i = 0; i < height; i++) {
			/* Clear the first pixel with yellow */
			buffer[i * stride + tmp - 1] = bg;

			/* Add blue to last pixel */
			buffer[i * stride + tmp + barwidth - 1] = bar;
		}
	} else {
		/**
		 ** When we do the first frame, there is no vertical bar on the right
		 ** side of the window to erase.
		 **/

		if (frame != 0) {
			/* clear the entire previous bar location with yellow */
			for (i = 0; i < height; i++) {
				p = buffer + i * stride + width - barwidth - 1;
				for (j = 0; j < barwidth; j++, p++)
					*p = bg;
			}
		}

		/* draw the new bar */
		for (i = 0; i < height; i++) {
			p = buffer + i * stride;
			for (j = 0; j < barwidth; j++, p++)
				*p = bar;
		}
	}

	/**
	 ** We only redraw the hourglass if the vertical bar partially intersects
	 ** it or when we draw the first frame. Because the barwidth is such that
	 ** on the first frame the bar intersects with the hourglass, we can only
	 ** test for one condition.
	 **/

	if (tmp + barwidth - 1 >= 10 && tmp < 110) {
		/* We only have to go through the lines where the hourglass is present */
		for (i = 0; i < hourglass_height; i++) {
			/* Move the pointer to the start of the hourglass */
			p = buffer + (i + 10) * stride + 10;
			for (j = 0; j < hourglass_width && j < width-10; j++, p++) {
				/* hourglass_data contains a space to indicate transparency */
				if (hourglass_data[i * hourglass_width + j] != ' ') {
					if(p < bufferEnd)
						*p = grey;
				}
			}
		}
	}

	/* we only have to update the x-axis of the dirty rects */
	if (tmp != 0) {
		rects[0] = tmp - 1;
		rects[2] = 1;
		rects[4] = tmp + barwidth - 1;
		rects[6] = 1;
	} else {
		rects[0] = 0;
		rects[2] = barwidth;
		if (frame != 0) {
			rects[4] = width - barwidth - 1;
			rects[6] = barwidth;
		} else {
			rects[4] = barwidth;
			rects[6] = width - barwidth;
		}
	}

	frame++;
}

static void draw_rgb565(int16_t *buffer,  /* a pointer to the pbuffer */
			int width,        /* width of the pbuffer */
			int height,       /* height of the pbuffer */
			int stride,       /* number of bytes between rows */
			int red,          /* shift offset for red component */
			int green,        /* shift offset for green component */
			int blue,         /* shift offset for blue component */
			int *rects,
			unsigned char redval,
			unsigned char greenval,
			unsigned char blueval)       /* (x, y, width, height) of 2 dirty rects */
{
	static int frame = 0;   /* frame counter used for the position of the bar */
	int16_t bg;             /* background yellow in packed format */
	int16_t bar;            /* blue color in packed format */
	int16_t grey;           /* gray color in packed format */
	int tmp;                /* use to calculate the position of the bar */
	int16_t *p;             /* points to a pixel in the buffer */
	int i;                  /* line counter */
	int j;                  /* pixel counter */
	int16_t *bufferEnd;     /* points to end of buffer */

	/* use stride expressed in pixels */
	stride >>= 1;

	/* The background is colour is changeable */
	bg = ((redval & 0x1f) << red) | ((greenval & 0x3f) << green) | ((blueval & 0x1f) << blue);

	/* The bar is blue: alpha=ff, red=0, green=0, blue=ff */
	bar = (0x1f << blue);

	/* The hourglass is gray; alpha=ff, red=a0, green=a0, blue=a0 */
	grey = (0x14 << red) | (0x28 << green) | (0x14 << blue);

	/**
	 ** Because the buffer contents are preserved by eglSwapBuffers and during
	 ** the mapping process, we only have to clear the window once.
	 **/

	bufferEnd = buffer + (height * stride);
	if (frame <= 0) {
		/* Loop through all lines in the buffer */
		for (i = 0, p = buffer; i < height; i++, p += stride - width) {
			/* Skip the pixels that will be covered by the vertical bar */
			p += barwidth;

			/* Do all the remaining pixels in a line */
			for (j = barwidth; j < width; j++, p++)
				/* Write yellow to the buffer */
				*p = bg;
		}
	}

	/**
	 ** We use tmp to make sure the vertical bar wraps when it touches the
	 ** right edge of the window instead of moving into oblivion.
	 **/
	tmp = frame % (width - barwidth);

	/**
	 ** When the bar wraps around, tmp will be 0 except for the first frame.
	 ** In this case, we must clear the previous bar location with yellow and
	 ** redraw and complete new bar at the left edge of the window. When we
	 ** don't wrap around, we can simply erase the column on the left side of
	 ** the bar with yellow and draw the column on the right side of the bar
	 ** blue.
	 **/

	if (tmp != 0) {
		/* Lopp through all lines in the buffer */
		for (i = 0; i < height; i++) {
			/* Clear the first pixel with yellow */
			buffer[i * stride + tmp - 1] = bg;

			/* Add blue to last pixel */
			buffer[i * stride + tmp + barwidth - 1] = bar;
		}
	} else {
		/**
		 ** When we do the first frame, there is no vertical bar on the right
		 ** side of the window to erase.
		 **/

		if (frame != 0) {
			/* clear the entire previous bar location with yellow */
			for (i = 0; i < height; i++) {
				p = buffer + i * stride + width - barwidth - 1;
				for (j = 0; j < barwidth; j++, p++)
					*p = bg;
			}
		}

		/* draw the new bar */
		for (i = 0; i < height; i++) {
			p = buffer + i * stride;
			for (j = 0; j < barwidth; j++, p++)
				*p = bar;
		}
	}

	/**
	 ** We only redraw the hourglass if the vertical bar partially intersects
	 ** it or when we draw the first frame. Because the barwidth is such that
	 ** on the first frame the bar intersects with the hourglass, we can only
	 ** test for one condition.
	 **/

	if (tmp + barwidth - 1 >= 10 && tmp < 110) {
		/* We only have to go through the lines where the hourglass is present */
		for (i = 0; i < hourglass_height; i++) {
			/* Move the pointer to the start of the hourglass */
			p = buffer + (i + 10) * stride + 10;
			for (j = 0; j < hourglass_width && j < width-10; j++, p++) {
				/* hourglass_data contains a space to indicate transparency */
				if (hourglass_data[i * hourglass_width + j] != ' ') {
					if(p < bufferEnd)
						*p = grey;
				}
			}
		}
	}

	/* we only have to update the x-axis of the dirty rects */
	if (tmp != 0) {
		rects[0] = tmp - 1;
		rects[2] = 1;
		rects[4] = tmp + barwidth - 1;
		rects[6] = 1;
	} else {
		rects[0] = 0;
		rects[2] = barwidth;
		if (frame != 0) {
			rects[4] = width - barwidth - 1;
			rects[6] = barwidth;
		} else {
			rects[4] = barwidth;
			rects[6] = width - barwidth;
		}
	}

	frame++;
}

static void draw_rgba8888(int32_t *buffer,  /* a pointer to the pbuffer */
			  int width,        /* width of the pbuffer */
			  int height,       /* height of the pbuffer */
			  int stride,       /* number of bytes between rows */
			  int red,          /* shift offset for red component */
			  int green,        /* shift offset for green component */
			  int blue,         /* shift offset for blue component */
			  int alpha,        /* shift offset for alpha component */
			  int *rects,       /* (x, y, width, height) of 2 dirty rects */
			  unsigned char redval,
			  unsigned char greenval,
			  unsigned char blueval,
			  unsigned char alphaval)
{
	static int frame = 0;   /* frame counter used for the position of the bar */
	int32_t bg;             /* background yellow in packed format */
	int32_t bar;            /* blue color in packed format */
	int32_t grey;           /* gray color in packed format */
	int tmp;                /* use to calculate the position of the bar */
	int32_t *p;             /* points to a pixel in the buffer */
	int i;                  /* line counter */
	int j;                  /* pixel counter */
	int32_t *bufferEnd;     /* points to end of buffer */

	/* use stride expressed in pixels */
	stride >>= 2;

	/* background has changeable rbga values */
	bg = (redval << red) | (greenval << green) | (blueval << blue) | (alphaval << alpha);

	/* The bar is blue: alpha=ff, red=0, green=0, blue=ff */
	bar = (0xff << blue) | (0xff << alpha);

	/* The hourglass is gray; alpha=ff, red=a0, green=a0, blue=a0 */
	grey = (0xa0 << red) | (0xa0 << green) | (0xa0 << blue) | (0xff << alpha);

	/**
	 ** Because the buffer contents are preserved by eglSwapBuffers and during
	 ** the mapping process, we only have to clear the window once.
	 **/

	bufferEnd = buffer + (height * stride);
	if (frame == 0) {
		/* Loop through all lines in the buffer */
		for (i = 0, p = buffer; i < height; i++, p += stride - width) {
			/* Skip the pixels that will be covered by the vertical bar */
			p += barwidth;

			/* Do all the remaining pixels in a line */
			for (j = barwidth; j < width; j++, p++)
				/* Write yellow to the buffer */
				*p = bg;
		}
	}

	/**
	 ** We use tmp to make sure the vertical bar wraps when it touches the
	 ** right edge of the window instead of moving into oblivion.
	 **/
	tmp = frame % (width - barwidth);

	/**
	 ** When the bar wraps around, tmp will be 0 except for the first frame.
	 ** In this case, we must clear the previous bar location with yellow and
	 ** redraw and complete new bar at the left edge of the window. When we
	 ** don't wrap around, we can simply erase the column on the left side of
	 ** the bar with yellow and draw the column on the right side of the bar
	 ** blue.
	 **/

	if (tmp != 0) {
		/* Lopp through all lines in the buffer */
		for (i = 0; i < height; i++) {
			/* Clear the first pixel with yellow */
			buffer[i * stride + tmp - 1] = bg;

			/* Add blue to last pixel */
			buffer[i * stride + tmp + barwidth - 1] = bar;
		}
	} else {
		/**
		 ** When we do the first frame, there is no vertical bar on the right
		 ** side of the window to erase.
		 **/

		if (frame != 0) {
			/* clear the entire previous bar location with yellow */
			for (i = 0; i < height; i++) {
				p = buffer + i * stride + width - barwidth - 1;
				for (j = 0; j < barwidth; j++, p++)
					*p = bg;
			}
		}

		/* draw the new bar */
		for (i = 0; i < height; i++) {
			p = buffer + i * stride;
			for (j = 0; j < barwidth; j++, p++)
				*p = bar;
		}
	}

	/**
	 ** We only redraw the hourglass if the vertical bar partially intersects
	 ** it or when we draw the first frame. Because the barwidth is such that
	 ** on the first frame the bar intersects with the hourglass, we can only
	 ** test for one condition.
	 **/

	if (tmp + barwidth - 1 >= 10 && tmp < 110) {
		/* We only have to go through the lines where the hourglass is present */
		for (i = 0; i < hourglass_height; i++) {
			/* Move the pointer to the start of the hourglass */
			p = buffer + (i + 10) * stride + 10;
			for (j = 0; j < hourglass_width && j < width-10; j++, p++) {
				/* hourglass_data contains a space to indicate transparency */
				if (hourglass_data[i * hourglass_width + j] != ' ') {
					if(p < bufferEnd)
						*p = grey;
				}
			}
		}
	}

	/* we only have to update the x-axis of the dirty rects */
	if (tmp != 0) {
		rects[0] = tmp - 1;
		rects[2] = 1;
		rects[4] = tmp + barwidth - 1;
		rects[6] = 1;
	} else {
		rects[0] = 0;
		rects[2] = barwidth;
		if (frame != 0) {
			rects[4] = width - barwidth - 1;
			rects[6] = barwidth;
		} else {
			rects[4] = barwidth;
			rects[6] = width - barwidth;
		}
	}

	frame++;
}

static void terminate_handle(int sig)
{
	WFD_PRINT("WFD VSYNC terminated by sig %d...", sig);

	test_stop = true;
}

static void test_exit(struct wfd_display *display, struct wfd_win *win)
{
	int i;

	WFD_PRINT("Before exit, press Enter to continue ...");
	ch = getchar();
	ch = ch;

	if (display->dev) {
		/* wfdDestroyPipeline has internal commit */
		if (win->pipeline)
			wfdDestroyPipeline(display->dev, win->pipeline);


		for (i = 0; i < MAX_BUFFER_COUNT; i++) {
			if (source[i])
				wfdDestroySource(display->dev, source[i]);

			if (eglImage[i])
				wfdDestroyWFDEGLImages(display->dev, 1, &(eglImage[i]));
		}

		if (display->port)
			wfdDestroyPort(display->dev, display->port);

		wfdDestroyDevice(display->dev);
		display->dev = 0;
	}

	if (1 == NUMBER_TEST_LOOPS) {
		WFD_PRINT("Test done, press Enter to exit ...");
		ch = getchar();
		ch = ch;
	}

	return;
}

static void main_draw(void *pointer, int w, int h, int stride, int format)
{
	/**
	 ** This is the size for an invisible exit button. We choose a value that's
	 ** big enough to be useable with touchscreens and pointer devices.
	 **/

	int size[2];                      /* size of the window on screen */
	// int ndisplays;                    /* number of displays */
	int rects[8];                     /* stores up to 2 dirty rectangles */
	unsigned int bg_rgba[4] = {0xff, 0xff, 0x0, 0xff}; /* stores rgba values from argument */
	static uint64_t last_t;
	static int frames = 0;
	struct timespec to;
	uint64_t t, delta;

	size[0] = w;
	size[1] = h;

	switch (format) {
		case WFD_FORMAT_RGBA8888:
			draw_rgba8888(pointer, size[0], size[1], stride, 16, 8, 0, 24, rects, bg_rgba[0], bg_rgba[1], bg_rgba[2], bg_rgba[3]);
			break;
		case WFD_FORMAT_RGBX8888:
			draw_rgba8888(pointer, size[0], size[1], stride, 16, 8, 0, 24, rects, bg_rgba[0], bg_rgba[1], bg_rgba[2], 0x00);
			break;
		case WFD_FORMAT_RGBA5551:
			draw_rgba5551(pointer, size[0], size[1], stride, 10, 5, 0, 15, rects, bg_rgba[0], bg_rgba[1], bg_rgba[2], bg_rgba[3]);
			break;
		case WFD_FORMAT_RGBX5551:
			draw_rgba5551(pointer, size[0], size[1], stride, 10, 5, 0, 15, rects, bg_rgba[0], bg_rgba[1], bg_rgba[2], 0x00);
			break;
		case WFD_FORMAT_RGB565:
			draw_rgb565(pointer, size[0], size[1], stride, 11, 5, 0, rects, bg_rgba[0], bg_rgba[1], bg_rgba[2]);
			break;
	}

	clock_gettime(CLOCK_REALTIME, &to);
	t = timespec2nsec(&to);

	if (frames == 0) {
		last_t = t;
	} else {
		delta = t - last_t;
		if (frames && delta >= 5000000000LL) {
			printf("%d frames in %6.3f seconds = %6.3f FPS\n",
				frames, 0.000000001f * delta,
				1000000000.0f * frames / delta);
			fflush(stdout);
			frames = -1;
		}
	}

	frames++;
}

struct util_color_component {
	unsigned int length;
	unsigned int offset;
};

struct util_rgb_info {
	struct util_color_component red;
	struct util_color_component green;
	struct util_color_component blue;
	struct util_color_component alpha;
};

enum util_yuv_order {
	YUV_YCbCr = 1,
	YUV_YCrCb = 2,
	YUV_YC = 4,
	YUV_CY = 8,
};

struct util_yuv_info {
	enum util_yuv_order order;
	unsigned int xsub;
	unsigned int ysub;
	unsigned int chroma_stride;
};

struct util_format_info {
	uint32_t format;
	const char *name;
	const struct util_rgb_info rgb;
	const struct util_yuv_info yuv;
};

#define MAKE_RGB_INFO(rl, ro, gl, go, bl, bo, al, ao) \
	.rgb = { { (rl), (ro) }, { (gl), (go) }, { (bl), (bo) }, { (al), (ao) } }

#define MAKE_YUV_INFO(order, xsub, ysub, chroma_stride) \
	.yuv = { (order), (xsub), (ysub), (chroma_stride) }

static const struct util_format_info format_info[] = {
	/* Indexed */
	/* YUV packed */
	{ WFD_FORMAT_UYVY, "UYVY", MAKE_YUV_INFO(YUV_YCbCr | YUV_CY, 2, 2, 2) },
	{ WFD_FORMAT_YVYU, "YVYU", MAKE_YUV_INFO(YUV_YCrCb | YUV_YC, 2, 2, 2) },
	/* YUV semi-planar */
	{ WFD_FORMAT_NV12, "NV12", MAKE_YUV_INFO(YUV_YCbCr, 2, 2, 2) },
	{ WFD_FORMAT_NV16, "NV16", MAKE_YUV_INFO(YUV_YCbCr, 2, 1, 2) },
		/* YUV planar */
	{ WFD_FORMAT_YUV420, "YU12", MAKE_YUV_INFO(YUV_YCbCr, 2, 2, 1) },
	/* YUV420_8BIT/YUV420_10BIT only used for AFBC test now */
	/* YUV422_10BIT for AFBC test */
	/* RGB16 */
	{ WFD_FORMAT_RGB565, "RG16", MAKE_RGB_INFO(5, 11, 6, 5, 5, 0, 0, 0) },
		/* RGB24 */
	{ WFD_FORMAT_RGB888, "RG24", MAKE_RGB_INFO(8, 16, 8, 8, 8, 0, 0, 0) },
	/* RGB32 */
	{ WFD_FORMAT_RGBA8888, "AR24", MAKE_RGB_INFO(8, 16, 8, 8, 8, 0, 8, 24) },
	{ WFD_FORMAT_RGBX8888, "XR24", MAKE_RGB_INFO(8, 16, 8, 8, 8, 0, 0, 0) },
	{ WFD_FORMAT_BGRA8888, "AB24", MAKE_RGB_INFO(8, 0, 8, 8, 8, 16, 8, 24) },
	{ WFD_FORMAT_BGRX8888, "XB24", MAKE_RGB_INFO(8, 0, 8, 8, 8, 16, 0, 0) },
	{ WFD_FORMAT_RGBA1010102, "AR30", MAKE_RGB_INFO(10, 20, 10, 10, 10, 0, 2, 30) },
	{ WFD_FORMAT_RGBX1010102, "XR30", MAKE_RGB_INFO(10, 20, 10, 10, 10, 0, 0, 0) },
	{ WFD_FORMAT_BGRA1010102, "AB30", MAKE_RGB_INFO(10, 0, 10, 10, 10, 20, 2, 30) },
	{ WFD_FORMAT_BGRX1010102, "XB30", MAKE_RGB_INFO(10, 0, 10, 10, 10, 20, 0, 0) },
};

uint32_t util_format_fourcc(const char *name)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(format_info); i++)
		if (!strcmp(format_info[i].name, name))
			return format_info[i].format;

	return 0;
}

/*
 * argc: -display=id:wxh
*/
static int wfd_parse_display(struct wfd_display *display, char *arg)
{
	char *p;
	char *end;

	p = arg + strlen("-display=");
	display->id = strtoul(p, &end, 10);
	if (*end != ':') {
		printf("missing ':' after display id\n");
		goto error;
	}
	p = end + 1;
	display->width = strtoul(p, &end, 10);
	if (*end != 'x') {
		printf("missing 'x' after width\n");
		goto error;
	}
	p = end + 1;
	display->height = strtoul(p, &end, 10);

	printf("display%d %d x %d\n", display->id, display->width, display->height);
error:
	return 0;
}


/*
 * argc: -pipeline=id:wxh[@format]
*/
static int wfd_parse_win(struct wfd_win *win, char *arg)
{
	char *p;
	char *end;

	win->format = WFD_FORMAT_BGRA8888;
	p = arg + strlen("-pipeline=");
	win->pipeline_id = strtoul(p, &end, 10);
        if (*end != ':') {
		printf("missing ':' after pipeline id\n");
                goto error;
	}
        p = end + 1;
        win->src_w = strtoul(p, &end, 10);
        if (*end != 'x') {
		printf("missing 'x' after pipeline id\n");
		goto error;
	}
	p = end + 1;
	win->src_h = strtoul(p, &end, 10);
	win->dst_w = win->src_w;
	win->dst_h = win->src_h;

        if (*end == '@') {
		win->format = util_format_fourcc(end+1);
		if (!win->format)
			printf("unknow format: %s\n", end + 1);
	}
	printf("pipeline%ld [%d x %d]->[%d x %d] format: %d\n",
		win->pipeline, win->src_w, win->src_h, win->dst_w, win->dst_h,
		win->format);
error:
	return 0;
}

#if defined(__RT_THREAD__)
int wfd_vsync(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	WFDint             numDevs         = 0;
	WFDint             size            = 0;
	WFDint             numPorts        = 0;
	WFDint             numPipelines    = 0;
	WFDint             numPortModes    = 0;
	WFDint             phy_width       = 0;
	WFDint             phy_height      = 0;
	WFDint             rect[4]         = { 0, 0, 0, 0 };
	WFDint             count           = 0;
	//WFDint             pipelineIdYUV       = 0;
	WFDint             iBorderColor    = BORDER_COLOR;
	WFDint             attribValue     = 0;
	WFDErrorCode       eError          = WFD_ERROR_NONE;

	WFDPipeline        commitPipe      = 0;
	WFDSource          commitSource    = 0;
	WFDint             usage           = WFD_USAGE_WRITE;

	win_image_t       *image[MAX_BUFFER_COUNT];

	WFDint             devIds[1];
	WFDint             portIds[4];
	WFDint             pipelineIds[8];
	WFDPortMode        portModes[MAX_PORTS_SUPPORTED];

	WFDint i;
	struct wfd_display *display;
	struct wfd_win *win;

	void *pbuf;        /* pixel buffer for draw */

	WFD_PRINT("OpenWF VSYNC Start....");

	signal(SIGQUIT, terminate_handle);
	signal(SIGINT,  terminate_handle);
	signal(SIGTERM, terminate_handle);

	memset((void *)devIds, 0x00, sizeof(WFDint) * MAX_DEVICES);
	display = calloc(1, sizeof(*display));
	win = calloc(1, sizeof(*win));
	if (!win || !display) {
		printf("alloc display/win failed\n");
		return 0;
	}

	for (i = 1; i < argc; i++) {
		if (strncmp(argv[i], "-display=", strlen("-display=")) == 0) {
		/**
		 ** The syntax of the display option is -display=(number)
		 **/
			wfd_parse_display(display, argv[i]);
		} else if (strncmp(argv[i], "-pipeline=", strlen("-pipeline=")) == 0) {
			wfd_parse_win(win, argv[i]);
		}
	}

	if (!display->id)
		display->id = DEFAULT_DISPLAY_ID;
	if (!win->format)
		win->format = DEFAULT_RGB_PIXEL_FORMAT;

	/* Get list of devices */
	numDevs = wfdEnumerateDevices(NULL, 0, NULL);
	WFD_PRINT("numDevs=%ld", numDevs);
	if (numDevs) {
		wfdEnumerateDevices(devIds, numDevs, NULL);
		for (count = 0; count < numDevs; count++) {
			WFD_PRINT("devIds[%ld]=%ld", count, devIds[count]);
		}
	}

	/* Create default device */
	display->dev = wfdCreateDevice(WFD_DEFAULT_DEVICE_ID, NULL);
	if (display->dev == WFD_INVALID_HANDLE) {
		WFD_PRINT("Create wfd devce failed");
		goto out;
	}

	/* Read a device attribute */
	attribValue = wfdGetDeviceAttribi(display->dev, WFD_DEVICE_ID);
	attribValue = attribValue;
	WFD_PRINT("wfdGetDeviceAttribi(WFD_DEVICE_ID)=%ld", attribValue);

	/* Get list of available ports */
	numPorts = wfdEnumeratePorts(display->dev, NULL, 0, NULL);
	if (numPorts <= 0) {
		WFD_PRINT("wfdEnumeratePorts failed: numPorts=%ld", numPorts);
		goto out;
	} else {
		WFD_PRINT("numPorts=%ld", numPorts);
	}

	size = wfdEnumeratePorts(display->dev, portIds, numPorts, NULL);
	for (count = 0; count < size; count++)
		WFD_PRINT("portIds[%ld]=%ld", count, portIds[count]);

	WFD_PRINT("Creating port%d", display->id);

	display->port = wfdCreatePort(display->dev, portIds[display->id - 1], NULL);
	if (display->port == WFD_INVALID_HANDLE) {
		WFD_PRINT("wfdCreatePort failed");
		goto out;
	}

	/* Get port modes */
	numPortModes = wfdGetPortModes(display->dev, display->port, NULL, 0);
	WFD_PRINT("numPortModes=%ld", numPortModes);
	eError = wfdGetError(display->dev);
	if (eError != WFD_ERROR_NONE) {
		WFD_PRINT("wfdGetPortModes eError=0x%08x", eError);
		goto out;
	}

	size = wfdGetPortModes(display->dev, display->port, portModes, numPortModes);
	for (count = 0; count < size; count++)
		WFD_PRINT("portModes[%ld]=%ld", count, (WFDint)((uintPtr)(portModes[count])));

	if ((numPortModes > 0) && (display->width > 0)) {
		for (count = 0; count < numPortModes; count++) {
			WFDint  width        = 0;
			WFDint  height       = 0;
			WFDint  refresh_rate = 0;
			WFDint  interlace    = false;

			width        = wfdGetPortModeAttribi(display->dev, display->port, portModes[count],
					WFD_PORT_MODE_WIDTH);
			height       = wfdGetPortModeAttribi(display->dev, display->port, portModes[count],
					WFD_PORT_MODE_HEIGHT);
			refresh_rate = wfdGetPortModeAttribi(display->dev, display->port, portModes[count],
					WFD_PORT_MODE_REFRESH_RATE);
			interlace    = wfdGetPortModeAttribi(display->dev, display->port, portModes[count],
					WFD_PORT_MODE_INTERLACED);

			WFD_PRINT("portModes[%ld]: %ldx%ld%s%ld", count, width, height, interlace ? "i" : "p", refresh_rate);
			if ((display->width == width) && (display->height == height))
				break;
		}

		/* use the default mode (count=0) if no modes match */
		count = ((count == numPortModes) ? (0) : (count));
	} else {
		count = 0;
	}

	/* Set port mode */
	wfdSetPortMode(display->dev, display->port, portModes[count]);
	eError = wfdGetError(display->dev);
	if (eError != WFD_ERROR_NONE) {
		WFD_PRINT("wfdSetPortMode eError=0x%08x", eError);
		goto out;
	}

	/* Retrieve the width and height information from port attributes */
	phy_width = wfdGetPortModeAttribi(display->dev, display->port, portModes[count], WFD_PORT_MODE_WIDTH);
	eError = wfdGetError(display->dev);
	WFD_PRINT("phy_width=%ld eError=0x%08x", phy_width, eError);

	phy_height = wfdGetPortModeAttribi(display->dev, display->port, portModes[count], WFD_PORT_MODE_HEIGHT);
	eError = wfdGetError(display->dev);
	WFD_PRINT("phy_height=%ld eError=0x%08x", phy_height, eError);

	if (!win->src_w) {
		win->src_w = phy_width;
		win->dst_w = win->src_w;
	}

	if (!win->src_h) {
		win->src_h = phy_height;
		win->dst_h = win->src_h;
	}

	/* Set port power mode */
	wfdSetPortAttribi(display->dev, display->port, WFD_PORT_POWER_MODE, WFD_POWER_MODE_ON);
	eError = wfdGetError(display->dev);
	if (eError != WFD_ERROR_NONE) {
		WFD_PRINT("wfdSetPortAttribi(WFD_PORT_POWER_MODE) " "eError=0x%08x", eError);
		goto out;
	}

	/* Set background color */
	wfdSetPortAttribi(display->dev, display->port, WFD_PORT_BACKGROUND_COLOR, iBorderColor);
	eError = wfdGetError(display->dev);
	if (eError != WFD_ERROR_NONE)
		WFD_PRINT("wfdSetPortAttribi(WFD_PORT_BACKGROUND_COLOR) " "eError=0x%08x", eError);

	/* Get list of pipelines */
	numPipelines = wfdGetPortAttribi(display->dev, display->port, WFD_PORT_PIPELINE_ID_COUNT);
	WFD_PRINT("numPipelines=%ld", numPipelines);

	wfdGetPortAttribiv(display->dev, display->port, WFD_PORT_BINDABLE_PIPELINE_IDS, numPipelines, pipelineIds);
	for (count = 0; count < numPipelines; count++)
		WFD_PRINT("pipelineIds[%ld]=%ld", count, pipelineIds[count]);

	/*
	 * assume the second pipeline is Esmart that support yuv
	 * How to do  pixel format check for each pipeline?
	 */
	if (!win->pipeline_id)
		win->pipeline_id = pipelineIds[0];

	/* Create pipelineRGB */
	win->pipeline = wfdCreatePipeline(display->dev, win->pipeline_id, NULL);
	if (WFD_INVALID_HANDLE == win->pipeline) {
		WFD_PRINT("wfdCreatePipeline id: %d failed", win->pipeline_id);
		goto out;
	}

	/* Bind pipelineRGB to port */
	wfdBindPipelineToPort(display->dev, display->port, win->pipeline);
	eError = wfdGetError(display->dev);
	if (eError != WFD_ERROR_NONE) {
		WFD_PRINT("wfdBindPipelineToPort eError=0x%08x", eError);
		goto out;
	}

	for (i= 0; i < MAX_BUFFER_COUNT; i++) {
		/* Create RGB EGL image */
		wfdCreateWFDEGLImages(display->dev, win->src_w, win->src_h, win->format, usage, 1, &(eglImage[i]));
		eError = wfdGetError(display->dev);
		if (eError != WFD_ERROR_NONE) {
			WFD_PRINT("wfdCreateWFDEGLImages eError=0x%08x", eError);
			goto out;
		}

		image[i] = (win_image_t *)(eglImage[i]);
		if (!image[i]) {
			WFD_PRINT("eglImage%ld is NULL", i);
			goto out;
		} else {
			WFD_PRINT("eglImage%ld vaddr=0x%p size=%d", i, image[i]->vaddr, image[i]->size);
		}
		/* Create RGB source */
		source[i] = wfdCreateSourceFromImage(display->dev, win->pipeline, image[i], NULL);
		if (source[i] == WFD_INVALID_HANDLE) {
			WFD_PRINT("wfdCreate source1 failed");
			goto out;
		}
	}

	pbuf = calloc(1, image[0]->size);
	if (!pbuf) {
		WFD_PRINT("alloc pixel buffer failed");
		goto out;
	}

	if (1 == NUMBER_TEST_LOOPS) {
		WFD_PRINT("before commit power on, " "press Enter to continue ...");
		ch = getchar();
		ch=ch;
	}

	wfdDeviceCommit(display->dev, WFD_COMMIT_ENTIRE_PORT, display->port);
	eError = wfdGetError(display->dev);
	if (eError != WFD_ERROR_NONE) {
		WFD_PRINT("wfdDeviceCommit(WFD_COMMIT_ENTIRE_PORT) " "eError=0x%08x", eError);
		goto out;
	}

	if (1 == NUMBER_TEST_LOOPS) {
		WFD_PRINT("wfdDeviceCommit power on ok, " "press Enter to continue ...");
		ch = getchar();
		ch=ch;
	}

	do {
		commitPipe    = win->pipeline;
		main_draw(pbuf, win->src_w, win->src_h, image[0]->strides[0], win->format);
		i = numLoops % MAX_BUFFER_COUNT;
		commitSource  = source[i];

		memcpy(image[i]->vaddr, pbuf, image[i]->size);
		if (numLoops == 0) {
			/* Apply source 1 */
			rect[0] = 0;
			rect[1] = 0;
			rect[2] = win->src_w;
			rect[3] = win->src_h;
			wfdSetPipelineAttribiv(display->dev, commitPipe, WFD_PIPELINE_SOURCE_RECTANGLE, 4, rect);
			eError = wfdGetError(display->dev);
			if (eError != WFD_ERROR_NONE) {
				WFD_PRINT("wfdSetPipelineAttribiv(WFD_PIPELINE_SOURCE_RECTANGLE) " "eError=0x%08x", eError);
				goto out;
			}

			/* Place the image in the centre (horizontally), along the top edge */
			rect[0] = (phy_width - win->dst_w) >> 1;
			rect[1] = 0;
			rect[2] = win->dst_w;
			rect[3] = win->dst_h;

			wfdSetPipelineAttribiv(display->dev, commitPipe, WFD_PIPELINE_DESTINATION_RECTANGLE, 4, rect);
			eError = wfdGetError(display->dev);
			if (eError != WFD_ERROR_NONE) {
				WFD_PRINT("wfdSetPipelineAttribiv(WFD_PIPELINE_DESTINATION_RECTANGLE) " "eError=0x%08x", eError);
				goto out;
			}
		}

		wfdBindSourceToPipeline(display->dev, commitPipe, commitSource, WFD_TRANSITION_AT_VSYNC, NULL);
		eError = wfdGetError(display->dev);
		if (eError != WFD_ERROR_NONE) {
			WFD_PRINT("wfdBindSourceToPipeline eError=0x%08x", eError);
			goto out;
		}

		/* Commit port */
		wfdDeviceCommit(display->dev, WFD_COMMIT_ENTIRE_PORT, display->port);
		eError = wfdGetError(display->dev);
		if (eError != WFD_ERROR_NONE) {
			WFD_PRINT("wfdDeviceCommit(WFD_COMMIT_ENTIRE_PORT) " "eError=0x%08x", eError);
			goto out;
		}

	} while (++numLoops && !test_stop);

	WFD_PRINT("wfd_vsync stop...");

	wfdBindSourceToPipeline(display->dev, commitPipe, WFD_INVALID_HANDLE, WFD_TRANSITION_AT_VSYNC, NULL);
	eError = wfdGetError(display->dev);
	if (eError != WFD_ERROR_NONE)
		WFD_PRINT("wfdBindSourceToPipeline eError=0x%08x", eError);

	/* Power port down */
	wfdSetPortAttribi(display->dev, display->port, WFD_PORT_POWER_MODE, WFD_POWER_MODE_OFF);
	eError = wfdGetError(display->dev);
	if (eError != WFD_ERROR_NONE)
		WFD_PRINT("wfdSetPortAttribi(WFD_PORT_POWER_MODE) " "eError=0x%08x", eError);

	wfdDeviceCommit(display->dev, WFD_COMMIT_ENTIRE_PORT, display->port);
	eError = wfdGetError(display->dev);
	if (eError != WFD_ERROR_NONE)
		WFD_PRINT("wfdDeviceCommit(WFD_COMMIT_ENTIRE_PORT) " "eError=0x%08x", eError);

out:
	test_exit(display, win);
	free(win);
	free(display);
	return 0;
}

#if defined(__RT_THREAD__) && defined(RT_USING_FINSH)
#include <finsh.h>
MSH_CMD_EXPORT(wfd_vsync, wfd vsync test. e.g: wfd_vsync);
#endif
