package com.thefrodo.image.jpeg

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Matrix
import android.os.Build
import android.util.Log
import androidx.annotation.IntRange
import androidx.annotation.Keep
import androidx.exifinterface.media.ExifInterface
import java.io.BufferedInputStream
import java.io.ByteArrayOutputStream
import java.io.File
import java.util.concurrent.atomic.AtomicBoolean

@Keep
class LibJpeg {

    companion object {
        const val TAG = "LibJpeg"
        private const val quality = 80
        private val nativeLoaded = AtomicBoolean(false)
    }

    enum class CompressMode {
        DEFAULT,
        ForceLibJpeg,
        ForceSystem,
        ForceLibJpegBitmap,
    }

    @Synchronized
    fun compress(
        mode: CompressMode,
        src: File,
        dst: File,
        overwrite: Boolean = false,
        keepAlpha: Boolean = false,
        nativeLoader: ((lib: String) -> Boolean)? = null
    ): Boolean {

        if (overwrite) {
            if (dst.exists()) {
                dst.deleteRecursively()
            }
        }

        if (dst.exists()) {
            Log.e(TAG, "dst file exists.")
            return false
        }

        if (nativeLoaded.get().not()) {
            if (nativeLoader == null) {
                System.loadLibrary("jpeg-turbo")
                nativeLoaded.set(true)
            } else {
                if (nativeLoader.invoke("jpeg-turbo")) {
                    nativeLoaded.set(true)
                }
            }
        }

        val isJpegFormat = try {
            val inputStream = src.inputStream()
            val bis = BufferedInputStream(inputStream)
            val isJpeg = isJpeg(bis)
            bis.close()
            isJpeg
        } catch (throwable: Throwable) {
            throwable.printStackTrace()
            return false
        }

        val useLibJpeg =
            isJpegFormat &&
                (mode == CompressMode.ForceLibJpeg || (mode == CompressMode.DEFAULT && Build.VERSION.SDK_INT < Build.VERSION_CODES.N))
        if (isJpegFormat && mode == CompressMode.ForceLibJpegBitmap) {
            val outBitmap = loadBitmapFromFile(src)
            if (outBitmap == null) {
                Log.e(TAG, "decode bitmap from source file failed.")
                return false
            }
            return nativeCompressBitmap(outBitmap, dst.absolutePath, quality)
        } else if (useLibJpeg) {
            return nativeCompress(src.absolutePath, dst.absolutePath, quality) == 1
        } else {
            val outBitmap = loadBitmapFromFile(src)

            if (outBitmap == null) {
                Log.e(TAG, "decode bitmap from source file failed.")
                return false
            }

            dst.outputStream().let { stream ->
                val bos = ByteArrayOutputStream()
                val ret = if (keepAlpha) {
                    outBitmap.compress(Bitmap.CompressFormat.PNG, quality, bos)
                } else {
                    outBitmap.compress(Bitmap.CompressFormat.JPEG, quality, bos)
                }
                if (ret.not()) {
                    bos.close()
                    stream.close()
                    outBitmap.recycle()
                    Log.e(TAG, "bitmap compress failed.")
                    return false
                }
                stream.write(bos.toByteArray())
                stream.flush()
                stream.close()
                bos.close()
                outBitmap.recycle()
                return ret
            }
        }
    }

    private fun loadBitmapFromFile(src: File): Bitmap? {
        val preOptions = BitmapFactory.Options()
        preOptions.inJustDecodeBounds = true
        preOptions.inSampleSize = 1

        BitmapFactory.decodeFile(src.absolutePath, preOptions)

        val srcWidth = preOptions.outWidth
        val srcHeight = preOptions.outHeight

        val outOptions = BitmapFactory.Options()
        outOptions.inSampleSize = calImageSubsamplingSize(srcWidth, srcHeight)

        var outBitmap = BitmapFactory.decodeFile(src.absolutePath, outOptions)
        if (outBitmap != null) {
            try {
                val exifInterface = ExifInterface(src)
                val rotation = exifInterface.rotationDegrees
                if (rotation > 0) {
                    outBitmap = rotatingImage(outBitmap, rotation)
                }
            } catch (throwable: Throwable) {
                throwable.printStackTrace()
            }
        }
        return outBitmap
    }

    private fun rotatingImage(bitmap: Bitmap?, angle: Int): Bitmap {
        val matrix = Matrix()
        matrix.postRotate(angle.toFloat())
        return Bitmap.createBitmap(bitmap!!, 0, 0, bitmap.width, bitmap.height, matrix, true)
    }

    private external fun calImageSubsamplingSize(width: Int, height: Int): Int

    /**
     * @param fancyUpsampling default true.
     *      if disable/false ,jpeg decompress will be faster 15-20% than true, but will loss more.
     *      Whether or not the quality loss from merged upsampling is noticeable depends on the image.
     */
    private external fun nativeCompress(
        src: String,
        dst: String,
        @IntRange(from = 0, to = 100) quality: Int,
        fancyUpsampling: Boolean = true,
    ): Int

    private external fun nativeCompressBitmap(
        bitmap: Bitmap,
        dst: String,
        @IntRange(from = 0, to = 100) quality: Int,
    ): Boolean
}
