package com.thefrodo.image

import android.content.Context
import kotlinx.coroutines.Dispatchers
import java.io.File
import java.util.concurrent.atomic.AtomicBoolean

object FrdImage {

    const val TAG_LibJpeg = "jpeg-turbo"
    const val TAG_LibPNG = "frdpng"

    private val nativeLoaded = AtomicBoolean(false)
    var nativeLoader: ((lib: String) -> Boolean)? = null

    fun nativeLoad() {
        if (nativeLoaded.get().not()) {
            if (nativeLoader == null) {
                System.loadLibrary("frdimage")
                nativeLoaded.set(true)
            } else {
                if (nativeLoader!!.invoke("frdimage")) {
                    nativeLoaded.set(true)
                }
            }
        }
    }

    fun Context.fileFromAsset(name: String): File = with(Dispatchers.IO) {
        val cache = File("$cacheDir/$name")
        if (cache.exists()) {
            return cache
        }
        return cache.apply { writeBytes(assets.open(name).readBytes()) }
    }
}
