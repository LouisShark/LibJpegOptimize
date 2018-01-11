#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <malloc.h>
#include <jpeglib.h>

void write_JPEG_file(uint8_t *temp, int w, int h, jint quality, const char *path) {
    //1.创建jpeg压缩对象
    jpeg_compress_struct jpegCompressStruct;
    //错误回调
    jpeg_error_mgr error;
    jpegCompressStruct.err = jpeg_std_error(&error);
    //创建压缩对象
    jpeg_create_compress(&jpegCompressStruct);

    //2.指定存储文件
    FILE* f = fopen(path, "wb");
    jpeg_stdio_dest(&jpegCompressStruct, f);

    //3.设置压缩参数
    jpegCompressStruct.image_width = (JDIMENSION) w;
    jpegCompressStruct.image_height = (JDIMENSION) h;
    //bgr
    jpegCompressStruct.input_components = 3;
    jpegCompressStruct.in_color_space = JCS_RGB;
    jpeg_set_defaults(&jpegCompressStruct);
    //开启哈夫曼
    jpegCompressStruct.optimize_coding = JNI_TRUE;
    jpeg_set_quality(&jpegCompressStruct, quality, JNI_TRUE);

    //4.开始压缩
    jpeg_start_compress(&jpegCompressStruct, JNI_TRUE);

    //5.循环写入每一行数据
    int row_stride = w * 3;
    //next_scanline 一行数据开头的位置
    JSAMPROW row[1];
    while (jpegCompressStruct.next_scanline < jpegCompressStruct.image_height) {
        //拿一行数据
        uint8_t *pixels = temp + jpegCompressStruct.next_scanline * row_stride;
        row[0] = pixels;
        jpeg_write_scanlines(&jpegCompressStruct, row, 1);
    }
    //6.压缩完成
    jpeg_finish_compress(&jpegCompressStruct);

    //7.释放jpeg对象
    fclose(f);
    jpeg_destroy_compress(&jpegCompressStruct);
}
extern "C"
void JNICALL
Java_ilouis_me_libjpegoptimize_MainActivity_nativeCompressBitmap(JNIEnv *env, jobject instance,
                                                                 jobject bitmap, jint quality,
                                                                 jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    //从bitmap获得argb数据
    AndroidBitmapInfo info;
    //获得bitmap的信息 比如宽丶高
    AndroidBitmap_getInfo(env, bitmap, &info);

    //c/c++ char = byte
    uint8_t *pixels;
    AndroidBitmap_lockPixels(env, bitmap, (void **) pixels);

    //jpeg 不是透明的
    //jpeg去掉透明度
    int w = info.width;
    int h = info.height;
    int color;
    //rgb
    uint8_t *data = (uint8_t *) malloc((size_t) (w * h * 3));
    uint8_t *temp = data;
    uint8_t r, g, b;
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            color = *(int *) pixels;
//            0xaarrggbb
            r = (uint8_t) ((color >> 16) & 0xFF);
            g = (uint8_t) ((color >> 8) & 0xFF);
            b = (uint8_t) (color & 0xFF);


            /**
             * lijpeg 是按照bgr来排列的
             */
            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            data += 3;


            //指针向后移动4字节
            pixels += 4;
        }
    }
    write_JPEG_file(temp, w, h, quality, path);

    AndroidBitmap_unlockPixels(env, bitmap);
    free(data);
    free(temp);
    env->ReleaseStringUTFChars(path_, path);
}

