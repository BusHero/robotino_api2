#ifndef _JPEG_MEM_H_
#define _JPEG_MEM_H_

#include <stdio.h>
#include <setjmp.h>

extern "C" {

#if defined WIN32 || defined __APPLE__
#include "rec/cv_lt/jpeg/jpeglib.h"
#else
#include <jpeglib.h>
#endif

struct mem_error_mgr : public jpeg_error_mgr 
{
    jmp_buf setjmp_buffer;
};

static void mem_error_exit( j_common_ptr cinfo )
{
  mem_error_mgr* err = (mem_error_mgr*) cinfo->err;
  longjmp( err->setjmp_buffer, 1 );
}

}

#endif
