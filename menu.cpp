#include <stdio.h>
#include "pico/stdlib.h"
#include <iostream>

#include "SX1280.h"
#include "Settings.h"
#include "SX1280_Def.h"
#include "mode.h"
#include "menu.h"

using namespace std;

uint menu(SX1280 &LT)
{
    uint mode = STANDBY;
    string input;
    printf("Type 'p' to setup power, 'f' to setup frequency, 'm' to switch mode, 'r' to reset device, 'i' for information, q to quit\n");
    while (input[0] != 'm' && input[0] != 'M')
    {
        watchdog_update();

        lineGetter(input);

        switch (input[0])
        {
        case 'p':
            int power;

            printf("Enter power in dBm (e.g. 7 for 7 dBm): ");
            lineGetter(input);

            if (input[0] == 'q')
            {
                return STANDBY;
            }
            else
            {
                power = atoi(input.c_str());
            }
            if ((power < -18) || (power > 13))
            {
                printf("Invalid input, defaulting to 0 dBm\n");
                power = 0;
            }

            watchdog_update();
            LT.setTxParams(RAMP_TIME, power);
            LT.resetDevice();
            sleep_ms(50);
            printf("\nPower set to %d dBm\n\n", LT.power);
            watchdog_update();
            return STANDBY;
        case 'f':
            uint32_t freq;

            printf("Enter frequency in kHz (e.g. 2267500 for 2.2675 GHz): ");
            lineGetter(input);
            if (input[0] == 'q')
            {
                return STANDBY;
            }
            else
            {
                freq = atoll(input.c_str());
                printf("Frequency set to %d kHz\n", freq);
            }
            if (freq < 2000000 || freq > 3000000)
            {
                printf("Invalid input, defaulting to 2.2675 GHz\n");
                freq = 2267500;
            }
            {
                watchdog_update();
                LT.setRfFrequency(freq * 1000);
                LT.resetDevice();
                sleep_ms(50);
                printf("Frequency set to %d kHz\n", freq);
                watchdog_update();
                return STANDBY;
                break;
            }
        case 'm':
            break;
        case 'r':
            LT.resetDevice();
            return STANDBY;
        case 'i':
            printf("Device Information:\n");
            printf(" - Status: %02X\n", LT.getStatus());
            printf(" - Frequency: %d kHz\n", LT.frequency / 1000);
            printf(" - Power: %d dBm\n", LT.power);
            input = "";
            printf("Type 'p' to setup power, 'f' to setup frequency, 'm' to switch mode, 'r' to reset device, 'i' for information, q to quit\n");
            break;
        default:
            input = "";
        }
    }
    printf("Enter mode (transmit, receive, continuous_wave, standby):\n");

    lineGetter(input);

    if (input == "tx" or input == "transmit")
    {
        mode = TRANSMIT;
        LT.resetDevice(); // if not reset, the sx1280 will be stuck in standby and won't transmit on the second try
        if (tx_menu(LT))
            printf("Ready to communicate!\n");
        sleep_ms(50);
        LT.writeChar('H', 1);
        LT.writeChar('A', 2);
        LT.writeChar('6', 3);
        LT.writeChar('G', 4);
        LT.writeChar('B', 5);
        LT.writeChar('N', 6);

        LT.startWriteSXBuffer(0);
        LT.writeSXBuffer(6);
        LT.endWriteSXBuffer();
        LT.transmitSXBuffer(0, 6, 0);
        sleep_ms(50);
        return TRANSMIT;
    }
    else if (input == "rx" or input == "receive")
    {
        mode = RECEIVE;
        LT.resetDevice(); // if not reset, the sx1280 will be stuck in standby and won't transmit on the second try
        LT.setupGFSK();
        printf("Ready to communicate!\n");
        sleep_ms(50);
        return RECEIVE;
    }
    else if (input == "cw" or input == "continuous_wave")
    {
        mode = CONTINUOUS_WAVE;
        LT.resetDevice(); // if not reset, the sx1280 will be stuck in standby and won't transmit on the second try
        LT.setupCW();
        printf("Transmitting continuous wave...\n");
        sleep_ms(50);
        return CONTINUOUS_WAVE;
    }
    else if (input == "s" or input == "standby")
    {
        mode = STANDBY;
        printf("Entering standby mode...\n");
        LT.setMode(RADIO_SET_STANDBY, MODE_STDBY_XOSC);
        sleep_ms(50);
    }
    else if (input == "reset")
    {
        mode = RESET;
        printf("Resetting device...\n");
        LT.resetDevice();
        sleep_ms(50);
    }
    else
    {
        printf("Invalid input, defaulting to standby mode\n");
        mode = STANDBY;
        printf("Entering standby mode...\n");
        LT.setMode(RADIO_SET_STANDBY, MODE_STDBY_XOSC);
        sleep_ms(50);
    }
    gpio_put(LED1, 0);
    return mode;
}


bool tx_menu(SX1280 &LT)
{
    string input;
    printf("Setting up for transmit mode...\n");
    printf("Choose modulation type \n\t%d GFSK\n\t%d LoRa\n\t%d FLRC\n\t%d BLE\n", GFSK, LoRa, FLRC, BLE);

    lineGetter(input);

    if (atoi(input.c_str()) < GFSK || atoi(input.c_str()) > BLE)
    {
        printf("Invalid modulation type: %d, defaulting to GFSK\n", atoi(input.c_str()));
        LT.setupGFSK();
        return true;
    }
    LT.connectmode = atoi(input.c_str());
    printf("\nSetting up modulation parameters... if default, type 0\n");

    lineGetter(input);
    if (atoi(input.c_str()) == 0)
    {
        watchdog_update();
        if (LT.connectmode == GFSK)
            LT.setupGFSK();
        else if (LT.connectmode == LoRa)
            LT.setupLoRa();
        else if (LT.connectmode == FLRC)
            LT.setupFLRC();
        else if (LT.connectmode == BLE)
            LT.setupBLE();
        return true;
    }
    else
    {
        watchdog_update();
        if (LT.connectmode == GFSK)
        {
            return GFSKmenu(LT);
        }
        else if (LT.connectmode == LoRa)
        {
            return LoRamenu(LT);
        }
        else if (LT.connectmode == FLRC)
        {
            return FLRCmenu(LT);
        }
        else if (LT.connectmode == BLE)
        {
            return BLEmenu(LT);
        }
        else
        {
            printf("Invalid modulation type: %d, defaulting to GFSK\n", LT.connectmode);
            LT.setupGFSK();
            return true;
        }
    }
    return false;
}

bool rx_menu(SX1280 &LT)
{
    string input;
    printf("\nSetting up for receive mode...\n");
    printf("Choose modulation type \n\t%d GFSK\n\t%d LoRa\n\t%d FLRC\n\t%d BLE\n", GFSK, LoRa, FLRC, BLE);

    lineGetter(input);

    if (atoi(input.c_str()) < GFSK || atoi(input.c_str()) > BLE)
    {
        printf("Invalid modulation type: %d, defaulting to GFSK\n", atoi(input.c_str()));
        LT.setupGFSK();
        return true;
    }
    LT.connectmode = atoi(input.c_str());

    watchdog_update();

    if (LT.connectmode == FLRC)
    {
        LT.setupFLRC();
    }
    else if (LT.connectmode == LoRa)
    {
        LT.setupLoRa();
    }
    else if (LT.connectmode == BLE)
    {
        LT.setupBLE();
    }
    else
    {
        LT.setupGFSK();
    }
    return true;
}

bool LoRamenu(SX1280 &LT)
{
    uint8_t spreading_factor, bandwidth, code_rate, preamble_mant, preamble_exp, packet_type, payload_length, crc, iq_swap;
    string input;

    printf("\nChoose LoRa spreading factor:\n");
    printf("0: SF5\n1: SF6\n2: SF7\n3: SF8\n4: SF9\n5: SF10\n6: SF11\n7: SF12\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        spreading_factor = LORA_SF5;
        break;
    case 1:
        spreading_factor = LORA_SF6;
        break;
    case 2:
        spreading_factor = LORA_SF7;
        break;
    case 3:
        spreading_factor = LORA_SF8;
        break;
    case 4:
        spreading_factor = LORA_SF9;
        break;
    case 5:
        spreading_factor = LORA_SF10;
        break;
    case 6:
        spreading_factor = LORA_SF11;
        break;
    case 7:
        spreading_factor = LORA_SF12;
        break;
    default:
        spreading_factor = LORA_SF7;
        break;
    }

    watchdog_update();
    printf("\nChoose LoRa bandwidth:\n");
    printf("0: 200 kHz\n1: 400 kHz\n2: 800 kHz\n3: 1600 kHz\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        bandwidth = LORA_BW_0200;
        break;
    case 1:
        bandwidth = LORA_BW_0400;
        break;
    case 2:
        bandwidth = LORA_BW_0800;
        break;
    case 3:
        bandwidth = LORA_BW_1600;
        break;
    default:
        bandwidth = LORA_BW_0800;
        break;
    }

    watchdog_update();
    printf("\nChoose LoRa code rate:\n");
    printf("0: 4/5\n1: 4/6\n2: 4/7\n3: 4/8\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        code_rate = LORA_CR_4_5;
        break;
    case 1:
        code_rate = LORA_CR_4_6;
        break;
    case 2:
        code_rate = LORA_CR_4_7;
        break;
    case 3:
        code_rate = LORA_CR_4_8;
        break;
    default:
        code_rate = LORA_CR_4_8;
        break;
    }

    watchdog_update();

    printf("\nChoose preamble legth\n");
    printf("LoRa preamble length is calculated this way: m*2^n\n");
    printf("m:\n");
    lineGetter(input);
    preamble_mant = atoi(input.c_str());
    printf("n:\n");
    lineGetter(input);
    preamble_exp = atoi(input.c_str());

    watchdog_update();
    printf("\nChoose LoRa packet type:\n");
    printf("0: Explicit header\n1: Implicit header\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        packet_type = LORA_PACKET_EXPLICIT;
        break;
    case 1:
        packet_type = LORA_PACKET_IMPLICIT;
        break;
    default:
        packet_type = LORA_PACKET_EXPLICIT;
        break;
    }

    watchdog_update();
    printf("\nEnter payload length (1-255, default 255): ");
    lineGetter(input);
    payload_length = atoi(input.c_str());
    if (payload_length == 0 || payload_length > 255)
    {
        payload_length = 255;
    }

    watchdog_update();
    printf("\nChoose CRC setting:\n");
    printf("0: CRC off\n1: CRC on\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        crc = LORA_CRC_OFF;
        break;
    case 1:
        crc = LORA_CRC_ON;
        break;
    default:
        crc = LORA_CRC_ON;
        break;
    }

    watchdog_update();
    printf("\nChoose IQ setting:\n");
    printf("0: Normal\n1: Inverted\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        iq_swap = LORA_IQ_NORMAL;
        break;      
    case 1:
        iq_swap = LORA_IQ_INVERTED;
        break;
    default:
        iq_swap = LORA_IQ_NORMAL;
        break;
    }

    watchdog_update();
    LT.setupLoRa(spreading_factor, bandwidth, code_rate, preamble_mant, preamble_exp, payload_length, packet_type, crc, iq_swap);
    return true;
}

bool GFSKmenu(SX1280 &LT)
{

    uint8_t bitrate_bandwidth, modulation_index, bt_filter, preamble_length, sync_word_length, packet_type, payload_length;
    uint8_t crc_type, whitening;
    string input;

    printf("\nChoose bit rate and bandwidth:\n");
    printf("0: GFSK BLE 2Mbs, 1.2MHz BW\n1: GFSK BLE 1Mbs, 1.2MHz BW\n2: GFSK BLE 500kbps, 1.2MHz BW\n");
    printf("3: GFSK BLE 500kbps, 600kHz BW\n4: GFSK BLE 800kbps, 1.2MHz BW\n5: GFSK BLE 800kbps, 2.4MHz BW\n");
    printf("6: GFSK BLE 1Mbps, 2.4MHz BW\n7: GFSK BLE 1.6Mbps, 2.4MHz BW\n8: GFSK BLE 2Mbps, 2.4MHz BW\n");

    lineGetter(input);

    switch (atoi(input.c_str()))
    {
    case 0:
        bitrate_bandwidth = GFS_BLE_BR_2_000_BW_2_4;
        break;
    case 1:
        bitrate_bandwidth = GFS_BLE_BR_1_000_BW_1_2;
        break;
    case 2:
        bitrate_bandwidth = GFS_BLE_BR_0_500_BW_1_2;
        break;
    case 3:
        bitrate_bandwidth = GFS_BLE_BR_0_500_BW_0_6;
        break;
    case 4:
        bitrate_bandwidth = GFS_BLE_BR_0_800_BW_1_2;
        break;
    case 5:
        bitrate_bandwidth = GFS_BLE_BR_0_800_BW_2_4;
        break;
    case 6:
        bitrate_bandwidth = GFS_BLE_BR_1_000_BW_2_4;
        break;
    case 7:
        bitrate_bandwidth = GFS_BLE_BR_1_600_BW_2_4;
        break;
    case 8:
        bitrate_bandwidth = GFS_BLE_BR_2_000_BW_2_4;
        break;
    default:
        bitrate_bandwidth = GFS_BLE_BR_1_000_BW_1_2;
        break;
    }

    watchdog_update();
    printf("\nChoose modulation index:\n");
    printf("0: 0.35\n1: 0.5\n2: 0.75\n3: 1.0\n4: 1.25\n5: 1.5\n");
    printf("6: 1.75\n7: 2.0\n8: 2.25\n9: 2.5\n10: 2.75\n11: 3.0\n12: 3.25\n13: 3.5\n14: 3.75\n15: 4.0\n");

    lineGetter(input);

    switch (atoi(input.c_str()))
    {
    case 0:
        modulation_index = GFS_BLE_MOD_IND_0_35;
        break;
    case 1:
        modulation_index = GFS_BLE_MOD_IND_0_50;
        break;
    case 2:
        modulation_index = GFS_BLE_MOD_IND_0_75;
        break;
    case 3:
        modulation_index = GFS_BLE_MOD_IND_1_00;
        break;
    case 4:
        modulation_index = GFS_BLE_MOD_IND_1_25;
        break;
    case 5:
        modulation_index = GFS_BLE_MOD_IND_1_50;
        break;
    case 6:
        modulation_index = GFS_BLE_MOD_IND_1_75;
        break;
    case 7:
        modulation_index = GFS_BLE_MOD_IND_2_00;
        break;
    case 8:
        modulation_index = GFS_BLE_MOD_IND_2_25;
        break;
    case 9:
        modulation_index = GFS_BLE_MOD_IND_2_50;
        break;
    case 10:
        modulation_index = GFS_BLE_MOD_IND_2_75;
        break;
    case 11:
        modulation_index = GFS_BLE_MOD_IND_3_00;
        break;
    case 12:
        modulation_index = GFS_BLE_MOD_IND_3_25;
        break;
    case 13:
        modulation_index = GFS_BLE_MOD_IND_3_50;
        break;
    case 14:
        modulation_index = GFS_BLE_MOD_IND_3_75;
        break;
    case 15:
        modulation_index = GFS_BLE_MOD_IND_4_00;
        break;
    default:
        modulation_index = GFS_BLE_MOD_IND_0_50;
        break;
    }

    watchdog_update();
    printf("\nChoose Gaussian filter BT:\n0: off\n1: 1.0\n2: 0.5\n");

    lineGetter(input);

    switch (atoi(input.c_str()))
    {
    case 0:
        bt_filter = BT_OFF;
        break;
    case 1:
        bt_filter = BT_1_0;
        break;
    case 2:
        bt_filter = BT_0_5;
        break;
    default:
        bt_filter = BT_OFF;
        break;
    }

    watchdog_update();
    printf("\nChoose preamble length:\n0: 4 bits\n1: 8 bits\n2: 12 bits\n3: 16 bits\n4: 20 bits\n5: 24 bits\n6: 28 bits\n7: 32 bits\n");

    lineGetter(input);

    switch (atoi(input.c_str()))
    {
    case 0:
        preamble_length = PREAMBLE_LENGTH_04_BITS;
        break;
    case 1:
        preamble_length = PREAMBLE_LENGTH_08_BITS;
        break;
    case 2:
        preamble_length = PREAMBLE_LENGTH_12_BITS;
        break;
    case 3:
        preamble_length = PREAMBLE_LENGTH_16_BITS;
        break;
    case 4:
        preamble_length = PREAMBLE_LENGTH_20_BITS;
        break;
    case 5:
        preamble_length = PREAMBLE_LENGTH_24_BITS;
        break;
    case 6:
        preamble_length = PREAMBLE_LENGTH_28_BITS;
        break;
    case 7:
        preamble_length = PREAMBLE_LENGTH_32_BITS;
        break;
    default:
        preamble_length = PREAMBLE_LENGTH_16_BITS;
        break;
    }

    watchdog_update();
    printf("\nChoose sync word length:\n1: 1 byte\n2: 2 bytes\n3: 3 bytes\n4: 4 bytes\n5: 5 bytes\n");

    lineGetter(input);

    switch (atoi(input.c_str()))
    {
    case 1:
        sync_word_length = SYNC_WORD_LEN_1_B;
        break;
    case 2:
        sync_word_length = SYNC_WORD_LEN_2_B;
        break;
    case 3:
        sync_word_length = SYNC_WORD_LEN_3_B;
        break;
    case 4:
        sync_word_length = SYNC_WORD_LEN_4_B;
        break;
    case 5:
        sync_word_length = SYNC_WORD_LEN_5_B;
        break;
    default:
        sync_word_length = SYNC_WORD_LEN_1_B;
        break;
    }

    packet_type = RADIO_PACKET_FIXED_LENGTH;
    watchdog_update();
    payload_length = 255;

    printf("\nChoose CRC type:\n0: off\n1: 1 byte\n2: 2 bytes\n3: 3 bytes\n");
    watchdog_update();

    lineGetter(input);

    switch (atoi(input.c_str()))
    {
    case 0:
        crc_type = RADIO_CRC_OFF;
        break;
    case 1:
        crc_type = RADIO_CRC_1_BYTES;
        break;
    case 2:
        crc_type = RADIO_CRC_2_BYTES;
        break;
    case 3:
        crc_type = RADIO_CRC_3_BYTES;
        break;
    default:
        crc_type = RADIO_CRC_2_BYTES;
        break;
    }

    watchdog_update();
    printf("\nChoose whitening:\n0: off\n1: on\n");

    lineGetter(input);

    switch (atoi(input.c_str()))
    {
    case 0:
        whitening = WHITENING_DISABLE;
        break;
    case 1:
        whitening = WHITENING_ENABLE;
        break;
    default:
        whitening = WHITENING_DISABLE;
        break;
    }

    watchdog_update();

    LT.setupGFSK(bitrate_bandwidth, modulation_index, bt_filter, preamble_length, sync_word_length, packet_type, payload_length, crc_type, whitening);
    return true;
}

bool FLRCmenu(SX1280 &LT)
{
    uint8_t bitrate_bandwidth, coding_rate, bt_filter, preamble_length, sync_word_length, sync_word_match, packet_type, payload_length, crc_type, whitening;
    string input;

    printf("\nChoose FLRC bit rate and bandwidth:\n");
    printf("0: 1.3Mbps, 1.2MHz BW\n1: 1.0Mbps, 1.2MHz BW\n2: 0.65Mbps, 0.6MHz BW\n3: 0.52Mbps, 0.6MHz BW\n4: 0.325Mbps, 0.3MHz BW\n5: 0.26Mbps, 0.3MHz BW\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        bitrate_bandwidth = FLRC_BR_1_300_BW_1_2;
        break;
    case 1:
        bitrate_bandwidth = FLRC_BR_1_000_BW_1_2;
        break;
    case 2:
        bitrate_bandwidth = FLRC_BR_0_650_BW_0_6;
        break;
    case 3:
        bitrate_bandwidth = FLRC_BR_0_520_BW_0_6;
        break;
    case 4:
        bitrate_bandwidth = FLRC_BR_0_325_BW_0_3;
        break;
    case 5:
        bitrate_bandwidth = FLRC_BR_0_260_BW_0_3;
        break;
    default:
        bitrate_bandwidth = FLRC_BR_1_000_BW_1_2;
        break;
    }

    watchdog_update();
    printf("\nChoose FLRC coding rate:\n");
    printf("0: 1/2\n1: 3/4\n2: 1/1\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        coding_rate = FLRC_CR_1_2;
        break;
    case 1:
        coding_rate = FLRC_CR_3_4;
        break;
    case 2:
        coding_rate = FLRC_CR_1_0;
        break;
    default:
        coding_rate = FLRC_CR_1_2;
        break;
    }

    watchdog_update();
    printf("\nChoose Gaussian filter BT:\n0: off\n1: 1.0\n2: 0.5\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        bt_filter = BT_DIS;
        break;
    case 1:
        bt_filter = BT_1;
        break;
    case 2:
        bt_filter = BT_0_5;
        break;
    default:
        bt_filter = BT_1;
        break;
    }

    watchdog_update();
    printf("\nChoose preamble length:\n");
    printf("0: 4 bits\n1: 8 bits\n2: 12 bits\n3: 16 bits\n4: 20 bits\n5: 24 bits\n6: 28 bits\n7: 32 bits\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        preamble_length = PREAMBLE_LENGTH_04_BITS;
        break;
    case 1:
        preamble_length = PREAMBLE_LENGTH_08_BITS;
        break;
    case 2:
        preamble_length = PREAMBLE_LENGTH_12_BITS;
        break;
    case 3:
        preamble_length = PREAMBLE_LENGTH_16_BITS;
        break;
    case 4:
        preamble_length = PREAMBLE_LENGTH_20_BITS;
        break;
    case 5:
        preamble_length = PREAMBLE_LENGTH_24_BITS;
        break;
    case 6:
        preamble_length = PREAMBLE_LENGTH_28_BITS;
        break;
    case 7:
        preamble_length = PREAMBLE_LENGTH_32_BITS;
        break;
    default:
        preamble_length = PREAMBLE_LENGTH_12_BITS;
        break;
    }

    watchdog_update();
    printf("\nChoose FLRC sync word length:\n");
    printf("0: no sync word\n1: 32-bit sync word\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        sync_word_length = FLRC_SYNC_NOSYNC;
        break;
    case 1:
        sync_word_length = FLRC_SYNC_WORD_LEN_P32S;
        break;
    default:
        sync_word_length = FLRC_SYNC_NOSYNC;
        break;
    }

    watchdog_update();
    printf("\nChoose FLRC sync word match type:\n");
    printf("0: no match\n1: match 1\n2: match 2\n3: match 1+2\n4: match 3\n5: match 1+3\n6: match 2+3\n7: match 1+2+3\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        sync_word_match = RX_DISABLE_SYNC_WORD;
        break;
    case 1:
        sync_word_match = RX_MATCH_SYNC_WORD_1;
        break;
    case 2:
        sync_word_match = RX_MATCH_SYNC_WORD_2;
        break;
    case 3:
        sync_word_match = RX_MATCH_SYNC_WORD_1_2;
        break;
    case 4:
        sync_word_match = RX_MATCH_SYNC_WORD_3;
        break;
    case 5:
        sync_word_match = RX_MATCH_SYNC_WORD_1_3;
        break;
    case 6:
        sync_word_match = RX_MATCH_SYNC_WORD_2_3;
        break;
    case 7:
        sync_word_match = RX_MATCH_SYNC_WORD_1_2_3;
        break;
    default:
        sync_word_match = RX_DISABLE_SYNC_WORD;
        break;
    }

    watchdog_update();
    printf("\nChoose FLRC packet type:\n");
    printf("0: Variable length\n1: Fixed length\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        packet_type = RADIO_PACKET_VARIABLE_LENGTH;
        break;
    case 1:
        packet_type = RADIO_PACKET_FIXED_LENGTH;
        break;
    default:
        packet_type = RADIO_PACKET_VARIABLE_LENGTH;
        break;
    }

    watchdog_update();
    printf("\nEnter payload length (1-255, default 127): ");
    lineGetter(input);
    payload_length = atoi(input.c_str());
    if (payload_length == 0 || payload_length > 255)
    {
        payload_length = 127;
    }

    watchdog_update();
    printf("\nChoose CRC type:\n");
    printf("0: off\n1: 1 byte\n2: 2 bytes\n3: 3 bytes\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        crc_type = RADIO_CRC_OFF;
        break;
    case 1:
        crc_type = RADIO_CRC_1_BYTES;
        break;
    case 2:
        crc_type = RADIO_CRC_2_BYTES;
        break;
    case 3:
        crc_type = RADIO_CRC_3_BYTES;
        break;
    default:
        crc_type = RADIO_CRC_2_BYTES;
        break;
    }

    watchdog_update();
    printf("\nChoose whitening:\n0: off\n1: on\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        whitening = WHITENING_DISABLE;
        break;
    case 1:
        whitening = WHITENING_ENABLE;
        break;
    default:
        whitening = WHITENING_DISABLE;
        break;
    }

    watchdog_update();
    LT.setupFLRC(bitrate_bandwidth, coding_rate, bt_filter, preamble_length, sync_word_length, sync_word_match, packet_type, payload_length, crc_type, whitening);
    return true;
}

bool BLEmenu(SX1280 &LT)
{
    uint8_t bitrate_bandwidth, modulation_index, bt_filter, connection_state, crc_type, payload, whitening;
    string input;

    printf("\nChoose BLE bit rate and bandwidth:\n");
    printf("0: 2.0Mbps, 2.4MHz BW\n1: 1.6Mbps, 2.4MHz BW\n2: 1.0Mbps, 2.4MHz BW\n3: 1.0Mbps, 1.2MHz BW\n4: 0.8Mbps, 2.4MHz BW\n5: 0.8Mbps, 1.2MHz BW\n6: 0.5Mbps, 1.2MHz BW\n7: 0.5Mbps, 0.6MHz BW\n8: 0.4Mbps, 1.2MHz BW\n9: 0.4Mbps, 0.6MHz BW\n10: 0.25Mbps, 0.6MHz BW\n11: 0.125Mbps, 0.3MHz BW\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        bitrate_bandwidth = GFS_BLE_BR_2_000_BW_2_4;
        break;
    case 1:
        bitrate_bandwidth = GFS_BLE_BR_1_600_BW_2_4;
        break;
    case 2:
        bitrate_bandwidth = GFS_BLE_BR_1_000_BW_2_4;
        break;
    case 3:
        bitrate_bandwidth = GFS_BLE_BR_1_000_BW_1_2;
        break;
    case 4:
        bitrate_bandwidth = GFS_BLE_BR_0_800_BW_2_4;
        break;
    case 5:
        bitrate_bandwidth = GFS_BLE_BR_0_800_BW_1_2;
        break;
    case 6:
        bitrate_bandwidth = GFS_BLE_BR_0_500_BW_1_2;
        break;
    case 7:
        bitrate_bandwidth = GFS_BLE_BR_0_500_BW_0_6;
        break;
    case 8:
        bitrate_bandwidth = GFS_BLE_BR_0_400_BW_1_2;
        break;
    case 9:
        bitrate_bandwidth = GFS_BLE_BR_0_400_BW_0_6;
        break;
    case 10:
        bitrate_bandwidth = GFS_BLE_BR_0_250_BW_0_6;
        break;
    case 11:
        bitrate_bandwidth = GFS_BLE_BR_0_125_BW_0_3;
        break;
    default:
        bitrate_bandwidth = GFS_BLE_BR_1_000_BW_1_2;
        break;
    }

    watchdog_update();
    printf("\nChoose modulation index:\n");
    printf("0: 0.35\n1: 0.5\n2: 0.75\n3: 1.0\n4: 1.25\n5: 1.5\n6: 1.75\n7: 2.0\n8: 2.25\n9: 2.5\n10: 2.75\n11: 3.0\n12: 3.25\n13: 3.5\n14: 3.75\n15: 4.0\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        modulation_index = GFS_BLE_MOD_IND_0_35;
        break;
    case 1:
        modulation_index = GFS_BLE_MOD_IND_0_50;
        break;
    case 2:
        modulation_index = GFS_BLE_MOD_IND_0_75;
        break;
    case 3:
        modulation_index = GFS_BLE_MOD_IND_1_00;
        break;
    case 4:
        modulation_index = GFS_BLE_MOD_IND_1_25;
        break;
    case 5:
        modulation_index = GFS_BLE_MOD_IND_1_50;
        break;
    case 6:
        modulation_index = GFS_BLE_MOD_IND_1_75;
        break;
    case 7:
        modulation_index = GFS_BLE_MOD_IND_2_00;
        break;
    case 8:
        modulation_index = GFS_BLE_MOD_IND_2_25;
        break;
    case 9:
        modulation_index = GFS_BLE_MOD_IND_2_50;
        break;
    case 10:
        modulation_index = GFS_BLE_MOD_IND_2_75;
        break;
    case 11:
        modulation_index = GFS_BLE_MOD_IND_3_00;
        break;
    case 12:
        modulation_index = GFS_BLE_MOD_IND_3_25;
        break;
    case 13:
        modulation_index = GFS_BLE_MOD_IND_3_50;
        break;
    case 14:
        modulation_index = GFS_BLE_MOD_IND_3_75;
        break;
    case 15:
        modulation_index = GFS_BLE_MOD_IND_4_00;
        break;
    default:
        modulation_index = GFS_BLE_MOD_IND_0_50;
        break;
    }

    watchdog_update();
    printf("\nChoose Gaussian filter BT:\n0: off\n1: 1.0\n2: 0.5\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        bt_filter = BT_OFF;
        break;
    case 1:
        bt_filter = BT_1;
        break;
    case 2:
        bt_filter = BT_0_5;
        break;
    default:
        bt_filter = BT_0_5;
        break;
    }

    watchdog_update();
    connection_state = BLE_TX_TEST_MODE;

    watchdog_update();
    printf("\nChoose CRC type:\n0: off\n1: 3 bytes\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        crc_type = BLE_CRC_OFF;
        break;
    case 1:
        crc_type = BLE_CRC_3B;
        break;
    default:
        crc_type = BLE_CRC_3B;
        break;
    }

    watchdog_update();
    printf("\nChoose BLE payload pattern:\n");
    printf("0: PRBS9\n1: Eye long 1.0\n2: Eye short 1.0\n3: PRBS15\n4: All 1\n5: All 0\n6: Eye long 0.1\n7: Eye short 0.1\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        payload = BLE_PRBS_9;
        break;
    case 1:
        payload = BLE_EYELONG_1_0;
        break;
    case 2:
        payload = BLE_EYESHORT_1_0;
        break;
    case 3:
        payload = BLE_PRBS_15;
        break;
    case 4:
        payload = BLE_ALL_1;
        break;
    case 5:
        payload = BLE_ALL_0;
        break;
    case 6:
        payload = BLE_EYELONG_0_1;
        break;
    case 7:
        payload = BLE_EYESHORT_0_1;
        break;
    default:
        payload = BLE_PRBS_9;
        break;
    }

    watchdog_update();
    printf("\nChoose whitening:\n0: off\n1: on\n");
    lineGetter(input);
    switch (atoi(input.c_str()))
    {
    case 0:
        whitening = BLE_WHITENING_DISABLE;
        break;
    case 1:
        whitening = BLE_WHITENING_ENABLE;
        break;
    default:
        whitening = BLE_WHITENING_DISABLE;
        break;
    }

    watchdog_update();
    LT.setupBLE(bitrate_bandwidth, modulation_index, bt_filter, connection_state, crc_type, payload, whitening);
    return true;
}