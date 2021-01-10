
#ifndef TFLITEBENCHMARK_TFLITENET_H
#define TFLITEBENCHMARK_TFLITENET_H

#include <tensorflow/lite/c/c_api.h>
#include <vector>

using namespace std;

typedef vector<float> FloatTensor;

class TfliteNet {

public:
    TfliteNet(bool isGpu, int numThreads);
    TfLiteStatus init(const char* modelContent, const size_t modelSize);
    TfLiteStatus forward(vector<FloatTensor>& inputs, vector<FloatTensor>& outputs);
    void release();

    void benchmark();
private:
    vector<FloatTensor> getDefaultInputs();

    bool isGpu = false;
    int numThreads = 4;
    TfLiteInterpreter *interpreter;
    TfLiteDelegate *delegate;
    TfLiteModel *model;

};


#endif //TFLITEBENCHMARK_TFLITENET_H
