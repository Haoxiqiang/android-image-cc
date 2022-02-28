package com.thefrodo.image.png

import android.content.Context
import androidx.test.core.app.ApplicationProvider
import com.thefrodo.image.FrdImage.fileFromAsset
import org.junit.Assert
import org.junit.Test

class LibPngTest {

    private var libPng = LibPng()
    private val context = ApplicationProvider.getApplicationContext<Context>()

    @Test
    @Throws(Exception::class)
    fun testIsPng() {

        val jpegFile = context.fileFromAsset("BoguraChili.jpeg")

        val jpegResult = libPng.isPng(jpegFile.absolutePath)
        Assert.assertEquals(false, jpegResult)

        val pngFile = context.fileFromAsset("mario.png")
        val pngResult = libPng.isPng(pngFile.absolutePath)
        Assert.assertEquals(true, pngResult)

        if (pngResult) {
            libPng.printPNgInfo(pngFile.absolutePath)
        }
    }
}
