package com.thefrodo.image.samples

import android.os.Bundle
import android.util.Log
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import com.thefrodo.image.FrdImage.fileFromAsset
import com.thefrodo.image.jpeg.LibJpeg
import com.thefrodo.image.png.LibPng
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

class MainActivity : AppCompatActivity() {

    private val libPng by lazy { LibPng() }
    private val libJpeg by lazy { LibJpeg() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        // NOTHING
        // ONLY BY TESTS FOR DEV ENV.

        findViewById<View>(R.id.isJpeg).setOnClickListener {
            lifecycleScope.launchWhenCreated {
                withContext(Dispatchers.IO) {
                    val jpegFile = fileFromAsset("BoguraChili.jpeg")
                    Log.d(
                        "LibJpeg",
                        "LibJpeg.isJpeg(path)= ${libJpeg.isJpeg(jpegFile.absolutePath)}"
                    )
                    libJpeg.printJpegInfo(jpegFile.absolutePath)
                }
            }
        }

        findViewById<View>(R.id.isPNG).setOnClickListener {
            lifecycleScope.launchWhenCreated {
                withContext(Dispatchers.IO) {
                    val jpegFile = fileFromAsset("BoguraChili.jpeg")
                    Log.d("LibPNG", "libPng.isPng(file)= ${libPng.isPng(jpegFile.absolutePath)}")

                    val pngFile = fileFromAsset("mario.png")
                    Log.d("LibPNG", "libPng.isPng(file)= ${libPng.isPng(pngFile.absolutePath)}")

                    libPng.printPNgInfo(pngFile.absolutePath)
                }
            }
        }
    }
}
