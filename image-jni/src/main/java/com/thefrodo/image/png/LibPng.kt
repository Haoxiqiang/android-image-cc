package com.thefrodo.image.png

import androidx.annotation.Keep
import com.thefrodo.image.FrdImage

@Keep
class LibPng {

    fun isPng(path: String): Boolean {
        FrdImage.nativeLoad(FrdImage.TAG_LibPNG)
        return isPNGImage(path)
    }

    fun printPNgInfo(path: String) {
        FrdImage.nativeLoad(FrdImage.TAG_LibPNG)
        return pngInfo(path)
    }

    private external fun isPNGImage(path: String): Boolean

    private external fun pngInfo(path: String)
}
