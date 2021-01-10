package com.example.tflitebenchmark

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle

class MainActivity : AppCompatActivity() {

    private val model = TfModel()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        System.loadLibrary("demo")
        model.initModel(assets)
        model.netBenchmark()
        model.close()
    }

}