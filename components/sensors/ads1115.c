#include "ads1115.h"
#include "driver/i2c.h"
#include <math.h>

#define I2C_PORT I2C_NUM_0
#define ADS1115_ADDR 0x48
#define ADS1115_REG_CONV 0x00
#define ADS1115_REG_CONFIG 0x01

#define BURDEN_RESISTOR_OHMS 33.0f
#define BIAS_VOLTAGE 1.65f // VCC/2 from bias divider

// Helper to write to a register
static esp_err_t i2c_write_register(uint8_t dev_addr, uint8_t reg, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}

// Helper to read from a register
static esp_err_t i2c_read_register(uint8_t dev_addr, uint8_t reg, uint8_t *buf, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    // Write phase
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    
    // Read phase
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
    
    if (len > 1) {
        i2c_master_read(cmd, buf, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, buf + len - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t ads1115_init(void) {
    // Config: 860SPS, continuous mode, +/- 2.048V range, AIN0 single-ended
    uint16_t config = 0x4283;
    uint8_t data[2] = { (config >> 8) & 0xFF, config & 0xFF };
    return i2c_write_register(ADS1115_ADDR, ADS1115_REG_CONFIG, data, 2);
}

float ads1115_read_voltage(void) {
    uint8_t buf[2];
    if (i2c_read_register(ADS1115_ADDR, ADS1115_REG_CONV, buf, 2) != ESP_OK) {
        return 0.0f;
    }
    
    // Combine MSB and LSB into signed 16-bit integer
    int16_t raw = (int16_t)((buf[0] << 8) | buf[1]);
    
    // Convert to voltage
    return (float)raw * (2.048f / 32768.0f);
}

float ads1115_voltage_to_current(float voltage) {
    return (voltage - BIAS_VOLTAGE) / BURDEN_RESISTOR_OHMS;
}