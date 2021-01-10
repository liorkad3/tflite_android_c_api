#include "TfliteNet.h"
#include <tensorflow/lite/delegates/gpu/delegate.h>
#include <android/log.h>

TfliteNet::TfliteNet(bool isGpu, int numThreads) {
    this->isGpu = isGpu;
    this->numThreads = numThreads;
}

TfLiteStatus TfliteNet::init(const char *modelContent, const size_t modelSize) {
    model = TfLiteModelCreate(modelContent, modelSize);

    TfLiteInterpreterOptions *options = TfLiteInterpreterOptionsCreate();
    TfLiteInterpreterOptionsSetNumThreads(options, numThreads);

    if (isGpu) {
        TfLiteGpuDelegateOptionsV2 gpuOptions = TfLiteGpuDelegateOptionsV2Default();
//        set gpu options here
        gpuOptions.inference_preference = TFLITE_GPU_INFERENCE_PREFERENCE_SUSTAINED_SPEED;
        delegate = TfLiteGpuDelegateV2Create(&gpuOptions);

        TfLiteInterpreterOptionsAddDelegate(options, delegate);
    }
    interpreter = TfLiteInterpreterCreate(model, options);

    //    delete options
    TfLiteInterpreterOptionsDelete(options);
    TfLiteModelDelete(model);

//    allocating should be performed only on init or after resizing input.
    TfLiteStatus status = TfLiteInterpreterAllocateTensors(interpreter);
    if (status != kTfLiteOk) {
        __android_log_print(ANDROID_LOG_DEBUG, "TfModelJni", "allocate tensors failed");
        return status;
    }
    return kTfLiteOk;
}

TfLiteStatus TfliteNet::forward(vector<FloatTensor> &inputs, vector<FloatTensor> &outputs) {
    TfLiteStatus status;
//    copy inputs from buffers
    for (int i = 0; i < inputs.size(); ++i) {
        TfLiteTensor *inputTensor = TfLiteInterpreterGetInputTensor(interpreter, i);
        status = TfLiteTensorCopyFromBuffer(inputTensor, inputs[i].data(),
                                            inputs[i].size() * sizeof(float));
        if (status != kTfLiteOk) {
            __android_log_print(ANDROID_LOG_DEBUG, "TfModelJni", "failed to create input tensor");
            return status;
        }
    }
//    run inference
    status = TfLiteInterpreterInvoke(interpreter);
    if (status != kTfLiteOk) {
        __android_log_print(ANDROID_LOG_DEBUG, "TfModelJni", "error invoke interpreter");
        return status;
    }

//  extract outputs
    int numOutputs = TfLiteInterpreterGetOutputTensorCount(interpreter);
    outputs.reserve(numOutputs);
    for (int i = 0; i < numOutputs; ++i) {
        const TfLiteTensor *outputTensor = TfLiteInterpreterGetOutputTensor(interpreter, i);
        size_t outputSize = TfLiteTensorByteSize(outputTensor) / sizeof(float);
        __android_log_print(ANDROID_LOG_DEBUG, "TfModelJni", "output-size=%zu", outputSize);
        FloatTensor outBuffer(outputSize);
        status = TfLiteTensorCopyToBuffer(outputTensor, outBuffer.data(),
                                          outBuffer.size() * sizeof(float));
        if (status != kTfLiteOk) {
            __android_log_print(ANDROID_LOG_DEBUG, "TfModelJni", "failed to extract output tensor");
            return status;
        }
        __android_log_print(ANDROID_LOG_DEBUG, "TfModelJni", "test-output=%f %f %f",
                            outBuffer[0], outBuffer[outputSize/2], outBuffer[outputSize-1]);
        outputs.emplace_back(outBuffer);
    }
    return kTfLiteOk;
}

void TfliteNet::benchmark() {
    int warmUp = 20;
    int test = 50;

    vector<FloatTensor> inputs = getDefaultInputs();
    vector<FloatTensor> outputs;
//    warmup
    for (int i = 0; i < warmUp; ++i) {
        forward(inputs, outputs);
    }
}

void TfliteNet::release() {
    __android_log_print(ANDROID_LOG_DEBUG, "TfModelJni", "release resources");
    TfLiteInterpreterDelete(interpreter);
    if (isGpu){
        TfLiteGpuDelegateV2Delete(delegate);
    }
}

vector<FloatTensor> TfliteNet::getDefaultInputs() {
    int numInputs = TfLiteInterpreterGetInputTensorCount(interpreter);
    __android_log_print(ANDROID_LOG_DEBUG, "TfModelJni", "num-inputs=%d", numInputs);
    vector<FloatTensor> inputs;
    inputs.reserve(numInputs);
    for (int i = 0; i < numInputs; ++i) {
        TfLiteTensor* tensor = TfLiteInterpreterGetInputTensor(interpreter, i);
        size_t inputSize = TfLiteTensorByteSize(tensor) / sizeof(float);
        __android_log_print(ANDROID_LOG_DEBUG, "TfModelJni", "input-size=%zu", inputSize);
        inputs.emplace_back(FloatTensor(inputSize, 0.5));
    }
    return inputs;
}




