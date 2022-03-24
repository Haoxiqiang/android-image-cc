package com.thefrodo.image.heif

import android.content.Context
import androidx.test.core.app.ApplicationProvider
import androidx.test.filters.SmallTest
import androidx.test.runner.AndroidJUnit4
import com.nokia.heif.AVCImageItem
import com.nokia.heif.GridImageItem
import com.nokia.heif.HEIF
import com.nokia.heif.HEVCImageItem
import com.nokia.heif.JPEGImageItem
import com.nokia.heif.io.ByteArrayInputStream
import org.junit.Test
import org.junit.runner.RunWith
import java.io.InputStream

@RunWith(AndroidJUnit4::class)
@SmallTest
class LibHeif {

    private val context = ApplicationProvider.getApplicationContext<Context>()

    @Test
    fun loadSingleImage() {
        // Create an instance of the HEIF library,
        val heif = HEIF()
        try {
            // Load the file
            val inputStream: InputStream = context.assets.open("Growing.heic")
            heif.load(ByteArrayInputStream(inputStream.readBytes()))
            // Get the primary image
            val primaryImage = heif.primaryImage
            // Check the type, assuming that it's a HEVC image
            if (primaryImage is HEVCImageItem) {
                val hevcImageItem = primaryImage
                val decoderConfig = hevcImageItem.decoderConfig.config
                val imageData = hevcImageItem.itemDataAsArray
                // Feed the data to a decoder
            } else if (primaryImage is GridImageItem) {
                val columnCount = primaryImage.columnCount
                val rowCount = primaryImage.rowCount
                for (c in 0..columnCount) {
                    for (r in 0..rowCount) {
                        val image = primaryImage.getImage(c, r)
                        if (image is HEVCImageItem) {
                            val decoderConfig = image.decoderConfig.config
                            val imageData = image.itemDataAsArray
                        } else if (image is JPEGImageItem) {
                        } else if (image is AVCImageItem) {
                        }
                    }
                }
            }
        } // All exceptions thrown by the HEIF library are of the same type
        // Check the error code to see what happened
        catch (e: Exception) {
            e.printStackTrace()
        }
    }
}
