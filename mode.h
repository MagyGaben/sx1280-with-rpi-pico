#ifndef MODE_H
#define MODE_H

#include "SX1280.h"
#include <cstring>

#pragma once

void tx(SX1280 &LT, string message);
void rx(SX1280 &LT);
void cw(SX1280 &LT);

void lineGetter(string &input);

#endif 