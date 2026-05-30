#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"

#include "SX1280.h"
#include "Settings.h"
#include "SX1280_Def.h"

#include "menu.h"
#include "mode.h"

int16_t rssi = 0;
FILE *txfile = NULL;
FILE *rxfile = NULL;

int main()
{
    stdio_init_all();
    sleep_ms(1500);
    printf("\n\n");
    SX1280 LT;
    string file2send;

    txfile = fopen("~/Documents/Docs/sx1280/txdata.txt", "r");
    if (txfile == NULL)
    {
        printf("Error opening txdata.txt\n");
        file2send = "Hello world! This is a very very long message to test the capabilities of the SX1280 in GFSK mode."; 
        file2send += " It should be long enough to fill the entire 255 byte payload and then some, so we can see how the radio handles it. ";
        file2send += " If you can read this, it means the transmission was successful and the radio is working properly. Congratulations!";

    }
    else
    {
        printf("txdata.txt opened successfully\n");
        char ch;
        while ((ch = fgetc(txfile)) != EOF){
            printf("%c", ch);
            file2send += ch;
        }
    }
    printf("File to send: %s\n", file2send.c_str());
    rxfile = fopen("~/Documents/Docs/sx1280/rxdata.txt", "w");
    if (rxfile == NULL)
    {
        printf("Error opening rxdata.txt\n");
    }

    if (LT.begin())
    {
        printf("Initialisation ended\n");
        printf("Status:\t");
    }
    else
    {
        while (1)
        {
            printf("failed in begin functions\n");
            sleep_ms(2000);
        }
    }

    printf("Test started!\n");
    uint mode = STANDBY;
    while (true)
    {
        watchdog_update();
        const char *mode_str;

        // Print current mode
        switch (mode)
        {
        case TRANSMIT:
            mode_str = "transmit";
            break;
        case RECEIVE:
            mode_str = "receive";
            break;
        case CONTINUOUS_WAVE:
            mode_str = "continuous wave";
            break;
        case STANDBY:
            mode_str = "standby";
            break;
        }
        printf("Current mode: %s\n", mode_str);

        // Execute mode-specific function
        switch (mode)
        {
        case TRANSMIT:
            tx(LT,file2send);
            break;
        case RECEIVE:
            rx(LT);
            break;
        case CONTINUOUS_WAVE:
            cw(LT);
            break;
        case STANDBY:
            LT.setMode(RADIO_SET_STANDBY, MODE_STDBY_XOSC);
            mode = menu(LT);
            break;
        default:
            printf("Invalid mode\n");
            printf("Entering standby mode...\n");
            LT.setMode(RADIO_SET_STANDBY, MODE_STDBY_XOSC);
            break;
        }

        // Check for user input to change mode
        int ch = getchar_timeout_us(0);
        if (ch != PICO_ERROR_TIMEOUT)
        {
            printf("Entering standby mode...\n");
            LT.setMode(RADIO_SET_STANDBY, MODE_STDBY_XOSC);
            mode = menu(LT);
            break;
        }
    }

    return 0;
}
