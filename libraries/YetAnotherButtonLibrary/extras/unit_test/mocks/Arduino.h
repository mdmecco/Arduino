/*
 * =============================================================================
 * Yet Another Button Library (for Arduino)
 * https://github.com/yergin/Yabl
 * Copyright 2017 Gino Bollaert
 * MIT License (https://opensource.org/licenses/mit-license.php)
 * -----------------------------------------------------------------------------
 */

#pragma once

#include "gmock/gmock.h"

#define HIGH true
#define LOW false

class MockArduino
{
public:
  MOCK_METHOD0(millis, unsigned long());
};

unsigned long millis();

void CREATE_MOCK_ARDUINO();
void DESTROY_MOCK_ARDUINO();
MockArduino& ARDUINO();
