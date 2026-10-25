#include <math.h>

float calculate_rms(float *buffer, int length) {
    float sum = 0.0f;
    for (int i = 0; i < length; i++) {
        sum += buffer[i] * buffer[i];
    }
    return sqrtf(sum / length);
}