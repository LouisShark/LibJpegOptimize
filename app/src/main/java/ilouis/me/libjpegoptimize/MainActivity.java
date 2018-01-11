package ilouis.me.libjpegoptimize;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * @author user
 */
public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("compress");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.head);

        /**
         * 质量压缩
         */
        compressBitmap(bitmap, Bitmap.CompressFormat.JPEG, 50, Environment.getExternalStorageDirectory() + "/test.jpeg");

        /**
         * 尺寸压缩
         */
        Bitmap scaleBitmap = Bitmap.createScaledBitmap(bitmap, 320, 320, true);

        compressBitmap(scaleBitmap, Bitmap.CompressFormat.JPEG, 100, Environment.getExternalStorageDirectory()+ "/test_scale.jpeg");


        //png格式
        compressBitmap(bitmap, Bitmap.CompressFormat.PNG, 100, Environment.getExternalStorageDirectory() + "/test.png");
        //webp格式
        compressBitmap(bitmap, Bitmap.CompressFormat.WEBP, 100, Environment.getExternalStorageDirectory() + "/test.webp");


        nativeCompressBitmap(bitmap, 50, Environment.getExternalStorageDirectory() + "/test_native.jpeg");


    }

    public native void nativeCompressBitmap(Bitmap bitmap, int quality, String path);

    /**
     * 压缩图片到制定文件
     * @param bitmap 待压缩图片
     * @param format 压缩的格式
     * @param q      质量
     * @param path  文件地址
     */
    public void compressBitmap(Bitmap bitmap, Bitmap.CompressFormat format, int q, String path) {
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(path);
            bitmap.compress(format, q, fos);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } finally {
            if (null != fos) {
                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

    }
}
