/*
 * Copyright (C) 2018 Universitat Autonoma de Barcelona 
 * David Castells-Rufas <david.castells@uab.cat>
 * 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   SeqFileReader.cpp
 * Author: dcr
 * 
 * Created on February 1, 2018, 3:37 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "SeqFileReader.h"

// use libjpeg
#include <png.h>
#include <jpeglib.h>


SeqFileReader::SeqFileReader(const char* file)
{
    m_path = file;
    m_fp = NULL;
    m_verbose = 0;
}



SeqFileReader::~SeqFileReader()
{
    if (isOpen())
        close();
}

void SeqFileReader::open()
{
    m_fp = fopen(m_path.c_str(), "rb");
}

void SeqFileReader::close()
{
    fclose(m_fp);
}

bool SeqFileReader::isOpen()
{
    return m_fp != NULL;
}

unsigned short SeqFileReader::readUSHORT()
{
    uint16_t v;
    fread(&v, 2 , 1, m_fp);
    return v;
}

void SeqFileReader::skipBytes(int bytes)
{
    long int offset = ftell(m_fp);
    offset += bytes;
    fseek(m_fp, offset, SEEK_SET);
}

int SeqFileReader::readDWORD()
{
    int32_t v;
    fread(&v, 4 , 1, m_fp);
    return v;
}

int SeqFileReader::readLONG()
{
    int32_t v;
    fread(&v, 4 , 1, m_fp);
    return v;
}

double SeqFileReader::readDOUBLE()
{
    double v;
    fread(&v, 8, 1, m_fp);
    return v;
}

void SeqFileReader::skipImageData(SeqFileHeader* header)
{
    int compression = header->imageFormat;
    
    if (compression == 102 || compression == 201)
    {
        scanNextJpegStartMarker();
        skipBytes(header->imageSizeBytes/25);
    }
    else if (compression == 001 || compression == 002)
    {
        scanNextPngStartMarker();
        skipBytes(header->imageSizeBytes/20);
    }
    else
    {
        printf("[ERROR] compression format (%d) not supported\n", compression);
        exit(-1);
    }
}

void SeqFileReader::readImageData(SeqFileHeader* header, Image* image)
{
    int compression = header->imageFormat;
    
    if (compression == 102 || compression == 201)
        readJpegImageData(header, image);
    else if (compression == 001 || compression == 002)
        readPngImageData(header, image);
    else
        readRawImageData(image);
}

void SeqFileReader::scanNextPngStartMarker()
{
    unsigned long offset = ftell(m_fp);

    bool found = false;
    char header[8];    // 8 is the maximum size that can be checked
    
    do
    {
        fread(header, 1, 8, m_fp);
    
        if (png_sig_cmp((png_byte*)header, 0, 8))
        {
            found = false;
            fseek(m_fp, offset++, SEEK_SET);
        }
        else
            found = true;
        
    } while (!found);
    
    // rollback to previous position            
    // fseek(m_fp, offset, SEEK_SET);
}

void SeqFileReader::scanNextJpegStartMarker()
{
    unsigned long offset = ftell(m_fp);

    unsigned int marker;
    
    do
    {
        marker = readDWORD();
        
        if (marker == 0xE0FFD8FF)
        {
//            printf("Offset: 0x%08X JPEG Marker 0x%08X\n", offset, marker );
        }
        else
        {
            //printf("Offset: 0x%08X = 0x%08X\n", offset, marker );
            fseek(m_fp, offset++, SEEK_SET);
        }
    } while (!(marker == 0xE0FFD8FF));
    
    // rollback to previous position            
    fseek(m_fp, offset-1, SEEK_SET);
}

void SeqFileReader::readPngImageData(SeqFileHeader* header, Image* image)
{
    scanNextPngStartMarker();
    unsigned long offset = ftell(m_fp);
    
    // initialize stuff 
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
    {
        printf("[ERROR] png_create_read_struct failed");
        exit(-1);
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        printf("[ERROR] png_create_info_struct failed");
        exit(-1);
    }
    
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        printf("[ERROR] Error during init_io");
        exit(-1);
    }

    png_init_io(png_ptr, m_fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    
    //assert(width == image->m_width);
    //assert(height == image->m_height);
    
    int color_type = png_get_color_type(png_ptr, info_ptr);
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    int number_of_passes = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);


    // read file 
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        printf("[ERROR] Error during read_image");
        exit(-1);
    }
    

    png_bytep row_pointers[height]; //(png_bytep*) malloc(sizeof(png_bytep) * height);
    for (int y=0; y<height; y++)
        row_pointers[y] = &image->m_buffer[y*image->m_stride*3]; //(png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

    png_read_image(png_ptr, row_pointers);

}

/**
 * Reads a JPEG Image from file
 * @param header
 * @param image
 */
void SeqFileReader::readJpegImageData(SeqFileHeader* header, Image* image)
{
    scanNextJpegStartMarker();
    unsigned long offset = ftell(m_fp);
//    
//    printf("JPEG Offset: 0x%08lX\n", offset);
    
    struct jpeg_decompress_struct info; //for our jpeg info
    struct jpeg_error_mgr err;          //the error handler
    
    info.err = jpeg_std_error(& err);     
    jpeg_create_decompress(& info);   //fills info structure
    
    jpeg_stdio_src(&info, m_fp);    
    jpeg_read_header(&info, TRUE);   // read jpeg file header

    jpeg_start_decompress(&info);    // decompress the file

    //set width and height
    assert(header->width == info.output_width);
    assert(header->height == info.output_height);
    
//    channels = info.num_components;
//    type = GL_RGB;
//    if(channels == 4) type = GL_RGBA;
//
//    data_size = x * y * 3;
    unsigned char * rowptr[1]; 
    while (info.output_scanline < info.output_height) // loop
  {
    // Enable jpeg_read_scanlines() to fill our jdata array
    rowptr[0] = (unsigned char *)image->m_buffer +  // secret to method
            3* info.output_width * info.output_scanline; 

    jpeg_read_scanlines(&info, rowptr, 1);

  }
  //---------------------------------------------------

  jpeg_finish_decompress(&info);   //finish decompressing
  
  fseek(m_fp, offset + 10, SEEK_SET );
}

void SeqFileReader::readRawImageData(Image* image)
{
    int ret = fread(image->m_buffer, sizeof(unsigned char), image->getBytes(), m_fp);
    
    if (ret != image->getBytes())
    {
        printf("[ERROR] not reading the same %d != %d", ret, image->getBytes());
        exit(-1);
    }
    
    
}

std::string SeqFileReader::readWideCharString(int len)
{
    wchar_t s[len];
    
    fread(s, 2 , len, m_fp);
    
    std::wstring ret = L"";
    ret.append(s);
    
    std::string ret2(ret.begin(), ret.end() );
    
    //ret2 = ret;
    
    return ret2;
}

void SeqFileReader::readHeader(SeqFileHeader* header)
{
    if (!isOpen())
        open();
    
    header->magic = readDWORD();        // ofset 0
    header->name = readWideCharString(24/2);      // offset 4
    header->version = readLONG();       // offset 24 + 4  = 28
    header->headerSize = readLONG();    // offset 28 + 4 = 32
    header->description = readWideCharString(512/2);
    header->width = readDWORD();
    header->height = readDWORD();
    header->bitDepth = readDWORD();
    header->bitDepthReal = readDWORD();
    header->imageSizeBytes = readDWORD();
    header->imageFormat = readDWORD();
    header->allocatedFrames = readDWORD();
    header->origin = readDWORD();
    header->trueImageSize = readDWORD();
    header->suggestedFrameRate = readDOUBLE();
    header->descriptionFormat = readLONG();
    header->referenceFrame = readDWORD();
    header->fixedSize = readDWORD();
    header->flags = readDWORD();
    header->bayerPattern = readLONG();
    header->timeOffsetUS = readLONG();
    header->extendedHeaderSize = readLONG();
    header->compressionFormat = readDWORD();    
    header->referenceTimeS = readLONG();
    header->referenceTimeMS = readUSHORT();
    header->referenceTimeUS = readUSHORT();
    
    if (m_verbose)
        printf("%s", header->toString().c_str());
 
    if (header->compressionFormat != 0)
    {
        fprintf(stderr, "[ERROR] SeqReader Header Compression not supported!");
        exit(-1);
    }

    
    int afterHeader = ftell(m_fp);
    
    if (m_verbose)
        printf("Header Size: 0x%08X\n", afterHeader);
    //exit(0);
    
    fseek(m_fp, header->headerSize, SEEK_SET);
    afterHeader = ftell(m_fp);
    
    if (m_verbose)
        printf("Image Offset: 0x%08X\n", afterHeader);
}