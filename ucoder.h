#ifndef MICRO_CODER_H
#define MICRO_CODER_H

void ucoderSetDefaultRNG(void);
void ucoderCreate(const char *privateKey, char *payload, char *jwt);
void ucoderCreate(uint8_t *privateKeyBytes, char *payload, char *jwt);
unsigned int ucoderCalculateSize(char *payload);

#endif
