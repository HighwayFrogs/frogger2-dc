//! Defines a custom data source for LIBJPEG

extern "C" {

GLOBAL(void)
jpeg_mem_dest (j_compress_ptr cinfo,
              JOCTET * pData,
              int FileSize,
              CDataSink * pDataSrc
);

}