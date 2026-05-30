/*
  Copyright 2022 - Juaneda Axel
  Licensed under a MIT license displayed at the bottom of this document.

  This code was adapted from the library of Stuart Robinson https://github.com/StuartsProjects/SX12XX-LoRa
*/

#ifndef SX1280_H // include guard
#define SX1280_H

#include <stdio.h>
#include <iostream>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/clocks.h"
#include "hardware/watchdog.h"

#include "Settings.h"
#include "SX1280_Def.h"

using namespace std;

class SX1280
{
public:
    SX1280();
    ~SX1280();

    uint8_t connectmode = GFSK; // current radio mode selection: GFSK, FSK, LoRa, FLRC, BLE
    short power = TXpower; // current power setting for transmissions, default value is set to value of TXpower in Settings.h
    uint32_t frequency = Frequency; // current frequency setting, default value is set to value of Frequency in Settings.h

    void setupLoRa(uint8_t SpreadingFactor = LORA_SF7, 
                   uint8_t Bandwidth = LORA_BW_0800, 
                   uint8_t CodeRate = LORA_CR_4_8, 
                   uint8_t Preamble_MANT = 12, 
                   uint8_t Preamble_EXP = 0,
                   uint8_t Payload_Length = 255,
                   uint8_t PacketType = LORA_PACKET_IMPLICIT,
                   uint8_t CRC = LORA_CRC_ON,
                   uint8_t IQ_SWAP = LORA_IQ_NORMAL); 

    void setupGFSK(uint8_t bitrate_bandwidth = GFSK_BLE_BR_1_000_BW_1_2,
                   uint8_t modulation_index = MOD_IND_0_5,
                   uint8_t bt_filter = BT_OFF,
                   uint8_t preamble_length = PREAMBLE_LENGTH_16_BITS,
                   uint8_t sync_word_length = SYNC_WORD_LEN_1_B,
                   uint8_t packet_type = RADIO_PACKET_FIXED_LENGTH,
                   uint8_t payload_length = 255,
                   uint8_t crc_type = RADIO_CRC_2_BYTES,
                   uint8_t whitening = WHITENING_DISABLE); // default values are for GFSK BLE 1Mbs

    void setupBLE(uint8_t bitrate_bandwidth = GFS_BLE_BR_1_000_BW_1_2,
                  uint8_t modulation_index = MOD_IND_0_5,
                  uint8_t bt_filter = BT_0_5,
                  uint8_t connection_state = BLE_TX_TEST_MODE,
                  uint8_t crc_type = BLE_CRC_3B,
                  uint8_t payload = BLE_PRBS_9,
                  uint8_t whitening = BLE_WHITENING_DISABLE); // default values are for BLE 1Mbs

    void setupFLRC(uint8_t bitrate_bandwidth = FLRC_BR_1_000_BW_1_2,
                   uint8_t coding_rate = FLRC_CR_1_2,
                   uint8_t bt_filter = BT_1,
                    uint8_t preamble_length = PREAMBLE_LENGTH_12_BITS,
                    uint8_t sync_word_length = FLRC_SYNC_NOSYNC,
                    uint8_t sync_word_match = RX_DISABLE_SYNC_WORD,
                    uint8_t packet_type = RADIO_PACKET_VARIABLE_LENGTH,
                    uint8_t payload_length = 127,
                    uint8_t crc_type = RADIO_CRC_2_BYTES,
                    uint8_t whitening = WHITENING_DISABLE
                  ); // default FLRC setup
    void setupCW();                                        // send 19.9 sec long cw then update watchdog
    bool begin();                                          // returns true if sx1280 found and initialised, false if not found or failed to initialise

    void setMode(uint8_t modeconfig, uint8_t param);
    void setRegulatorMode(uint8_t mode);                                               // LDO or DC-DC
    void setPacketType(uint8_t packettype);                                            // GFSK, LoRa, FLRC or BLE
    void setRfFrequency(uint32_t frequency);                           // frequency in Hz, offset in Hz
    void setBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress);           // set base address for tx and rx buffer,
                                                                                       // default 0 and 0x80
    void setModulationParams(uint8_t modParam1, uint8_t modParam2, uint8_t modParam3); // modParam1, modParam2 and modParam3
                                                                                       // are modulation specific, see datasheet for details
    void setPacketParams(uint8_t packetParam1=0, uint8_t packetParam2=0, uint8_t packetParam3=0, uint8_t packetParam4=0,
                         uint8_t packetParam5=0, uint8_t packetParam6=0, uint8_t packetParam7=0); // packetParam1, packetParam2,
                                                                                            // packetParam3, packetParam4, packetParam5, packetParam6 and packetParam7 are modulation specific, see datasheet for details
    void setDioIrqParams(uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask);
    void setHighSensitivity(); // sets the sx1280 to high sensitivity mode
    void setTxParams(uint8_t RampTime, short txpower = TXpower); //default power is set to value of TXpower in Settings.h

    void resetDevice();

    uint8_t receiveSXBuffer(uint8_t startaddr, uint16_t timeout); // returns length of received packet,
                                                                  // reads packet to sx1280 buffer
    void startReadSXBuffer(uint8_t ptr);                          // set start address for reading received packet from sx1280 buffer
    uint8_t readBufferChar(char *rxbuffer);                     // read received packet from sx1280 buffer to char buffer, 

    int16_t readPacketRSSI();
    int8_t readPacketSNR();

    void readSXBuffer(uint8_t size);
    void getSXBuffer(uint8_t *destination, uint8_t size);

    uint8_t endReadSXBuffer();

    bool isTXReady();
    void startWriteSXBuffer(uint8_t ptr);
    void writeSXBuffer(uint8_t size);
    void endWriteSXBuffer();
    void transmitSXBuffer(uint8_t startaddr, uint8_t length, uint16_t timeout);

    bool checkIrqs();

    void writeUint8(uint8_t u8, uint8_t addr);
    uint8_t readUint8(uint8_t addr);

    void writeUint16(uint16_t u16, uint8_t startaddr);
    uint16_t readUint16(uint8_t startaddr);

    void writeInt8(int8_t i8, uint8_t addr);
    int8_t readInt8(uint8_t addr);

    void writeInt16(int16_t i16, uint8_t startaddr);
    int16_t readInt16(uint8_t startaddr);
    
    void writeString256(char* str);

    void writeFloat(float fl, uint8_t startaddr);
    float readFloat(uint8_t startaddr);

    void writeChar(char c, uint8_t addr);
    char readChar(uint8_t addr);

    uint8_t getStatus();
    void toggle_pin(uint OUTPUT_PIN);

#ifdef DEBUG
    uint8_t getTXBuffer(uint8_t addr);
#endif

private:
    void checkBusy();
    bool checkDevice();
    void SetStandby();

    void writeRegister(uint16_t address, uint8_t value);
    void readRegisters(uint16_t address, uint8_t *buffer, uint16_t size);
    uint8_t readRegister(uint16_t address);
    void writeRegisters(uint16_t address, uint8_t *buffer, uint16_t size);

    void readCommand(uint8_t Opcode, uint8_t *buffer, uint16_t size);
    void writeCommand(uint8_t Opcode, uint8_t *buffer, uint16_t size);

    void setTx(uint16_t timeout);
    void txEnable();

    void setRx(uint16_t timeout);
    void rxEnable();

    void clearIrqStatus(uint16_t irqMask);
    uint16_t readIrqStatus();

    uint8_t RXPacketL, periodBase = PERIODBASE_01_MS;
    uint8_t rxBuffer[RXBUFFER_SIZE], txBuffer[RXBUFFER_SIZE];
    bool rxtxpinmode;
    bool waitIrq = false;
};

#endif

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
