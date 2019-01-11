/*********************************************************************
*
* Software License Agreement (BSD License)
*
*  Copyright (c) 2009, Robert Bosch LLC.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Robert Bosch nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
*********************************************************************/
#define __STDC_CONSTANT_MACROS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */

extern "C" {
#include <linux/videodev2.h>
	//#include <libavcodec/avcodec.h>
	//#include <libswscale/swscale.h>
}

#include <usb_cam/usb_cam.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

struct buffer {
	void * start;
	size_t length;
};

static char *camera_dev;
static unsigned int pixelformat = 0;
static usb_cam_io_method io = IO_METHOD_MMAP;
static int fd = -1;
struct buffer * buffers = NULL;
static unsigned int n_buffers = 0;
//static AVFrame *avframe_camera = NULL;
//static AVFrame *avframe_rgb = NULL;
//static AVCodec *avcodec = NULL;
//static AVCodecContext *avcodec_context = NULL;
//static int avframe_camera_size = 0;
//static int avframe_rgb_size = 0;

//struct SwsContext *video_sws = NULL;

static void errno_exit(const char * s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));

	//exit(EXIT_FAILURE);
}

static int xioctl(int fd, int request, void * arg)
{
	int r;

	do
	r = ioctl(fd, request, arg); while (-1==r&&EINTR==errno);

	return r;
}

const static unsigned char dht_data[] = {
  0xff, 0xc4, 0x01, 0xa2, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
  0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x01, 0x00, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
  0x0a, 0x0b, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05,
  0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d, 0x01, 0x02, 0x03, 0x00, 0x04,
  0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22,
  0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15,
  0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36,
  0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
  0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66,
  0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
  0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95,
  0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8,
  0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2,
  0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5,
  0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
  0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
  0xfa, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05,
  0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04,
  0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22,
  0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33,
  0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25,
  0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36,
  0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
  0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66,
  0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
  0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94,
  0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
  0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba,
  0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
  0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
  0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa
};

const unsigned char uchar_clipping_table[] = {
	0,0,0,0,0,0,0,0, // -128 - -121
	0,0,0,0,0,0,0,0, // -120 - -113
	0,0,0,0,0,0,0,0, // -112 - -105
	0,0,0,0,0,0,0,0, // -104 -  -97
	0,0,0,0,0,0,0,0, //  -96 -  -89
	0,0,0,0,0,0,0,0, //  -88 -  -81
	0,0,0,0,0,0,0,0, //  -80 -  -73
	0,0,0,0,0,0,0,0, //  -72 -  -65
	0,0,0,0,0,0,0,0, //  -64 -  -57
	0,0,0,0,0,0,0,0, //  -56 -  -49
	0,0,0,0,0,0,0,0, //  -48 -  -41
	0,0,0,0,0,0,0,0, //  -40 -  -33
	0,0,0,0,0,0,0,0, //  -32 -  -25
	0,0,0,0,0,0,0,0, //  -24 -  -17
	0,0,0,0,0,0,0,0, //  -16 -   -9
	0,0,0,0,0,0,0,0, //   -8 -   -1
	0,1,2,3,4,5,6,7,
	8,9,10,11,12,13,14,15,
	16,17,18,19,20,21,22,23,
	24,25,26,27,28,29,30,31,
	32,33,34,35,36,37,38,39,
	40,41,42,43,44,45,46,47,
	48,49,50,51,52,53,54,55,
	56,57,58,59,60,61,62,63,
	64,65,66,67,68,69,70,71,
	72,73,74,75,76,77,78,79,
	80,81,82,83,84,85,86,87,
	88,89,90,91,92,93,94,95,
	96,97,98,99,100,101,102,103,
	104,105,106,107,108,109,110,111,
	112,113,114,115,116,117,118,119,
	120,121,122,123,124,125,126,127,
	128,129,130,131,132,133,134,135,
	136,137,138,139,140,141,142,143,
	144,145,146,147,148,149,150,151,
	152,153,154,155,156,157,158,159,
	160,161,162,163,164,165,166,167,
	168,169,170,171,172,173,174,175,
	176,177,178,179,180,181,182,183,
	184,185,186,187,188,189,190,191,
	192,193,194,195,196,197,198,199,
	200,201,202,203,204,205,206,207,
	208,209,210,211,212,213,214,215,
	216,217,218,219,220,221,222,223,
	224,225,226,227,228,229,230,231,
	232,233,234,235,236,237,238,239,
	240,241,242,243,244,245,246,247,
	248,249,250,251,252,253,254,255,
	255,255,255,255,255,255,255,255, // 256-263
	255,255,255,255,255,255,255,255, // 264-271
	255,255,255,255,255,255,255,255, // 272-279
	255,255,255,255,255,255,255,255, // 280-287
	255,255,255,255,255,255,255,255, // 288-295
	255,255,255,255,255,255,255,255, // 296-303
	255,255,255,255,255,255,255,255, // 304-311
	255,255,255,255,255,255,255,255, // 312-319
	255,255,255,255,255,255,255,255, // 320-327
	255,255,255,255,255,255,255,255, // 328-335
	255,255,255,255,255,255,255,255, // 336-343
	255,255,255,255,255,255,255,255, // 344-351
	255,255,255,255,255,255,255,255, // 352-359
	255,255,255,255,255,255,255,255, // 360-367
	255,255,255,255,255,255,255,255, // 368-375
	255,255,255,255,255,255,255,255, // 376-383
};
const int clipping_table_offset = 128;

/** Clip a value to the range 0<val<255. For speed this is done using an
* array, so can only cope with numbers in the range -128<val<383.
*/
static unsigned char
CLIPVALUE(int val)
{
	// Old method (if)
	/*   val = val < 0 ? 0 : val; */
	/*   return val > 255 ? 255 : val; */

	// New method (array)
	return uchar_clipping_table[val+clipping_table_offset];
}

/**
* Conversion from YUV to RGB.
* The normal conversion matrix is due to Julien (surname unknown):
*
* [ R ]   [  1.0   0.0     1.403 ] [ Y ]
* [ G ] = [  1.0  -0.344  -0.714 ] [ U ]
* [ B ]   [  1.0   1.770   0.0   ] [ V ]
*
* and the firewire one is similar:
*
* [ R ]   [  1.0   0.0     0.700 ] [ Y ]
* [ G ] = [  1.0  -0.198  -0.291 ] [ U ]
* [ B ]   [  1.0   1.015   0.0   ] [ V ]
*
* Corrected by BJT (coriander's transforms RGB->YUV and YUV->RGB
*                   do not get you back to the same RGB!)
* [ R ]   [  1.0   0.0     1.136 ] [ Y ]
* [ G ] = [  1.0  -0.396  -0.578 ] [ U ]
* [ B ]   [  1.0   2.041   0.002 ] [ V ]
*
*/
static void
YUV2RGB(const unsigned char y,
		const unsigned char u,
		const unsigned char v,
		unsigned char* r,
		unsigned char* g,
		unsigned char* b)
{
	const int y2=(int)y;
	const int u2=(int)u-128;
	const int v2=(int)v-128;
	//std::cerr << "YUV=("<<y2<<","<<u2<<","<<v2<<")"<<std::endl;


	// This is the normal YUV conversion, but
	// appears to be incorrect for the firewire cameras
	//   int r2 = y2 + ( (v2*91947) >> 16);
	//   int g2 = y2 - ( ((u2*22544) + (v2*46793)) >> 16 );
	//   int b2 = y2 + ( (u2*115999) >> 16);
	// This is an adjusted version (UV spread out a bit)
	int r2 = y2 + ( (v2*37221) >> 15);
	int g2 = y2 - ( ((u2*12975) + (v2*18949)) >> 15 );
	int b2 = y2 + ( (u2*66883) >> 15);
	//std::cerr << "   RGB=("<<r2<<","<<g2<<","<<b2<<")"<<std::endl;


	// Cap the values.
	*r=CLIPVALUE(r2);
	*g=CLIPVALUE(g2);
	*b=CLIPVALUE(b2);
}

void
uyvy2rgb (char *YUV, char *RGB, int NumPixels) {
	int i, j;
	unsigned char y0, y1, u, v;
	unsigned char r, g, b;
	for (i = 0, j = 0; i < (NumPixels << 1); i += 4, j += 6)
	{
		u = (unsigned char) YUV[i + 0];
		y0 = (unsigned char) YUV[i + 1];
		v = (unsigned char) YUV[i + 2];
		y1 = (unsigned char) YUV[i + 3];
		YUV2RGB (y0, u, v, &r, &g, &b);
		RGB[j + 0] = r;
		RGB[j + 1] = g;
		RGB[j + 2] = b;
		YUV2RGB (y1, u, v, &r, &g, &b);
		RGB[j + 3] = r;
		RGB[j + 4] = g;
		RGB[j + 5] = b;
	}
}

static void
yuyv2rgb(char *YUV, char *RGB, int NumPixels) {
	int i, j;
	unsigned char y0, y1, u, v;
	unsigned char r, g, b;

	for (i = 0, j = 0; i < (NumPixels << 1); i += 4, j += 6)
	{
		//printf( "%d\n", i );
		y0 = (unsigned char) YUV[i + 0];
		u = (unsigned char) YUV[i + 1];
		y1 = (unsigned char) YUV[i + 2];
		v = (unsigned char) YUV[i + 3];
		YUV2RGB (y0, u, v, &r, &g, &b);
		RGB[j + 0] = r;
		RGB[j + 1] = g;
		RGB[j + 2] = b;
		YUV2RGB (y1, u, v, &r, &g, &b);
		RGB[j + 3] = r;
		RGB[j + 4] = g;
		RGB[j + 5] = b;
	}
}

//static int init_mjpeg_decoder(int image_width, int image_height)
//{
//  avcodec_init();
//  avcodec_register_all();
//
//  avcodec = avcodec_find_decoder(CODEC_ID_MJPEG);
//  if (!avcodec)
//  {
//    fprintf(stderr,"Could not find MJPEG decoder\n");
//    return 0;
//  }
//
//  avcodec_context = avcodec_alloc_context();
//  avframe_camera = avcodec_alloc_frame();
//  avframe_rgb = avcodec_alloc_frame();
//
//  avpicture_alloc((AVPicture *)avframe_rgb, PIX_FMT_RGB24, image_width, image_height);
//
//  avcodec_context->codec_id = CODEC_ID_MJPEG;
//  avcodec_context->width = image_width;
//  avcodec_context->height = image_height;
//
//  avframe_camera_size = avpicture_get_size(PIX_FMT_YUV422P, image_width, image_height);
//  avframe_rgb_size = avpicture_get_size(PIX_FMT_RGB24, image_width, image_height);
//
//  /* open it */
//  if (avcodec_open(avcodec_context, avcodec) < 0)
//  {
//    fprintf(stderr,"Could not open MJPEG Decoder\n");
//    return 0;
//  }
//  return 1;
//}
//
//static void
//mjpeg2rgb(char *MJPEG, int len, char *RGB, int NumPixels)
//{
//  int got_picture;
//
//  memset(RGB, 0, avframe_rgb_size);
//
//  avcodec_decode_video(avcodec_context, avframe_camera, &got_picture, (uint8_t *) MJPEG, len);
//
//  if (!got_picture) {
//    fprintf(stderr,"Webcam: expected picture but didn't get it...\n");
//    return;
//  }
//
//  int xsize = avcodec_context->width;
//  int ysize = avcodec_context->height;
//  int pic_size = avpicture_get_size(avcodec_context->pix_fmt, xsize, ysize);
//  if (pic_size != avframe_camera_size) {
//    fprintf(stderr,"outbuf size mismatch.  pic_size: %d bufsize: %d\n",pic_size,avframe_camera_size);
//    return;
//  }
//
//  video_sws = sws_getContext( xsize, ysize, avcodec_context->pix_fmt, xsize, ysize, PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
//  sws_scale(video_sws, avframe_camera->data, avframe_camera->linesize, 0, ysize, avframe_rgb->data, avframe_rgb->linesize );
//  sws_freeContext(video_sws);  
//
//  int size = avpicture_layout((AVPicture *) avframe_rgb, PIX_FMT_RGB24, xsize, ysize, (uint8_t *)RGB, avframe_rgb_size);
//  if (size != avframe_rgb_size) {
//    fprintf(stderr,"webcam: avpicture_layout error: %d\n",size);
//    return;
//  }
//}

int is_huffman( const unsigned char *buf)
{
  const unsigned char *ptbuf;
  int i = 0;
  ptbuf = buf;
  while (((ptbuf[0] << 8) | ptbuf[1]) != 0xffda) {
    if (i++ > 2048)
      return 0;
    if (((ptbuf[0] << 8) | ptbuf[1]) == 0xffc4)
      return 1;
    ptbuf++;
  }
  return 0;
}

/******************************************************************************
Description.: 
Input Value.: 
Return Value: 
******************************************************************************/
int memcpy_picture(unsigned char *out, const unsigned char *buf, int size)
{
  const unsigned char *ptdeb, *ptlimit, *ptcur = buf;
  int sizein, pos=0;

  if (!is_huffman(buf)) {
    ptdeb = ptcur = buf;
    ptlimit = buf + size;
    while ((((ptcur[0] << 8) | ptcur[1]) != 0xffc0) && (ptcur < ptlimit))
      ptcur++;
    if (ptcur >= ptlimit)
        return pos;
    sizein = ptcur - ptdeb;

    memcpy(out+pos, buf, sizein); pos += sizein;
    memcpy(out+pos, dht_data, sizeof(dht_data)); pos += sizeof(dht_data);
    memcpy(out+pos, ptcur, size - sizein); pos += size-sizein;
  } else {
    memcpy(out+pos, ptcur, size); pos += size;
  }
  return pos;
}

static void process_image(const void * src, int len, usb_cam_camera_image_t *dest)
{
	//if(pixelformat==V4L2_PIX_FMT_YUYV)
	//	yuyv2rgb((char*)src, dest->image, dest->width*dest->height);
	//else if(pixelformat==V4L2_PIX_FMT_UYVY)
	//	uyvy2rgb((char*)src, dest->image, dest->width*dest->height);
	//else if(pixelformat==V4L2_PIX_FMT_MJPEG)
	//	//mjpeg2rgb((char*)src, len, dest->image, dest->width*dest->height);
	//{

	if(pixelformat==V4L2_PIX_FMT_MJPEG)
	{
		dest->image_size = memcpy_picture( (unsigned char*)dest->image, (const unsigned char*)src, len );
	}
	else
	{
		dest->image_size = len;
		memcpy( dest->image, src, len );
	}
}

static int read_frame(usb_cam_camera_image_t *image)
{
	struct v4l2_buffer buf;
	unsigned int i;
	int len;

	switch (io)
	{
	case IO_METHOD_READ:
		len = read(fd, buffers[0].start, buffers[0].length);
		if (len==-1)
		{
			switch (errno)
			{
			case EAGAIN:
				return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit("read");
				return 0;
			}
		}

		process_image(buffers[0].start, len, image);

		break;

	case IO_METHOD_MMAP:
		CLEAR (buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		if (-1==xioctl(fd, VIDIOC_DQBUF, &buf))
		{
			switch (errno)
			{
			case EAGAIN:
				return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit("VIDIOC_DQBUF");
				return 0;
			}
		}

		assert (buf.index < n_buffers);
		len = buf.bytesused;
		process_image(buffers[buf.index].start, len, image);

		if (-1==xioctl(fd, VIDIOC_QBUF, &buf))
		{
			errno_exit("VIDIOC_QBUF");
			return 0;
		}

		break;

	case IO_METHOD_USERPTR:
		CLEAR (buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;

		if (-1==xioctl(fd, VIDIOC_DQBUF, &buf))
		{
			switch (errno)
			{
			case EAGAIN:
				return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit("VIDIOC_DQBUF");
				return 0;
			}
		}

		for(i = 0; i<n_buffers; ++i)
		{
			if (buf.m.userptr==(unsigned long) buffers[i].start&&buf.length==buffers[i].length)
			{
				break;
			}
		}

		assert (i < n_buffers);
		len = buf.bytesused;
		process_image((void *) buf.m.userptr, len, image);

		if (-1==xioctl(fd, VIDIOC_QBUF, &buf))
		{
			errno_exit("VIDIOC_QBUF");
			return 0;
		}

		break;
	}

	return 1;
}

static void stop_capturing(void)
{
	enum v4l2_buf_type type;

	switch (io) {
case IO_METHOD_READ:
	/* Nothing to do. */
	break;

case IO_METHOD_MMAP:
case IO_METHOD_USERPTR:
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1==xioctl(fd, VIDIOC_STREAMOFF, &type))
		errno_exit("VIDIOC_STREAMOFF");

	break;
	}
}

static bool start_capturing(void)
{
	unsigned int i;
	enum v4l2_buf_type type;

	switch (io) {
case IO_METHOD_READ:
	/* Nothing to do. */
	break;

case IO_METHOD_MMAP:
	for(i = 0; i<n_buffers; ++i) {
		struct v4l2_buffer buf;

		CLEAR (buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1==xioctl(fd, VIDIOC_QBUF, &buf))
		{
			errno_exit("VIDIOC_QBUF");
			return false;
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1==xioctl(fd, VIDIOC_STREAMON, &type))
	{
		errno_exit("VIDIOC_STREAMON");
		return false;
	}

	break;

case IO_METHOD_USERPTR:
	for(i = 0; i<n_buffers; ++i) {
		struct v4l2_buffer buf;

		CLEAR (buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;
		buf.index = i;
		buf.m.userptr = (unsigned long) buffers[i].start;
		buf.length = buffers[i].length;

		if (-1==xioctl(fd, VIDIOC_QBUF, &buf))
		{
			errno_exit("VIDIOC_QBUF");
			return false;
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1==xioctl(fd, VIDIOC_STREAMON, &type))
	{
		errno_exit("VIDIOC_STREAMON");
		return false;
	}

	break;
	}

	return true;
}

static void uninit_device(void)
{
	unsigned int i;

	switch (io) {
case IO_METHOD_READ:
	free(buffers[0].start);
	break;

case IO_METHOD_MMAP:
	for(i = 0; i<n_buffers; ++i)
		if (-1==munmap(buffers[i].start, buffers[i].length))
			errno_exit("munmap");
	break;

case IO_METHOD_USERPTR:
	for(i = 0; i<n_buffers; ++i)
		free(buffers[i].start);
	break;
	}

	n_buffers = 0;
	free(buffers);
	buffers = NULL;
}

static void init_read(unsigned int buffer_size)
{
	buffers = (buffer*)calloc(1, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	buffers[0].length = buffer_size;
	buffers[0].start = malloc(buffer_size);

	if (!buffers[0].start) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}
}

static void init_mmap(void)
{
	struct v4l2_requestbuffers req;

	CLEAR (req);

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1==xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL==errno) {
			fprintf(stderr, "%s does not support memory mapping\n", camera_dev);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count<2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n", camera_dev);
		exit(EXIT_FAILURE);
	}

	buffers = (buffer*)calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for(n_buffers = 0; n_buffers<req.count; ++n_buffers) {
		struct v4l2_buffer buf;

		CLEAR (buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;

		if (-1==xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL /* start anywhere */, buf.length, PROT_READ|PROT_WRITE /* required */, MAP_SHARED /* recommended */, fd, buf.m.offset);

		if (MAP_FAILED==buffers[n_buffers].start)
			errno_exit("mmap");
	}
}

static void init_userp(unsigned int buffer_size)
{
	struct v4l2_requestbuffers req;
	unsigned int page_size;

	page_size = getpagesize();
	buffer_size = (buffer_size+page_size-1)&~(page_size-1);

	CLEAR (req);

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;

	if (-1==xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL==errno) {
			fprintf(stderr, "%s does not support "
				"user pointer i/o\n", camera_dev);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	buffers = (buffer*)calloc(4, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for(n_buffers = 0; n_buffers<4; ++n_buffers) {
		buffers[n_buffers].length = buffer_size;
		buffers[n_buffers].start = memalign(/* boundary */page_size, buffer_size);

		if (!buffers[n_buffers].start) {
			fprintf(stderr, "Out of memory\n");
			exit(EXIT_FAILURE);
		}
	}
}

static bool init_device(int image_width, int image_height)
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;

	if (-1==xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL==errno) {
			fprintf(stderr, "%s is no V4L2 device\n", camera_dev);
			return false;
		} else {
			errno_exit("VIDIOC_QUERYCAP");
			return false;
		}
	}

	if (!(cap.capabilities&V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is no video capture device\n", camera_dev);
		return false;
	}

	switch (io) {
case IO_METHOD_READ:
	if (!(cap.capabilities&V4L2_CAP_READWRITE)) {
		fprintf(stderr, "%s does not support read i/o\n", camera_dev);
		return false;
	}

	break;

case IO_METHOD_MMAP:
case IO_METHOD_USERPTR:
	if (!(cap.capabilities&V4L2_CAP_STREAMING)) {
		fprintf(stderr, "%s does not support streaming i/o\n", camera_dev);
		return false;
	}

	break;
	}

	/* Select video input, video standard and tune here. */

	CLEAR (cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (0==xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect; /* reset to default */

		if (-1==xioctl(fd, VIDIOC_S_CROP, &crop)) {
			switch (errno) {
case EINVAL:
	/* Cropping not supported. */
	break;
default:
	/* Errors ignored. */
	break;
			}
		}
	} else {
		/* Errors ignored. */
	}

	CLEAR (fmt);

	//  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	//  fmt.fmt.pix.width = 640;
	//  fmt.fmt.pix.height = 480;
	//  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	//  fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = image_width;
	fmt.fmt.pix.height = image_height;
	fmt.fmt.pix.pixelformat = pixelformat;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;


	if (-1==xioctl(fd, VIDIOC_S_FMT, &fmt))
		errno_exit("VIDIOC_S_FMT");

	/* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width*2;
	if (fmt.fmt.pix.bytesperline<min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline*fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage<min)
		fmt.fmt.pix.sizeimage = min;

	image_width = fmt.fmt.pix.width;
	image_height = fmt.fmt.pix.height;

	switch (io) {
case IO_METHOD_READ:
	init_read(fmt.fmt.pix.sizeimage);
	break;

case IO_METHOD_MMAP:
	init_mmap();
	break;

case IO_METHOD_USERPTR:
	init_userp(fmt.fmt.pix.sizeimage);
	break;
	}

	return true;
}

static void close_device(void)
{
	if (-1==close(fd))
		errno_exit("close");

	fd = -1;
}

static bool open_device(void)
{
	struct stat st;

	if (-1==stat(camera_dev, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n", camera_dev, errno, strerror(errno));
		return false;
	}

	if (!S_ISCHR (st.st_mode)) {
		fprintf(stderr, "%s is no device\n", camera_dev);
		return false;
	}

	fd = open(camera_dev, O_RDWR /* required */|O_NONBLOCK, 0);

	if (-1==fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n", camera_dev, errno, strerror(errno));
		return false;
	}

	return true;
}

usb_cam_camera_image_t *usb_cam_camera_start(const char* dev, usb_cam_io_method io_method,
											 usb_cam_pixel_format pixel_format, int image_width, int image_height)
{
	camera_dev = (char*)calloc(1,strlen(dev)+1);
	strcpy(camera_dev,dev);

	usb_cam_camera_image_t *image = NULL;
	io = io_method;
	pixelformat = 0;

	if( false == open_device() )
	{
		return NULL;
	}

	if( pixel_format == PIXEL_FORMAT_RAW )
	{
		v4l2_fmtdesc fmtd;
		fmtd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmtd.index = 0;
		while ( ioctl( fd, VIDIOC_ENUM_FMT, &fmtd ) != -1 )
		{
			if ( fmtd.pixelformat == V4L2_PIX_FMT_RGB24 )
			{
				printf( "rgb24 supported\n" );
				pixelformat = fmtd.pixelformat;
				break;
			}
			else if ( fmtd.pixelformat == V4L2_PIX_FMT_RGB32 )
			{
				printf( "rgb32 supported\n" );
				pixelformat = fmtd.pixelformat;
			}
			else if ( fmtd.pixelformat == V4L2_PIX_FMT_YUYV )
			{
				printf( "yuyv supported\n" );
				if ( pixelformat == 0 )
				{
					pixelformat = fmtd.pixelformat;
				}
			}
			fmtd.index++;
		}
		if ( 0 == pixelformat )
		{
			fprintf( stderr, "None of the formats provided by the device are supported.\n" );
			return NULL;
		}

	}
	else if(pixel_format == PIXEL_FORMAT_MJPEG)
	{
		pixelformat = V4L2_PIX_FMT_MJPEG;
		//init_mjpeg_decoder(image_width, image_height);
	}
	else {
		fprintf(stderr, "Unknown pixelformat.\n");
		return NULL;
	}

	if( false == init_device(image_width, image_height) )
	{
		return NULL;
	}

	if( false == start_capturing() )
	{
		return NULL;
	}

	image = (usb_cam_camera_image_t *) calloc(1, sizeof(usb_cam_camera_image_t));

	image->width = image_width;
	image->height = image_height;

	switch( pixelformat )
	{
	case V4L2_PIX_FMT_RGB24:
		image->bytes_per_pixel = 3;
		break;

	case V4L2_PIX_FMT_RGB32:
		image->bytes_per_pixel = 4;
		break;

	case V4L2_PIX_FMT_YUYV:
		image->bytes_per_pixel = 2;
		break;

	default:
		image->bytes_per_pixel = 3;
		break;
	}


	image->image_size = image->width*image->height*image->bytes_per_pixel;
	image->is_new = 0;
	image->image = (char *) calloc(image->image_size, sizeof(char));
	memset(image->image, 0, image->image_size*sizeof(char));

	return image;
}

void usb_cam_camera_shutdown(void)
{
	if( -1 == fd )
	{
		return;
	}

	stop_capturing();
	uninit_device();
	close_device();

	//if (avcodec_context) {
	//	avcodec_close(avcodec_context);
	//	av_free(avcodec_context);
	//	avcodec_context = NULL;
	//}
	//if (avframe_camera)
	//	av_free(avframe_camera);
	//avframe_camera = NULL;
	//if (avframe_rgb)
	//	av_free(avframe_rgb);
	//avframe_rgb = NULL;
}

bool usb_cam_camera_grab_image(usb_cam_camera_image_t *image)
{
	fd_set fds;
	struct timeval tv;
	int r;

	FD_ZERO (&fds);
	FD_SET (fd, &fds);

	/* Timeout. */
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	r = select(fd+1, &fds, NULL, NULL, &tv);

	if (-1==r) {
		if (EINTR==errno)
			return false;

		errno_exit("select");
		return false;
	}

	if (0==r) {
		fprintf(stderr, "select timeout\n");
		return false;
	}

	if( 0 == read_frame(image) )
	{
		return false;
	}

	image->is_new = 1;

	switch( pixelformat )
	{
	case V4L2_PIX_FMT_MJPEG:
		strcpy( image->pixel_format, "mjpg" );
		break;

	case V4L2_PIX_FMT_RGB24:
		strcpy( image->pixel_format, "rgb24" );
		break;

	case V4L2_PIX_FMT_RGB32:
		strcpy( image->pixel_format, "rgb32" );
		break;

	case V4L2_PIX_FMT_YUYV:
		strcpy( image->pixel_format, "yuyv" );
		break;

	default:
		strcpy( image->pixel_format, "na" );
		break;
	}

	return true;
}

bool usb_cam_camera_set_control(unsigned int id, int value)
{
	struct v4l2_control control;

	for (int i=0; i<20; i++)
	{
		// try to set value
		CLEAR(control);
		control.id = id;
		control.value = value;
	    if (-1 == xioctl (fd, VIDIOC_S_CTRL, &control))
	    {
	    	fprintf( stderr, "Error setting control value!");
	    	usleep( 5000 );
	    	continue;
	    }

	    // test if value is set
	    CLEAR(control);
	    control.id = id;
	    if (-1 == xioctl (fd, VIDIOC_G_CTRL, &control))
	    {
	    	fprintf( stderr, "Error testing contol value!");
	    	usleep( 5000 );
	    	continue;
	    }

	    if(control.value == value)
	    	return true;
	    else
	    {
	    	fprintf( stderr, "Control value not set yet, trying again in 5ms!");
	    	usleep( 5000 );
	    }
	}

	fprintf( stderr, "Could not set control value for 20 times, giving up!");
    return false;
}

bool usb_cam_camera_set_brightness(int value)
{
	struct v4l2_control control;
	CLEAR(control);

	control.id = V4L2_CID_BRIGHTNESS;
	control.value = value;

    if (-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
    {
    	fprintf( stderr, "Could not set brightness!");
    	return false;
    }
    return true;
}

bool usb_cam_camera_set_contrast(int value)
{
	return usb_cam_camera_set_control(V4L2_CID_CONTRAST, value);
}

bool usb_cam_camera_set_saturation(int value)
{
	return usb_cam_camera_set_control(V4L2_CID_SATURATION, value);
}

bool usb_camera_set_auto_white_balance(bool value)
{
	return usb_cam_camera_set_control(V4L2_CID_AUTO_WHITE_BALANCE, (int)value);
}

bool usb_camera_set_gain(int value)
{
	return usb_cam_camera_set_control(V4L2_CID_GAIN, value);
}

bool usb_camera_set_white_balance_temperature(int value)
{
	return usb_cam_camera_set_control(V4L2_CID_WHITE_BALANCE_TEMPERATURE, value);
}

bool usb_camera_set_backlight_compensation(int value)
{
	return usb_cam_camera_set_control(V4L2_CID_BACKLIGHT_COMPENSATION, value);
}

bool usb_camera_set_manual_exposure_mode()
{
	v4l2_ext_control controlList[2];

	//configure exposure priority
	controlList[0].id = V4L2_CID_EXPOSURE_AUTO_PRIORITY;

	//configure exposure mode
	controlList[1].id = V4L2_CID_EXPOSURE_AUTO;


	struct v4l2_ext_controls controls;
	controls.ctrl_class = V4L2_CID_CAMERA_CLASS;
	controls.count = 2;
	controls.controls = controlList;

	for(int i=0; i<20; i++)
	{
		controlList[0].value = 0; // disabled
		controlList[1].value = 1; // manual mode

	    if (-1 == xioctl (fd, VIDIOC_S_EXT_CTRLS, &controls))
	    {
	    	fprintf( stderr, "[V4LCamera] Error setting exposure mode!");
	    	usleep( 5000 );
	    	continue;
	    }

	    //test if value is set correctly
	    if (-1 == xioctl (fd, VIDIOC_G_EXT_CTRLS, &controls))
	    {
	    	fprintf( stderr, "[V4LCamera] Error testing exposure mode!");
	    	usleep( 5000 );
	    	continue;
	    }

	    if(controls.controls[0].value == 0 && controls.controls[1].value == 1)
	    	return true;
	    else
	    {
	    	fprintf( stderr, "[V4LCamera] Extended control value not set yet, trying again in 5ms!");
	    	usleep( 5000 );
	    }
	}

	fprintf( stderr, "[V4LCamera] Could not set extended control value for 20 times, giving up!");
    return false;
}

bool usb_camera_set_auto_exposure_mode()
{
	fprintf( stderr, "[V4LCamera] Setting exposure mode!");
	v4l2_ext_control controlList[2];

	//configure exposure priority
	controlList[0].id = V4L2_CID_EXPOSURE_AUTO_PRIORITY;

	//configure exposure mode
	controlList[1].id = V4L2_CID_EXPOSURE_AUTO;


	struct v4l2_ext_controls controls;
	controls.ctrl_class = V4L2_CID_CAMERA_CLASS;
	controls.count = 2;
	controls.controls = controlList;

	for(int i=0; i<20; i++)
	{
		controlList[0].value = 0; // disabled
		controlList[1].value = 0; // auto mode

	    if (-1 == xioctl (fd, VIDIOC_S_EXT_CTRLS, &controls))
	    {
	    	fprintf( stderr, "[V4LCamera] Error setting exposure mode!");
	    	usleep( 5000 );
	    	continue;
	    }

	    //test if value is set correctly
	    if (-1 == xioctl (fd, VIDIOC_G_EXT_CTRLS, &controls))
	    {
	    	fprintf( stderr, "[V4LCamera] Error testing exposure mode!");
	    	usleep( 5000 );
	    	continue;
	    }

	    if(controls.controls[0].value == 0 && controls.controls[1].value == 0)
	    	return true;
	    else
	    {
	    	fprintf( stderr, "[V4LCamera] Extended control value not set yet, trying again in 5ms!");
	    	usleep( 5000 );
	    }
	}

	fprintf( stderr, "[V4LCamera] Could not set extended control value for 20 times, giving up!");
    return false;
}

bool usb_camera_set_exposure(int value)
{
	fprintf( stderr, "[V4LCamera] Setting exposure!");
	v4l2_ext_control controlList[1];

	//configure exposure priority
	controlList[0].id = V4L2_CID_EXPOSURE_ABSOLUTE;

	struct v4l2_ext_controls controls;
	controls.ctrl_class = V4L2_CID_CAMERA_CLASS;
	controls.count = 1;
	controls.controls = controlList;

	for(int i=0; i<20; i++)
	{
		controlList[0].value = value;

	    if (-1 == xioctl (fd, VIDIOC_S_EXT_CTRLS, &controls))
	    {
	    	fprintf( stderr, "[V4LCamera] Error setting exposure mode!");
	    	usleep( 5000 );
	    	continue;
	    }

	    //test if value is set correctly
	    if (-1 == xioctl (fd, VIDIOC_G_EXT_CTRLS, &controls))
	    {
	    	fprintf( stderr, "[V4LCamera] Error testing exposure mode!");
	    	usleep( 5000 );
	    	continue;
	    }

	    if(controls.controls[0].value == value)
	    	return true;
	    else
	    {
	    	fprintf( stderr, "[V4LCamera] Extended control value not set yet, trying again in 5ms!");
	    	usleep( 5000 );
	    }
	}

	fprintf( stderr, "[V4LCamera] Could not set extended control value for 20 times, giving up!");
    return false;
}

bool usb_camera_set_auto_focus_enabled(bool value)
{
	v4l2_ext_control controlList[1];

	//configure auto focus
	controlList[0].id = V4L2_CID_FOCUS_AUTO;
	controlList[0].value = (int) value; // disabled

	struct v4l2_ext_controls controls;
	controls.ctrl_class = V4L2_CID_CAMERA_CLASS;
	controls.count = 1;
	controls.controls = controlList;

    if (-1 == ioctl (fd, VIDIOC_S_EXT_CTRLS, &controls))
    {
    	fprintf( stderr, "[V4LCamera] Could not set autofocus mode!");
    	return false;
    }
    return true;
}

bool usb_camera_set_focus(int value)
{
	v4l2_ext_control controlList[2];

	//configure focus point
	controlList[0].id = V4L2_CID_FOCUS_ABSOLUTE;
	controlList[0].value = value; // focus point

	struct v4l2_ext_controls controls;
	controls.ctrl_class = V4L2_CID_CAMERA_CLASS;
	controls.count = 1;
	controls.controls = controlList;

    if (-1 == ioctl (fd, VIDIOC_S_EXT_CTRLS, &controls))
    {
    	fprintf( stderr, "[V4LCamera] Could not set focus!");
    	return false;
    }
    return true;
}

bool usb_camera_set_sharpness(int value)
{
	struct v4l2_control control;
	CLEAR(control);

	control.id = V4L2_CID_SHARPNESS;
	control.value = value;

    if (-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
    {
    	fprintf( stderr, "[V4LCamera] Could not set sharpness!");
    	return false;
    }
    return true;
}