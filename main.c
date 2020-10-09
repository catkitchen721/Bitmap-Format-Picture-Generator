#include <stdio.h>
#include <stdlib.h>
/*
    BMP picture file generator in C.
    Support creating 24-bits(RGB) and 32-bits(with alpha channel for transparent pictures) bmp files.
    Only depends on <stdio.h> and <stdlib.h>.
*/

/* DEBUG or RELEASE */
#define RELEASE

#ifdef DEBUG
    #define d_printf(args...) printf(args)
#else
    #define d_printf(args...)
#endif /* DEBUG */

typedef struct fileHeader  /* 14 bytes, but it will be 16 bytes after alignment. */
{
    unsigned : 16;
    unsigned type: 16;
    unsigned size: 32;
    unsigned reserved1: 16;
    unsigned reserved2: 16;
    unsigned off_bits: 32;
}FileHeader;

typedef struct infoHeader  /* 40 bytes. */
{
    unsigned size: 32;
    unsigned width: 32;
    unsigned height: 32;
    unsigned planes: 16;
    unsigned bit_count: 16;
    unsigned compression: 32;
    unsigned size_image: 32;
    unsigned x_pels_per_meter: 32;
    unsigned y_pels_per_meter: 32;
    unsigned clr_used: 32;
    unsigned clr_important: 32;
}InfoHeader;

typedef struct v4HeaderTail  /* 68 bytes, follow the infoHeader when bit_count is 32 */
{
    unsigned red_mask: 32;
    unsigned green_mask: 32;
    unsigned blue_mask: 32;
    unsigned alpha_mask: 32;
    unsigned cs_type: 32;
    unsigned red_x: 32;
    unsigned red_y: 32;
    unsigned red_z: 32;
    unsigned green_x: 32;
    unsigned green_y: 32;
    unsigned green_z: 32;
    unsigned blue_x: 32;
    unsigned blue_y: 32;
    unsigned blue_z: 32;
    unsigned gamma_red: 32;
    unsigned gamma_green: 32;
    unsigned gamma_blue: 32;
}V4HeaderTail;

unsigned char b(long i, long j)  /* pixel function of color blue */
{
    return 255;
}

unsigned char g(long i, long j)  /* pixel function of color green */
{
    return 0;
}

unsigned char r(long i, long j)  /* pixel function of color red */
{
    return 0;
}

unsigned char a(long i, long j)  /* pixel function of alpha */
{
    return ((double)(i + j)) / ((double)(2046)) * 255 ;
}

int main()
{
    FILE *fp = NULL;
    char *fname = "out.bmp";
    FileHeader fh = {};
    InfoHeader ih = {};
    V4HeaderTail v4ht = {};
    short bit_count = 32;  /* It can be 24 or 32. */
    long height = 1024;
    long width = 1024;
    long pixel_size = height * width * (((long)bit_count) / 8L);
    long i = 0, j = 0, p = 0;
    unsigned char *bmp = (unsigned char *)malloc(sizeof(unsigned char) * pixel_size);

    d_printf("%I64u %I64u\n", sizeof(fh), sizeof(ih));
    d_printf("%hd, %ld, %ld, %ld\n", bit_count, height, width, pixel_size);
    fh.type = (unsigned)0x4d42;  /* "BM" */
    fh.off_bits = (bit_count == 24)?(unsigned)54:(unsigned)122;
    fh.size = (unsigned)(fh.off_bits + pixel_size);
    d_printf("%s\n", (unsigned char *)(&fh) + 2);
    d_printf("%x %x %x %x\n", fh.size, fh.reserved1, fh.reserved2, fh.off_bits);
    /* File Header Completed. */

    ih.size = (bit_count == 24)?(unsigned)40:(unsigned)108;
    ih.width = (unsigned)width;
    ih.height = (unsigned)height;
    ih.compression = (bit_count == 24)?(unsigned)0:(unsigned)3;  /* 0 for BI_RGB, 3 for BI_BITFIELDS */
    ih.planes = (unsigned)1;
    ih.bit_count = (unsigned)bit_count;
    ih.size_image = (unsigned)pixel_size;
    d_printf("%x %x %x %x %x %x\n",
             ih.size, ih.width, ih.height, ih.planes, ih.bit_count, ih.size_image);
    /* Info Header Completed. */

    /* Here is only for bit_count == 32 */
    v4ht.cs_type = (unsigned)0x57696e20;  /* "Win " */
    v4ht.red_mask = (unsigned)0x00ff0000;
    v4ht.green_mask = (unsigned)0x0000ff00;
    v4ht.blue_mask = (unsigned)0x000000ff;
    v4ht.alpha_mask = (unsigned)0xff000000;
    /* V4 Header Tail Completed. */

    if(!bmp) return -1;
    for(i = 0; i < width; i++)
    {
        for(j = 0; j < height; j++)
        {
            p = (j * width + i) * (((long)bit_count) / 8L);
            if(bit_count == 24)
            {
                bmp[p] = b(i, j);
                bmp[p + 1] = g(i, j);
                bmp[p + 2] = r(i, j);
            }
            else
            {
                bmp[p] = b(i, j);
                bmp[p + 1] = g(i, j);
                bmp[p + 2] = r(i, j);
                bmp[p + 3] = a(i, j);
            }
        }
    }
    d_printf("first pixel: %u\n", (unsigned)bmp[0]);
    d_printf("last pixel: %u\n", (unsigned)bmp[sizeof(unsigned char) * pixel_size - 1]);
    /* Pixels Part Completed. */

    fp = fopen(fname, "wb");
    fwrite((unsigned char *)(&fh) + 2, 1, 14, fp);
    fwrite((unsigned char *)(&ih), 1, 40, fp);
    if(bit_count == 32)
    {
        fwrite((unsigned char *)(&v4ht), 1, 68, fp);
    }
    fwrite(bmp, 1, pixel_size, fp);
    /* File Writing Completed. */

    fclose(fp);
    free(bmp);
    printf("BMP picture file: \"%s\" has been generated.\n", fname);
    return 0;
}
