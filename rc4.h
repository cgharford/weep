// Copyright (c) 2012, UNC Chapel Hill. All rights reserved.
// Author: Kevin Z. Snow
//
// A header-only implementation of Alleged RC4
#ifndef _RC4_H
#define _RC4_H

#include <stdint.h>

typedef struct {
  uint8_t S[256], i, j;
} Rc4State;

class Rc4 {
 public:
  // Constructor
  Rc4() {}

  // Initialize RC4 key state from the given key
  void Init(const uint8_t key[], size_t keyLength, int nRounds) {
    int i;
    for(i = 0; i < 256; i++) key_.S[i] = i;
    key_.j = 0;
    for(i = 0; i < nRounds; i++) {
      key_.j = key_.j + key_.S[i] + key[i % keyLength];
      swap(&key_.S[i], &key_.S[key_.j]);
    }
    if (i != 256) return;
    key_.i = 0;
    key_.j = 0;
  }

  // Same as initialize, but always uses 256 rounds
  void Init(const uint8_t key[], size_t keyLength) {
    Init(key, keyLength, 256);
  }

  // Generates a single byte of the keystream
  uint8_t Generate() {
    key_.i++;
    key_.j += key_.S[key_.i];
    swap(&key_.S[key_.i], &key_.S[key_.j]);
    return key_.S[(key_.S[key_.i] + key_.S[key_.j]) % 256];
  }

  // Generates pseudo-random numbers from the current key-state,
  // XOR's each input byte with the random number
  void Apply(uint8_t data[], size_t dataLength) {
    for (size_t index = 0; index < dataLength; ++index) {
      data[index] = data[index] ^ Generate();
    }
  }

  void GetState(Rc4State* state) {
    *state = key_;
  }

  void GetInvertedState(Rc4State* state) {
    *state = key_;
    for (int i = 0; i < 256; i++) {
      state->S[key_.S[i]] = i;
    }
  }

 protected:
  void swap(uint8_t* x, uint8_t* y) {
    uint8_t temp;
    temp = *x;
    *x = *y;
    *y = temp;
  }

  Rc4State key_;
};
#endif
