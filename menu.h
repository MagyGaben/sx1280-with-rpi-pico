#pragma once

enum Mode
{
    TRANSMIT,
    RECEIVE,
    CONTINUOUS_WAVE,
    STANDBY,
    RESET
};

uint menu(SX1280 &LT);
bool tx_menu(SX1280 &LT);
bool rx_menu(SX1280 &LT);

bool GFSKmenu(SX1280 &LT);
bool LoRamenu(SX1280 &LT);
bool FLRCmenu(SX1280 &LT);
bool BLEmenu(SX1280 &LT);