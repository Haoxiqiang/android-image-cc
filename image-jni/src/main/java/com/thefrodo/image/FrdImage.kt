package com.thefrodo.image

import android.content.Context
import kotlinx.coroutines.Dispatchers
import java.io.File
import java.util.concurrent.ConcurrentHashMap

object FrdImage {

    const val TAG_LibJpeg = "jpeg-turbo"
    const val TAG_LibPNG = "frdpng"

    private val concurrentHashMap = ConcurrentHashMap<String, Boolean>()
    var nativeLoader: ((lib: String) -> Boolean)? = null

    fun nativeLoad(libName: String) {
        if (concurrentHashMap[libName] != true) {
            if (nativeLoader == null) {
                System.loadLibrary(libName)
                concurrentHashMap[libName] = true
            } else {
                if (nativeLoader!!.invoke(libName)) {
                    concurrentHashMap[libName] = true
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
