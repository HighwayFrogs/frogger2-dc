//! Defines a custom data source for LIBJPEG
GLOBAL(void)
jpeg_mem_src (j_decompress_ptr cinfo,
              JOCTET * pData,
              int FileSize,
              void *pDataSrc,
              JMETHOD(void, notifyCppWorld, (j_common_ptr)));
              // Jo Hagelberg 15.4.99: added for notification callback

/*
/--------------------------------------------------------------------
|
|      $log$
|
--------------------------------------------------------------------
*/
