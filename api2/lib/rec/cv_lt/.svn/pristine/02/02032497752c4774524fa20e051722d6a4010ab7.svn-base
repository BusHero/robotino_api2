//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#include "rec/cv_lt/jpeg.h"
#include "rec/cv_lt/jpeg_mem.hpp"

extern "C" {

	struct mem_jpeg_dest_mgr : public jpeg_destination_mgr
	{
		unsigned int bufferSize;
		unsigned int* jpegDataSize;

		mem_jpeg_dest_mgr( char* buffer, unsigned int bufferSize, unsigned int* jpegDataSize );
	};

	static void jpeg_init_dest( j_compress_ptr cinfo );
	static boolean jpeg_empty_output_buffer( j_compress_ptr cinfo );
	static void jpeg_term_dest(j_compress_ptr cinfo);


	static void jpeg_init_dest( j_compress_ptr cinfo )
	{
	}

	static boolean jpeg_empty_output_buffer( j_compress_ptr cinfo )
	{
		mem_jpeg_dest_mgr* dest = (mem_jpeg_dest_mgr*) cinfo->dest;
		// should never be called, buffer should be big enough
		(*cinfo->err->error_exit)( (j_common_ptr) cinfo );
		return true;
	}

	static void jpeg_term_dest( j_compress_ptr cinfo )
	{
		mem_jpeg_dest_mgr* dest = (mem_jpeg_dest_mgr*)cinfo->dest;

		*(dest->jpegDataSize) = dest->bufferSize - dest->free_in_buffer;
	}

}


mem_jpeg_dest_mgr::mem_jpeg_dest_mgr( char* buffer, unsigned int bufferSize, unsigned int* jpegDataSize )
: bufferSize( bufferSize )
,jpegDataSize( jpegDataSize )
{
	init_destination = jpeg_init_dest;
	empty_output_buffer = jpeg_empty_output_buffer;
	term_destination = jpeg_term_dest;
	next_output_byte = (JOCTET*) buffer;
	free_in_buffer = bufferSize;
}

//@param src Pointer to an RGB image or grayscale image
//@return Returns the compressed image size in bytes
unsigned int rec::cv_lt::jpg_compress( const char* srcData,
									  const unsigned int width,
									  const unsigned int height,
									  const unsigned int step,
									  const unsigned int numChannels,
									  char* dst,
									  const unsigned int dstSize,
									  int quality
									  )
{
	struct jpeg_compress_struct cinfo;
	struct mem_error_mgr jerr;

	cinfo.err = jpeg_std_error( &jerr );
	jerr.error_exit = mem_error_exit;

	if( !setjmp(jerr.setjmp_buffer) )
	{
		unsigned int compressedSize = 0;
		jpeg_create_compress(&cinfo);
		mem_jpeg_dest_mgr mjdm( dst, dstSize, &compressedSize );

		cinfo.dest = &mjdm;
		cinfo.image_width = width;
		cinfo.image_height = height;
		cinfo.input_components = numChannels;
		if( numChannels == 1 )
		{
			cinfo.in_color_space = JCS_GRAYSCALE;
		}
		else
		{
			cinfo.in_color_space = JCS_RGB;
		}
		jpeg_set_defaults( &cinfo );

		jpeg_set_quality( &cinfo, quality, true );

		jpeg_start_compress( &cinfo, true );

		JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
		const unsigned char* imgLine = (const unsigned char*)srcData;
		while( cinfo.next_scanline < cinfo.image_height )
		{
			row_pointer[0] = const_cast<unsigned char*>( imgLine );
			jpeg_write_scanlines( &cinfo, row_pointer, 1 );
			imgLine += step;

			if( dstSize - compressedSize < 10000 )
			{
				return 0;
			}
		}

		jpeg_finish_compress( &cinfo );
		jpeg_destroy_compress( &cinfo );

		return compressedSize;
	}
	else
	{
		jpeg_destroy_compress( &cinfo );
		return 0;
	}
}


// compression

