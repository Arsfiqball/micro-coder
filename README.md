# Micro Coder
Library to generate ES256 based JWT for microcontroller (Arduino, ESP etc.)

## Requirements
* This library works on Arduino framework
* You need to install [micro-ecc](https://github.com/kmackay/micro-ecc/tree/master) library

## Installation
This library is not yet available to install via package manager. You can download and unpack it manually, or use git submodule.

## Usage
This library exposes three functions:
* ucoderSetDefaultRNG(), this will configure RNG for uECC library
* ucoderCalculateSize(char *payload), calculate JWT size
* ucoderCreate(const char *privateKey, char *payload, char *jwt), to create JSON Web Token
  - privateKey format must be a ":" separated hex bytes char array (see example below)
  - payload can be any char array size
  - jwt is an output container for this function, its size must be determied via ucoderCalculateSize(payload),

Example:
```cpp
// Arduino.h is auto included in Arduino IDE
#include <Arduino.h>
#include "modules/micro-coder/ucoder.h"

const char privateKey[] = "6f:fa:ac:f3:16:88:89:2b:62:43:e3:52:7c:17:03:19:32:b5:00:31:47:cc:49:0c:63:ca:8b:a2:d9:1d:6c:eb";

void setup() {
  Serial.begin(9600);
  ucoderSetDefaultRNG();
}

void loop() {
  char payload[] = "{\"aud\":\"1234567890\"}";
  char jwt[ucoderCalculateSize(payload)];
  ucoderCreate(privateKey, payload, jwt);
  Serial.println(jwt);
  delay(10000);
}
```
