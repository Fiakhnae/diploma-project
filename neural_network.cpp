#include "neural_network.h"

const char * tag = "Neural Network";

namespace {
    constexpr int kTensorArenaSize = 12 * 1024;
    alignas(16) uint8_t tensor_arena[kTensorArenaSize] = {0};
    tflite::MicroInterpreter* interpreter = nullptr;
    TfLiteTensor* input = nullptr;
    TfLiteTensor* output = nullptr;
}

extern "C" void Model_Init(void) 
{
    static tflite::MicroMutableOpResolver<8> resolver;
    resolver.AddFullyConnected();
    resolver.AddRelu();
    resolver.AddQuantize();
    resolver.AddDequantize();
    resolver.AddSub();
    resolver.AddMul();
    resolver.AddReshape();
    resolver.AddAdd();


    const tflite::Model* model = tflite::GetModel(natural_light_model_tflite);
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize
    );
    interpreter = &static_interpreter;

    if (model->version() != TFLITE_SCHEMA_VERSION) 
        MicroPrintf("TFLite version %d, model version %d", TFLITE_SCHEMA_VERSION, model->version());

    TfLiteStatus status = interpreter->AllocateTensors();
    if (status != kTfLiteOk) {
        ESP_LOGE(tag, "Tensor allocation failed");
        return;
    }

    ESP_LOGI(tag, "Initialized. Arena used %d/%d bytes",
             interpreter->arena_used_bytes(), kTensorArenaSize);


    input  = interpreter->input(0);
    output = interpreter->output(0);

    if (input == nullptr || output == nullptr)
        ESP_LOGE(tag, "Input or output is null");
        

    int in_axes        = input->dims->size;           // скільки осей (має бути 1)
    int in_elements    = 1;
    for (int i = 0; i < in_axes; ++i) 
    {
        in_elements *= input->dims->data[i];          // product по всіх осях (у нас просто 3)
    }
    ESP_LOGI(tag, "Input shape: [%d], elements=%d, bytes=%u",
            input->dims->data[0], in_elements, input->bytes);

    // Розмірність output
    int out_axes        = output->dims->size;          // скільки осей (1)
    int out_elements    = 1;
    for (int i = 0; i < out_axes; ++i) 
    {
        out_elements *= output->dims->data[i];         // product (у нас це 1)
    }
    ESP_LOGI(tag, "Output shape: [%d], elements=%d, bytes=%u",
         output->dims->data[0], out_elements, output->bytes);

    if (input->type != kTfLiteFloat32)
        ESP_LOGE(tag, "Type of input is not Float32");

    if (output->type != kTfLiteFloat32) 
       ESP_LOGE(tag, "Type of output is not Float32");

    ESP_LOGI(tag, "Inputs: %u", interpreter->inputs_size());
    ESP_LOGI(tag, "Outputs: %u", interpreter->outputs_size());
    const auto* subgraph = model->subgraphs()->Get(0);
    ESP_LOGI(tag, "Model tensors: %lu", subgraph->tensors()->size());
}

extern "C" uint16_t Predict_Color_Temperature(uint16_t time_min, uint16_t lux, uint16_t current_ct) 
{
    float time_min_f = (float)time_min / 1440.0f;
    float lux_f = (float)lux / 65536.0f;
    float current_ct_f = (float)current_ct / 65536.0f;

    /*ESP_LOGI(tag, "time_min_f: %f", time_min_f);
    ESP_LOGI(tag, "lux_f: %f", lux_f);
    ESP_LOGI(tag, "current_ct_f: %f", current_ct_f);*/

    /*const float input_scale = input->params.scale;
    const int input_zero_point = input->params.zero_point;

    int8_t* in_data = input->data.int8;
    in_data[0] = static_cast<int8_t>(roundf(time_min_f / input_scale) + input_zero_point);
    in_data[1] = static_cast<int8_t>(roundf(lux_f / input_scale) + input_zero_point);
    in_data[2] = static_cast<int8_t>(roundf(current_ct_f / input_scale) + input_zero_point);*/

    float* in_data = input->data.f;
    in_data[0] = time_min_f;
    in_data[1] = lux_f;
    in_data[2] = current_ct_f;

    /*ESP_LOGI(tag, "In_data (quantized): [%d, %d, %d]",
         in_data[0], in_data[1], in_data[2]);*/

    if (interpreter->Invoke() != kTfLiteOk) 
    {
        ESP_LOGE(tag, "Invoke failed");
        return 0;
    }

    /*const float output_scale = output->params.scale;
    const int output_zero_point = output->params.zero_point;

    int8_t quantized_output = output->data.int8[0];
    float dequantized = (quantized_output - output_zero_point) * output_scale;

    float result = dequantized * 65536.0f;
    ESP_LOGI(tag, "result: %f", result);*/

    float output_value = output->data.f[0];
    float result = output_value * 65536.0f;
    return static_cast<uint16_t>(result);
}

