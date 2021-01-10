package com.example.tflitebenchmark

import android.content.res.AssetManager

class TfModel {

    companion object{
        const val MODEL_NAME = "mobilenet.tflite"
    }
    private var mNetAddress: Long = 0

    fun initModel(assetManager: AssetManager){
        mNetAddress = createInstance(assetManager, isGpu = false)
    }

    fun netBenchmark() = bench(mNetAddress)

    fun close() = release(mNetAddress)

    private external fun createInstance(assetManager: AssetManager,
                                        modelName: String = MODEL_NAME,
                                        isGpu: Boolean = true,
                                        numThreads: Int =4
    ): Long

    private external fun release(netAddress: Long)

    private external fun bench(netAddress: Long)
}