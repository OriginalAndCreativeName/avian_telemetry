/*
 * lora.h
 *
 *  Created on: Jul 30, 2026
 *      Author: ftf14
 */

#ifndef INC_LORA_H_
#define INC_LORA_H_

void LoRa_initialize(SPI_HandleTypeDef hspi);
void LoRa_Reset();
void LoRa_write_reg(uint16_t reg, uint8_t* data, uint16_t length);
void LoRa_read_reg(uint16_t reg, uint8_t* data, uint16_t length);
void LoRa_write_buf(uint8_t offset, uint8_t* data, uint16_t length);
void LoRa_set_tx(uint32_t timeout);

void LoRa_tx(uint8_t *payload, uint16_t length, uint32_t timeout);
uint8_t LoRa_tx_ready();
void LoRa_HandleTXIrq();

#endif /* INC_LORA_H_ */
