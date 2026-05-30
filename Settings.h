/*******************************************************************************************************
  Licensed under a MIT license displayed at the bottom of this document.
  Programs for raspberry pi pico

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards and using an ebyte module,
//be sure to change the definitions to match your own setup. Some pins such as DIO2,
//DIO3, BUZZER may not be in used by this sketch so they do not need to be
//connected and should be included and be set to -1.
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

#define SPI_PORT spi0

// Copyright of the author Stuart Robinson

#define NSS 17 //26
#define RFBUSY 9 //2
#define NRESET 10 //0
#define LED1 25//25
#define DIO1 8
#define DIO2 7                 //not used 
#define DIO3 6                 //not used
#define RX_EN -1    //27            //pin for RX enable, used on some SX1280 devices, set to -1 if not used
#define TX_EN -1               //pin for TX enable, used on some SX1280 devices, set to -1 if not used                        
#define BUZZER -1               //connect a buzzer here if wanted        

#define LORA_DEVICE DEVICE_SX1280               //we need to define the device we are using  

//LoRa Modem Parameters
#define Frequency 2267500000                     //frequency of transmissions
#define Offset 0                                 //offset frequency for calibration purposes

#define TXpower  -18                           //power for transmissions in dBm between -18 and +12

#define packet_delay 1000                        //mS delay between packets

#define RXBUFFER_SIZE 255                        //RX buffer size, not used in this program 

/*
  MIT license
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
  documentation files (the "Software"), to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all copies or substantial portions
  of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
  TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

/* --- Bitrate + Bandwidth (modParam1) --- */
#define GFSK_BLE_BR_2_000_BW_2_4   0x04
#define GFSK_BLE_BR_1_600_BW_2_4   0x28
#define GFSK_BLE_BR_1_000_BW_2_4   0x4C
#define GFSK_BLE_BR_1_000_BW_1_2   0x45
#define GFSK_BLE_BR_0_800_BW_2_4   0x70
#define GFSK_BLE_BR_0_800_BW_1_2   0x69
#define GFSK_BLE_BR_0_500_BW_1_2   0x8D
#define GFSK_BLE_BR_0_500_BW_0_6   0x86
#define GFSK_BLE_BR_0_400_BW_1_2   0xB1
#define GFSK_BLE_BR_0_400_BW_0_6   0xAA
#define GFSK_BLE_BR_0_250_BW_0_6   0xCE
#define GFSK_BLE_BR_0_250_BW_0_3   0xC7
#define GFSK_BLE_BR_0_125_BW_0_3   0xEF

/* --- Modulation Index (modParam2) --- */
#define MOD_IND_0_35   0x00
#define MOD_IND_0_5    0x01
#define MOD_IND_0_75   0x02
#define MOD_IND_1_00   0x03
#define MOD_IND_1_25   0x04
#define MOD_IND_1_50   0x05
#define MOD_IND_1_75   0x06
#define MOD_IND_2_00   0x07
#define MOD_IND_2_25   0x08
#define MOD_IND_2_50   0x09
#define MOD_IND_2_75   0x0A
#define MOD_IND_3_00   0x0B
#define MOD_IND_3_25   0x0C
#define MOD_IND_3_50   0x0D
#define MOD_IND_3_75   0x0E
#define MOD_IND_4_00   0x0F

/* --- Gaussian Filter BT (modParam3) --- */
#define BT_OFF   0x00
#define BT_1_0   0x10
#define BT_0_5   0x20


/* =========================
 * PACKET PARAMETERS
 * ========================= */

/* --- Preamble Length (packetParam1) --- */
#define PREAMBLE_LENGTH_04_BITS   0x00
#define PREAMBLE_LENGTH_08_BITS   0x10
#define PREAMBLE_LENGTH_12_BITS   0x20
#define PREAMBLE_LENGTH_16_BITS   0x30
#define PREAMBLE_LENGTH_20_BITS   0x40
#define PREAMBLE_LENGTH_24_BITS   0x50
#define PREAMBLE_LENGTH_28_BITS   0x60
#define PREAMBLE_LENGTH_32_BITS   0x70

/* --- Sync Word Length (packetParam2) --- */
#define SYNC_WORD_LEN_1_B   0x00
#define SYNC_WORD_LEN_2_B   0x02
#define SYNC_WORD_LEN_3_B   0x04
#define SYNC_WORD_LEN_4_B   0x06
#define SYNC_WORD_LEN_5_B   0x08

/* --- Sync Word Selection (packetParam3) --- */
#define RADIO_SELECT_SYNCWORD_OFF      0x00
#define RADIO_SELECT_SYNCWORD_1        0x10
#define RADIO_SELECT_SYNCWORD_2        0x20
#define RADIO_SELECT_SYNCWORD_1_2      0x30
#define RADIO_SELECT_SYNCWORD_3        0x40
#define RADIO_SELECT_SYNCWORD_1_3      0x50
#define RADIO_SELECT_SYNCWORD_2_3      0x60
#define RADIO_SELECT_SYNCWORD_1_2_3    0x70

/* --- Packet Type (packetParam4) --- */
#define RADIO_PACKET_FIXED_LENGTH      0x00
#define RADIO_PACKET_VARIABLE_LENGTH   0x20

/* --- Packet Type (packetParam6) --- */
#define PAYLOAD_LENGTH(x)   ((uint8_t)(x))

/* --- CRC (packetParam6) --- */
#define RADIO_CRC_OFF        0x00
#define RADIO_CRC_1_BYTES    0x10
#define RADIO_CRC_2_BYTES    0x20

/* --- Whitening (packetParam7) --- */
#define WHITENING_ENABLE     0x00
#define WHITENING_DISABLE    0x08

