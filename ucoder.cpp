#include "ucoder.h"

#include <Arduino.h>
#include <uECC.h>

#include "base64.h"
#include "sha256.h"

extern "C" {

static int RNG(uint8_t *dest, unsigned size) {
  while (size) {
    uint8_t val = 0;
    for (unsigned i = 0; i < 8; ++i) {
      int init = analogRead(0);
      int count = 0;
      while (analogRead(0) == init) {
        ++count;
      }

      if (count == 0) {
        val = (val << 1) | (init & 0x01);
      } else {
        val = (val << 1) | (count & 0x01);
      }
    }
    *dest = val;
    ++dest;
    --size;
  }

  return 1;
}

}  // extern "C"

const PROGMEM char *joseHeaderBase64 = "eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9";

void ucoderSetDefaultRNG() {
  // set default RNG function
  uECC_set_rng(&RNG);
}

unsigned int ucoderCalculateSize(char *payload) {
  // calculate JWT size
  return strlen(joseHeaderBase64) + encode_base64_length(strlen(payload)) + encode_base64_length(32) + 2;
}

void ucoderCreate(const char *privateKey, char *payload, char *jwt) {
  uint8_t privateKeyBytes[32];

  for (size_t i = 0, j = 0; i < strlen(privateKey), j < 32; i += 3, j++) {
    if (privateKey[i] >= 48 && privateKey[i] <= 57) {
      privateKeyBytes[j] = (privateKey[i] - 48) * 16;
    } else if (privateKey[i] >= 97 && privateKey[i] <= 102) {
      privateKeyBytes[j] = (privateKey[i] - 97 + 10) * 16;
    } else if (privateKey[i] >= 65 && privateKey[i] <= 70) {
      privateKeyBytes[j] = (privateKey[i] - 65 + 10) * 16;
    } else {
      privateKeyBytes[j] = 0;
    }

    if (privateKey[i + 1] >= 48 && privateKey[i + 1] <= 57) {
      privateKeyBytes[j] += (privateKey[i + 1] - 48);
    } else if (privateKey[i + 1] >= 97 && privateKey[i + 1] <= 102) {
      privateKeyBytes[j] += (privateKey[i + 1] - 97 + 10);
    } else if (privateKey[i + 1] >= 65 && privateKey[i + 1] <= 70) {
      privateKeyBytes[j] += (privateKey[i + 1] - 65 + 10);
    }
  }

  uint8_t *tokenPointer = (uint8_t *)jwt;
  memcpy(tokenPointer, joseHeaderBase64, strlen(joseHeaderBase64));
  tokenPointer += strlen(joseHeaderBase64);
  *tokenPointer++ = '.';

  encode_base64((uint8_t *)payload, strlen(payload), tokenPointer);
  tokenPointer += encode_base64_length(strlen(payload));

  while (*(tokenPointer - 1) == '=') tokenPointer--;
  *(tokenPointer) = 0;

  uint8_t *hash;

  Sha256.flush();
  Sha256.init();
  Sha256.print(jwt);
  hash = Sha256.result();

  *tokenPointer++ = '.';

  uint8_t signature[64];
  uECC_sign(privateKeyBytes, hash, HASH_LENGTH, signature, uECC_secp256r1());
  encode_base64(signature, 64, tokenPointer);
  tokenPointer += encode_base64_length(64);

  while (*(tokenPointer - 1) == '=') tokenPointer--;
  *(tokenPointer) = 0;
}
