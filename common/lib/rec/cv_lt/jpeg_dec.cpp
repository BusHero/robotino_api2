//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#include "rec/cv_lt/jpeg.h"
#include "rec/cv_lt/jpeg_mem.hpp"

#include <cstring>

extern "C" {

	/* JPEG DHT Segment for YCrCb omitted from MJPEG data */
	static unsigned char jpeg_odml_dht[0x1a4] = {
		0xff, 0xc4, 0x01, 0xa2,

		0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,

		0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,

		0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d,
		0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
		0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
		0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
		0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
		0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
		0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
		0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
		0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
		0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
		0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
		0xf9, 0xfa,

		0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77,
		0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
		0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
		0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
		0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
		0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
		0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
		0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
		0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
		0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
		0xf9, 0xfa
	};

	struct mem_jpeg_source_mgr : public jpeg_source_mgr
	{
		bool buffer_filled;
		char dummy_buffer[ 2 ];

		mem_jpeg_source_mgr( char* data, unsigned int length );
	};

	static void jpeg_init_source( j_decompress_ptr cinfo );
	static boolean jpeg_fill_input_buffer( j_decompress_ptr cinfo );
	static void jpeg_skip_input_data( j_decompress_ptr cinfo, long num_bytes );
	static void jpeg_term_source (j_decompress_ptr cinfo);

	static void jpeg_init_source( j_decompress_ptr cinfo )
	{
	}

	static boolean jpeg_fill_input_buffer( j_decompress_ptr cinfo )
	{
		mem_jpeg_source_mgr* src = (mem_jpeg_source_mgr*)cinfo->src;
		if( src->buffer_filled ) 
		{
			// Insert a fake EOI marker - as per jpeglib recommendation
			src->next_input_byte = (const JOCTET*) src->dummy_buffer;
			src->bytes_in_buffer = 2;
		} 
		else 
		{
			src->buffer_filled = true;
		}
		return true;
	}

	static void jpeg_skip_input_data( j_decompress_ptr cinfo, long num_bytes )
	{
		mem_jpeg_source_mgr* src = (mem_jpeg_source_mgr*)cinfo->src;

		if (num_bytes > 0) 
		{
			while( num_bytes > (long) src->bytes_in_buffer ) 
			{
				num_bytes -= (long) src->bytes_in_buffer;
				jpeg_fill_input_buffer(cinfo);
			}
			src->next_input_byte += (size_t) num_bytes;
			src->bytes_in_buffer -= (size_t) num_bytes;
		}
	}

	static void jpeg_term_source (j_decompress_ptr cinfo)
	{
		/* no work necessary here */
	}

	/*
	* Parse the DHT table.
	* This code comes from jpeg6b (jdmarker.c).
	*/
	static int jpeg_load_dht (struct jpeg_decompress_struct *info, unsigned char *dht,
		JHUFF_TBL *ac_tables[], JHUFF_TBL *dc_tables[])
	{
		unsigned int length = (dht[2] << 8) + dht[3] - 2;
		unsigned int pos = 4;
		unsigned int count, i;
		int index;

		JHUFF_TBL **hufftbl;
		unsigned char bits[17];
		unsigned char huffval[256];

		while (length > 16)
		{
			bits[0] = 0;
			index = dht[pos++];
			count = 0;
			for (i = 1; i <= 16; ++i)
			{
				bits[i] = dht[pos++];
				count += bits[i];
			}
			length -= 17;

			if (count > 256 || count > length)
				return -1;

			for (i = 0; i < count; ++i)
				huffval[i] = dht[pos++];
			length -= count;

			if (index & 0x10)
			{
				index -= 0x10;
				hufftbl = &ac_tables[index];
			}
			else
				hufftbl = &dc_tables[index];

			if (index < 0 || index >= NUM_HUFF_TBLS)
				return -1;

			if (*hufftbl == NULL)
				*hufftbl = jpeg_alloc_huff_table ((j_common_ptr)info);
			if (*hufftbl == NULL)
				return -1;

			memcpy ((*hufftbl)->bits, bits, sizeof (*hufftbl)->bits);
			memcpy ((*hufftbl)->huffval, huffval, sizeof (*hufftbl)->huffval);
		}

		if (length != 0)
			return -1;

		return 0;
	}

}


mem_jpeg_source_mgr::mem_jpeg_source_mgr( char* data, unsigned int length )
: buffer_filled( false )
{
	dummy_buffer[ 0 ] = (JOCTET) 0xFF;
	dummy_buffer[ 1 ] = (JOCTET) JPEG_EOI;
	next_input_byte = (const JOCTET*) data;
	bytes_in_buffer = length;
	init_source = jpeg_init_source;
	fill_input_buffer = jpeg_fill_input_buffer;
	skip_input_data = jpeg_skip_input_data;
	resync_to_restart = jpeg_resync_to_restart;
	term_source = jpeg_term_source;
}

bool rec::cv_lt::jpg_info( const char* srcData,
						  const unsigned int srcSize,
						  unsigned int* width,
						  unsigned int* height,
						  unsigned int* numChannels )
{
	struct jpeg_decompress_struct cinfo;
	struct mem_error_mgr jerr;

	jpeg_create_decompress(&cinfo);
	mem_jpeg_source_mgr mjsm( const_cast<char*>( srcData ), srcSize );
	cinfo.src = &mjsm;

	cinfo.err = jpeg_std_error( &jerr );
	jerr.error_exit = mem_error_exit;

	if( !setjmp(jerr.setjmp_buffer) )
	{
		jpeg_read_header( &cinfo, TRUE );
		*width = cinfo.image_width;
		*height = cinfo.image_height;
		*numChannels = cinfo.num_components;
	}
	else
	{
		*width = 0;
		*height = 0;
		*numChannels = 0;
	}

	jpeg_destroy_decompress(&cinfo);

	if( *width > 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool rec::cv_lt::jpg_decompress( const char* srcData,
								const unsigned int srcSize,
								char* dst,
								const unsigned int dstSize,
								unsigned int* width,
								unsigned int* height,
								unsigned int* numChannels )
{
	struct jpeg_decompress_struct cinfo;
	struct mem_error_mgr jerr;

	jpeg_create_decompress(&cinfo);
	mem_jpeg_source_mgr mjsm( const_cast<char*>( srcData ), srcSize );
	cinfo.src = &mjsm;

	cinfo.err = jpeg_std_error( &jerr );
	jerr.error_exit = mem_error_exit;

	if( !setjmp(jerr.setjmp_buffer) )
	{
		jpeg_read_header( &cinfo, TRUE );

		if ( cinfo.ac_huff_tbl_ptrs[0] == NULL &&
			cinfo.ac_huff_tbl_ptrs[1] == NULL &&
			cinfo.dc_huff_tbl_ptrs[0] == NULL &&
			cinfo.dc_huff_tbl_ptrs[1] == NULL )
		{
			jpeg_load_dht( &cinfo, jpeg_odml_dht, cinfo.ac_huff_tbl_ptrs, cinfo.dc_huff_tbl_ptrs );
		}


		if( 3 == cinfo.num_components )
		{
			*width = cinfo.image_width;
			*height = cinfo.image_height;
			*numChannels = cinfo.num_components;

			if( dstSize < *width * *height * *numChannels )
			{
				return false;
			}

			jpeg_start_decompress( &cinfo );

			char* p = dst;

			while (cinfo.output_scanline < cinfo.output_height) 
			{
				int rows = jpeg_read_scanlines(&cinfo, (JSAMPARRAY) &p, 1);
				if( 0 == rows )
				{
					jpeg_destroy_decompress(&cinfo);
					return false;
				}
				p += *width * *numChannels;
			}

			//jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			return true;
		}
	}

	jpeg_destroy_decompress(&cinfo);
	return false;
}


// compression

