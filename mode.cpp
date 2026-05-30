#include <stdio.h>
#include "pico/stdlib.h"
#include <iostream>

#include "SX1280.h"
#include "Settings.h"
#include "SX1280_Def.h"
#include "menu.h"

void tx(SX1280 &LT, string message)
{
    string message_slice;
    string tmp;
    uint64_t delta_time = to_us_since_boot(get_absolute_time());
    if (message == "")
    {
        printf("No message to send\n");
        return;
    }
    while (message.length() > 255)
    {
        watchdog_update();
        message_slice = message.substr(0, 255);
        printf("Message length: %d\n", message.length());

        message.erase(0, 255);
        tmp = message_slice.substr(0, 64);
        printf("Message to send:\n \t \t %s\n", tmp.c_str());
        tmp = message_slice.substr(64, 64);
        printf("\t \t %s\n", tmp.c_str());
        tmp = message_slice.substr(128, 64);
        printf("\t \t %s\n", tmp.c_str());
        tmp = message_slice.substr(192, 64);
        printf("\t \t %s\n", tmp.c_str());

        LT.writeString256((char *)message_slice.c_str());
        LT.startWriteSXBuffer(0);
        LT.writeSXBuffer(255);
        LT.endWriteSXBuffer();
        delta_time = to_us_since_boot(get_absolute_time()) - delta_time;
        printf("Time taken for initialization: %d ms\n", delta_time / 1000);
        delta_time = to_us_since_boot(get_absolute_time());

        if (LT.isTXReady())
        {
            printf("start...");
            gpio_put(25, 1);
            delta_time = to_us_since_boot(get_absolute_time()) - delta_time;
            printf("Time taken for preparation: %d ms\n", delta_time / 1000);
            delta_time = to_us_since_boot(get_absolute_time());
            LT.transmitSXBuffer(0, 255, 0); //setTx might be enough
            printf("finished\n");
            sleep_ms(5); // nerf the tx
            gpio_put(25, 0);
        }
        else
        {
            printf("\nTX not ready\n");
        }
        delta_time = to_us_since_boot(get_absolute_time()) - delta_time;
        printf("Time taken for transmission: %d ms\n", delta_time / 1000);
        delta_time = to_us_since_boot(get_absolute_time());
    }
    message.append(255 - message.size(), '#');

    tmp = message.substr(0, 64);
    printf("Message to send:\n \t \t %s\n", tmp.c_str());
    tmp = message.substr(64, 64);
    printf("\t \t %s\n", tmp.c_str());
    tmp = message.substr(128, 64);
    printf("\t \t %s\n", tmp.c_str());
    tmp = message.substr(192, 64);
    printf("\t \t %s\n", tmp.c_str());

    LT.writeString256((char *)message.c_str());
    LT.startWriteSXBuffer(0);
    LT.writeSXBuffer(255);
    LT.endWriteSXBuffer();

    if (LT.isTXReady())
    {
        printf("start...");
        gpio_put(25, 1);
        LT.transmitSXBuffer(0, 255, 0);
        printf("finished\n");
        sleep_ms(5); // nerf the tx
        gpio_put(25, 0);
    }
    else
    {
        printf("TX not ready\n");
    }
    delta_time = to_us_since_boot(get_absolute_time()) - delta_time;
    printf("Time taken for transmission: %d ms\n", delta_time / 1000);
}

void rx(SX1280 &LT)
{
    printf("receiving...\n");
    if (LT.checkIrqs())
    {
        LT.receiveSXBuffer(0, 0);

        LT.startReadSXBuffer(0);
        LT.readSXBuffer(8);
        LT.endReadSXBuffer(); // todo save data to file or something, for now just print it

        std::cout << "pressure: " << LT.readUint16(0) << " altitude: " << LT.readInt16(2) << " temperature: " << LT.readFloat(4) << std::endl;
    }
    else
    {
        printf("no data received\n");
    }
}

void cw(SX1280 &LT)
{
    LT.setupCW();
}

void lineGetter(string &input)
{
    input = "";
    char tmp_char;
    gpio_put(LED1, 1);
    tmp_char = getchar_timeout_us(0);
    watchdog_update();
    while (tmp_char != '\n' && tmp_char != '\r')
    {
        watchdog_update();
        if (tmp_char != PICO_ERROR_TIMEOUT && input.size() != 255)
        {
            input += (char)tmp_char;
            printf("%c", (char)tmp_char);
        }
        gpio_put(LED1, gpio_get(LED1) ^ 1);
        sleep_ms(100);
        tmp_char = getchar_timeout_us(100000);
    }
    printf("\n");
    gpio_put(LED1, 0);
}


