#include "main.h"
#include "lora.h"
#include "string.h"

#define OP_SET_STANDBY               0x80
#define OP_SET_FS                    0xC1
#define OP_SET_TX                    0x83
#define OP_SET_RX                    0x82
#define OP_STOP_TIMER_ON_PREAMBLE    0x9F
#define OP_SET_RX_DUTY_CYCLE         0x94
#define OP_SET_CAD                   0xC5
#define OP_SET_TX_CONTINUOUS_WAVE    0xD1
#define OP_SET_TX_INFINITE_PREAMBLE  0xD2
#define OP_SET_REGULATOR_MODE        0x96
#define OP_CALIBRATE                 0x89
#define OP_CALIBRATE_IMAGE           0x98
#define OP_SET_PA_CONFIG             0x95
#define OP_SET_RX_TX_FALLBACK_MODE   0x93

#define OP_SET_DIO_IRQ_PARAMS           0x08
#define OP_GET_IRQ_STATUS               0x12
#define OP_CLEAR_IRQ_STATUS             0x02
#define OP_SET_DIO2_AS_RF_SWITCH_CTRL   0x9D
#define OP_SET_DIO3_AS_TCXO_CTRL        0x97

#define OP_WRITE_REG 0x0D
#define OP_READ_REG 0x1D
#define OP_WRITE_BUFFER 0x0E
#define OP_READ_BUFFER 0x1E

#define OP_SET_RF_FREQUENCY             0x86
#define OP_SET_PACKET_TYPE              0x8A
#define OP_GET_PACKET_TYPE              0x11
#define OP_SET_TX_PARAMS                0x8E
#define OP_SET_MODULATION_PARAMS        0x8B
#define OP_SET_PACKET_PARAMS            0x8C
#define OP_SET_CAD_PARAMS               0x88
#define OP_SET_BUFFER_BASE_ADDRESS      0x8F
#define OP_SET_LORA_SYMB_NUM_TIMEOUT    0xA0

#define OP_GET_STATUS                   0xC0
#define OP_GET_RSSI_INST                0x15
#define OP_GET_RX_BUFFER_STATUS         0x13
#define OP_GET_PACKET_STATUS            0x14
#define OP_GET_DEVICE_ERRORS            0x17
#define OP_CLEAR_DEVICE_ERRORS          0x07
#define OP_GET_STATS                    0x10
#define OP_RESET_STATS                  0x00


SPI_HandleTypeDef LoRa_hspi;

uint8_t LoRa_tx_isready = 0;
uint8_t curr_packet_size = 17;


void wait_busy_Tsw(){
	for (int i =0; i<110; i++){
		__NOP();
	}
}

uint8_t LoRa_is_busy(){
	return HAL_GPIO_ReadPin(LoRa_Busy_GPIO_Port,LoRa_Busy_Pin);
}

uint8_t LoRa_tx_ready(){
	return LoRa_tx_isready;
}


void LoRa_HandleTXIrq(){ //clear IRQ, and then set LoRa available again
	uint8_t tx_buf[3];
	tx_buf[0] = OP_CLEAR_IRQ_STATUS;
	tx_buf[1] = 0x00;
	tx_buf[2] = 0b00000001;
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 3, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	wait_busy_Tsw();

	LoRa_tx_isready = 1;
}

void LoRa_write_reg(uint16_t reg, uint8_t *data, uint16_t length){
	uint8_t tx_buf[3];
	tx_buf[0] = OP_WRITE_REG;
	tx_buf[1] = reg >> 8;
	tx_buf[2] = reg & 0xFF;
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 3, 10);
    HAL_SPI_Transmit(&LoRa_hspi, data, length, 10);
    //printf("TX: %X \n", HAL_SPI_Transmit_IT(&LoRa_hspi, data, length));

	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);

}
void LoRa_read_reg(uint16_t reg, uint8_t* data, uint16_t length){
	uint8_t tx_buf[4];

	tx_buf[0] = OP_READ_REG;
	tx_buf[1] = reg >> 8;
	tx_buf[2] = reg & 0xFF;

	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 4, 10);
    HAL_SPI_Receive(&LoRa_hspi, data, length, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
    //printf("RX: %X \n", HAL_SPI_Receive_IT(&LoRa_hspi, data, length));
}

void LoRa_write_buf(uint8_t offset, uint8_t* data, uint16_t length){
	uint8_t tx_buf[4];

	tx_buf[0] = OP_WRITE_BUFFER;
	tx_buf[1] = offset;

	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 2, 10);
    HAL_SPI_Transmit(&LoRa_hspi, data, length, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	wait_busy_Tsw();
    //printf("RX: %X \n", HAL_SPI_Receive_IT(&LoRa_hspi, data, length));
}

void LoRa_set_packet_len(uint8_t length){
	uint8_t tx_buf[10];
	tx_buf[0] = OP_SET_PACKET_PARAMS ;
	tx_buf[1] = 0x00; //preamble len
	tx_buf[2] = 0x08; //preamble len
	tx_buf[3] = 0x00; //explicit
	tx_buf[4] = length; //set packet size
	tx_buf[5] = 0x01; //CRC on
	tx_buf[6] = 0x00; ///standard IQ
	tx_buf[7] = 0x00;
	tx_buf[8] = 0x00;
	tx_buf[9] = 0x00;
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 10, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	wait_busy_Tsw();
}

void LoRa_tx(uint8_t *payload, uint16_t length, uint32_t timeout){
	//add 4 bytes needed by radiohead
	/*
	uint8_t msg_len = length+4;
	uint8_t tx_buf[msg_len];
	tx_buf[0] = 0xFF;
	tx_buf[1] = 0x01;
	tx_buf[2] = 0x00;
	tx_buf[3] = 0x00;
	memcpy(tx_buf + 4, payload, length);*/

	LoRa_write_buf(0, payload, length); //write data to buffer
	while(LoRa_is_busy());
	if(curr_packet_size != length){
		LoRa_set_packet_len(length); //set packet length if it changed
		while(LoRa_is_busy());
		curr_packet_size = length;
	}
	LoRa_set_tx(timeout); //set TX
	LoRa_tx_isready = 0;
}

void LoRa_set_tx(uint32_t timeout){
	uint8_t tx_buf[4];

	tx_buf[0] = OP_SET_TX;
	tx_buf[1] = timeout >> 16;
	tx_buf[2] = timeout >> 8;
	tx_buf[3] = timeout;
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 4, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);

}


void LoRa_Reset(){
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LoRa_NRST_GPIO_Port, LoRa_NRST_Pin, GPIO_PIN_RESET);
	printf("%d", HAL_GPIO_ReadPin(LoRa_Busy_GPIO_Port,LoRa_Busy_Pin));

	HAL_Delay(10);
	printf("%d", HAL_GPIO_ReadPin(LoRa_Busy_GPIO_Port,LoRa_Busy_Pin));
	HAL_GPIO_WritePin(LoRa_NRST_GPIO_Port, LoRa_NRST_Pin, GPIO_PIN_SET);
	printf("%d", HAL_GPIO_ReadPin(LoRa_Busy_GPIO_Port,LoRa_Busy_Pin));
	HAL_Delay(10);
}
void LoRa_initialize(SPI_HandleTypeDef hspi){
	LoRa_hspi = hspi;

	uint8_t tx_buf[10];
	tx_buf[0] = OP_SET_DIO3_AS_TCXO_CTRL;
	tx_buf[1] = 0x05; //txco voltage
	tx_buf[2] = 0; //DELAY, done at the start of each msg.
	tx_buf[3] = 0x01; //set to 5ms
	tx_buf[4] = 0x40;
	//tx_buf[3] = 0xFF; //set to 1s
	//tx_buf[4] = 0xFF;
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 5, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	tx_buf[0] = OP_SET_DIO2_AS_RF_SWITCH_CTRL;
	tx_buf[1] = 0x01; //txco voltage
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 2, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_Delay(10);


	uint8_t temp; //modify TxClampConfig as recommended by datasheet
	LoRa_read_reg(0x08D8, &temp, 1);
	temp = temp | 0x1E;
	LoRa_write_reg(0x08D8, temp, 1);
	HAL_Delay(10);

	tx_buf[0] = OP_SET_RX_TX_FALLBACK_MODE;
	tx_buf[1] = 0x40; //keep in FS mode so that we are ready to send msg ASAP
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 2, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	tx_buf[0] = OP_SET_STANDBY;
	tx_buf[1] = 1;
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 2, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_Delay(10); //THIS DELAY NEEDS TO BE MORE THAN STARTUP DELAY

	//specify LORA
	tx_buf[0] = OP_SET_PACKET_TYPE;
	tx_buf[1] = 0x01;
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 2, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_Delay(10);



	tx_buf[0] = OP_SET_RF_FREQUENCY ;
	uint32_t freq = 0x39300000; //915 mhz
	tx_buf[1] = freq>>24;
	tx_buf[2] = freq>>16;
	tx_buf[3] = freq>>8;
	tx_buf[4] = freq;
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 5, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_Delay(10);


	tx_buf[0] = OP_SET_PA_CONFIG ; //see datasheet for more info
	tx_buf[1] = 0x03; //paDutyCycle
	tx_buf[2] = 0x05; //hpMax
	tx_buf[3] = 0x00; //deviceSel
	tx_buf[4] = 0x01; //paLut always 0x01
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 5, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	tx_buf[0] = OP_SET_TX_PARAMS ;
	tx_buf[1] = 0x16; //set max power, as described by prev datasheet section
	tx_buf[2] = 0x04; //ramp time
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 3, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	tx_buf[0] = OP_SET_BUFFER_BASE_ADDRESS ;
	tx_buf[1] = 0x00;
	tx_buf[2] = 0x00;
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 3, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	tx_buf[0] = OP_SET_MODULATION_PARAMS ;
	tx_buf[1] = 0x05; //SF6
	tx_buf[2] = 0x06; //BW 500khz
	tx_buf[3] = 0x01; //CR 4/5
	tx_buf[4] = 0x00; //disable low data rate optimize
	tx_buf[5] = 0x00;
	tx_buf[6] = 0x00;
	tx_buf[7] = 0x00;
	tx_buf[8] = 0x00;
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 9, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	tx_buf[0] = OP_SET_PACKET_PARAMS ;
	tx_buf[1] = 0x00; //preamble len
	tx_buf[2] = 0x08; //preamble len
	tx_buf[3] = 0x01; //implicit
	tx_buf[4] = 17; //17 byte packet
	tx_buf[5] = 0x01; //CRC on
	tx_buf[6] = 0x00; ///standard IQ
	tx_buf[7] = 0x00;
	tx_buf[8] = 0x00;
	tx_buf[9] = 0x00;
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 10, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	tx_buf[0] = OP_SET_DIO_IRQ_PARAMS;
	tx_buf[1] = 0x00;
	tx_buf[2] = 0b00000011; //enamble TX done and RX done in IRQ
	tx_buf[3] = 0x00;//DIO 1
	tx_buf[4] = 0b00000001; //set TXdone to DIO1
	tx_buf[5] = 0x00; //DIO 2 (already used)
	tx_buf[6] = 0x00;
	tx_buf[7] = 0x00;//DIO 3 (already used)
	tx_buf[8] = 0x00;
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LoRa_hspi, tx_buf, 9, 10);
	HAL_GPIO_WritePin(SPI_NSS_LoRa_GPIO_Port, SPI_NSS_LoRa_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	LoRa_tx_isready=1;

}



