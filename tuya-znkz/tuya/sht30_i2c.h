/*
 * Hard.h
 *
 *  Created on: Feb 4, 2021
 *      Author: LiQi
 */

#ifndef USER_LIB_INC_SHT30_I2C_H_
#define USER_LIB_INC_SHT30_I2C_H_
#include "main.h"
//SHT30地址
#define SHT_ADDR (0x44<<1)

//外部硬件定义
extern I2C_HandleTypeDef hi2c1;

//SHT30写命令
uint8_t i2c_write_cmd(uint16_t cmd);
//SHT30初始化
uint8_t sht30_init();
//获取SHT30数值
uint8_t sht30_sample(float *t, float *h);

#endif /* USER_LIB_INC_SHT30_I2C_H_ */
