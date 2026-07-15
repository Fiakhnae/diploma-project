
#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "natural_light_model.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "esp_nn.h"

extern "C" {
#endif

void Model_Init(void);
uint16_t Predict_Color_Temperature(uint16_t time_min, uint16_t lux, uint16_t current_ct);

#ifdef __cplusplus
}
#endif

#endif
