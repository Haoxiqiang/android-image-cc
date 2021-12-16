@file:Suppress("DEPRECATION")

package com.thefrodo.image.jpeg

import android.os.SystemClock
import android.util.Log
import androidx.test.filters.SmallTest
import androidx.test.platform.app.InstrumentationRegistry.getInstrumentation
import androidx.test.runner.AndroidJUnit4
import org.junit.After
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import java.io.File

@RunWith(AndroidJUnit4::class)
@SmallTest
class LibJpegTest {

    private val jpegTurbo = LibJpeg()
    private var cacheDir: File
    private val originFile: File

    init {
        val context = getInstrumentation().context
        cacheDir = context.externalCacheDir!!
        cacheDir.mkdirs()
        originFile = File(context.cacheDir, "test4k.jpeg")
    }

    @Before
    fun setup() {
        val context = getInstrumentation().context
        cacheDir = context.externalCacheDir!!
        cacheDir.mkdirs()
        if (originFile.exists()) {
            originFile.deleteRecursively()
        }
        originFile.outputStream().use { cache ->
            getInstrumentation().context.assets.open(originFile.name)
                .use { inputStream ->
                    inputStream.copyTo(cache)
                }
        }
    }

    @After
    fun copyOrigin() {
        source()
    }

    private fun source(): File {
        val srcFile = File(cacheDir, "origin.jpeg")
        originFile.copyTo(srcFile, true)
        return srcFile
    }

    @Test
    fun compressBySystem() {
        // test once
        val src = source()
        val dstSysFile = File(cacheDir, "test-sys.jpeg")
        val start = SystemClock.elapsedRealtime()
        assert(
            jpegTurbo.compress(
                mode = LibJpeg.CompressMode.ForceSystem,
                src = src,
                dst = dstSysFile,
                overwrite = true,
            )
        )
        val end = SystemClock.elapsedRealtime()
        Log.d(LibJpeg.TAG, "LibJpeg#compressBySystem cost=${end - start}ms")
    }

    @Test
    fun compressLibJpeg() {
        // test once
        val src = source()
        val dstFile = File(cacheDir, "test.jpeg")
        val start = SystemClock.elapsedRealtime()
        assert(
            jpegTurbo.compress(
                mode = LibJpeg.CompressMode.ForceLibJpeg,
                src = src,
                dst = dstFile,
                overwrite = true,
            )
        )
        val end = SystemClock.elapsedRealtime()
        Log.d(LibJpeg.TAG, "LibJpeg#compressLibJpeg cost=${end - start}ms")
    }

    @Test
    fun compressWithAlpha() {
        // test once
        val src = source()
        val dstWithAlphaFile = File(cacheDir, "test-alpha.jpeg")
        val start = SystemClock.elapsedRealtime()
        assert(
            jpegTurbo.compress(
                mode = LibJpeg.CompressMode.ForceLibJpeg,
                src = src,
                dst = dstWithAlphaFile,
                keepAlpha = true,
                overwrite = true,
            )
        )
        val end = SystemClock.elapsedRealtime()
        Log.d(LibJpeg.TAG, "LibJpeg#compressWithAlpha cost=${end - start}ms")
    }

    @Test
    fun compressBitmap() {
        // test once
        val src = source()
        val dstFromBitmapFile = File(cacheDir, "test-from-bitmap.jpeg")
        val start = SystemClock.elapsedRealtime()
        assert(
            jpegTurbo.compress(
                mode = LibJpeg.CompressMode.ForceLibJpegBitmap,
                src = src,
                dst = dstFromBitmapFile,
                keepAlpha = true,
                overwrite = true,
            )
        )
        val end = SystemClock.elapsedRealtime()
        Log.d(LibJpeg.TAG, "LibJpeg#compressBitmap cost=${end - start}ms")
    }

    @Test
    fun compressSys100() {
        // test sys 100th
        val src = source()
        val dstSys100File = File(cacheDir, "test-sys-100.jpeg")
        var cost = 0L
        for (i in 0..99) {
            val start = SystemClock.elapsedRealtime()
            assert(
                jpegTurbo.compress(
                    mode = LibJpeg.CompressMode.ForceSystem,
                    src = src,
                    dst = dstSys100File,
                    overwrite = true,
                )
            )
            val end = SystemClock.elapsedRealtime()
            cost += end - start
            dstSys100File.copyTo(src, true)
        }
        Log.d(LibJpeg.TAG, "LibJpeg#compressSys100 cost=${cost}ms avg:${cost.toFloat() / 100}")
    }

    //
    @Test
    fun compress100() {
        // test 100th
        val src = source()
        val dst100File = File(cacheDir, "test100.jpeg")
        var cost = 0L
        for (i in 0..99) {
            val start = SystemClock.elapsedRealtime()
            assert(
                jpegTurbo.compress(
                    mode = LibJpeg.CompressMode.ForceLibJpeg,
                    src = src,
                    dst = dst100File,
                    overwrite = true,
                )
            )
            val end = SystemClock.elapsedRealtime()
            cost += end - start

            dst100File.copyTo(src, true)
        }

        Log.d(LibJpeg.TAG, "LibJpeg#compress100 cost=${cost}ms avg:${cost.toFloat() / 100}")
    }
}
