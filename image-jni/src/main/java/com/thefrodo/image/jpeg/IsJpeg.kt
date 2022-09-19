package com.thefrodo.image.jpeg

import java.io.BufferedInputStream

private val JPEG_SIGNATURE = byteArrayOf(0xff.toByte(), 0xd8.toByte(), 0xff.toByte())

// Maximum size for checking file type signature (see image_type_recognition_lite.cc)
private const val SIGNATURE_CHECK_SIZE = 5000

fun LibJpeg.isJpeg(inputStream: BufferedInputStream): Boolean {
    inputStream.mark(SIGNATURE_CHECK_SIZE)
    val signatureCheckBytes = ByteArray(SIGNATURE_CHECK_SIZE)
    inputStream.read(signatureCheckBytes)

    for (i in JPEG_SIGNATURE.indices) {
        if (signatureCheckBytes[i] != JPEG_SIGNATURE[i]) {
            return false
        }
    }
    return true
}
