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

class BounceInterface
{
public:
  virtual void attach(int pin) = 0;
  virtual void attach(int pin, int mode) = 0;
  virtual bool update() = 0;
  virtual bool read() = 0;
  virtual bool rose() = 0;
  virtual bool fell() = 0;
};

class Bounce : ::testing::NiceMock<BounceInterface>
{
public:
  MOCK_METHOD1(attach, void(int pin));
  MOCK_METHOD2(attach, void(int pin, int mode));
  MOCK_METHOD0(update, bool());
  MOCK_METHOD0(read, bool());
  MOCK_METHOD0(rose, bool());
  MOCK_METHOD0(fell, bool());
  
  unsigned long previous_millis = 0;
};
