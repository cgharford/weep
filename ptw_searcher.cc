// Copyright 2015, University of North Carolina
//
// This file contains the implementation of the PTWSearcher class' methods.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include "ptw_searcher.h"
#include <inttypes.h>
#include <sstream>


using std::queue;
using std::cout;
using std::endl;

// You can pass a pointer to this function as the third argument to std::sort.
bool SortByVotes(PTWEntry a, PTWEntry b) {
  return a.votes > b.votes;
}

const int numOfBytes = 5;
const int numOfColumns = 255; // total: 256. Can have interval of 2, 4, 8, 16
int columnInterval = 8;
vector<uint64_t> candidates_new; 
vector<uint64_t> candidates_tested; 
bool gotTheKey = false;
uint64_t correctKey = 0;

std::string PTWSearcher::Search() {
  // Do a search over the sorted key byte indices. It's
  // up to you how to represent and implement the search.
  for (PTWTable::iterator it = ptw_table_.begin(); it < ptw_table_.end(); ++it) {
    std::sort((*it).begin(), (*it).end(), SortByVotes);
  }
  
  cout << "Printing the table!!!!" << endl;
  // PrintPtwTable();
  for (PTWTable::iterator it = ptw_table_.begin(); it < ptw_table_.end(); ++it) {
    for (int i=0; i<8; ++i) {
      printf("%02X (%d)  ", (*it)[i].approximation, (*it)[i].votes);
    }
    printf("\n");
  }
  // (Your implementation goes here)

  // For ( n = 1 - > 256) {
  //   ​1. Take the first n columns and use N-Queen Algorithm to generate all possible combinations.
  //   ​2. Eliminate the combinations that have been tested before.
  //   3. Test the new combination.
  //   4. Record the tested combination
  // }    

  for (int range = 0; range <= numOfColumns; range += columnInterval) // Max range of column to 256   It could increment by 5?
  {
    if (!gotTheKey)
    {
      printf("Looking at total of %i columns\n", range);
      candidates_new.clear();
      generateCandidates(0, range);
      printf("Generated new candidates #%li\n", candidates_new.size());
      // removeTestedCandidates();
      testNewCandidates();
      // recordTestedCandidates();
    }
  }
  if (!gotTheKey)
  {
    return "";
  } else {
    std::ostringstream os;
    os << std::hex << correctKey;
    std::string key = os.str(); 
    // printf("The correct key is: %s\n", key.c_str());
    printf("The correct key is: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", getByteFromRange(32, 39, correctKey), getByteFromRange(24, 31, correctKey), getByteFromRange(16, 23, correctKey), getByteFromRange(8, 15, correctKey), getByteFromRange(0, 7, correctKey));
    return key;
  }
}

uint64_t position[numOfBytes];
// Recursively generate a tuple like [0 0 0 0], then [0 0 0 1] then etc...
void PTWSearcher::generateCandidates(int k, int range) {
  if (k == numOfBytes) 
  {
    uint64_t item = 0;
    for (int i = 0; i < numOfBytes; i++) {
      uint64_t key_adjusted = 0;
      if (i==0)
      {
        key_adjusted = position[i];
      } else {
        key_adjusted = (position[i] - position[i-1]) % 256;
      }
      item |= (key_adjusted << 8*(numOfBytes-i-1));
    }
    // printf("Solution: 0x%10lx\n", item);
    candidates_new.push_back(item);
  }
  else
  {
    for (int i = 0; i < range; i++) // Generate ALL combinations
    {
      // position[k] = i;
      position[k] = ptw_table_[k][i].approximation;
      // Place another queen
      generateCandidates(k + 1, range);
    }
  }
}

void PTWSearcher::testNewCandidates() {
  for (uint64_t index = 0; index < candidates_new.size(); index++) {
    uint64_t item = candidates_new[index];
    // printf("Testing new candidates #%li - 0x%10lx ...\n", index, item);
    uint8_t byte0 = getByteFromRange(32, 39, item);
    uint8_t byte1 = getByteFromRange(24, 31, item);
    uint8_t byte2 = getByteFromRange(16, 23, item);
    uint8_t byte3 = getByteFromRange(8, 15, item);
    uint8_t byte4 = getByteFromRange(0, 7, item);
    // printf("byte0 = %x\n", byte0);
    uint8_t wep_key [5] = { byte0, byte1, byte2, byte3, byte4 };
    // printf("Now testing wep_key: %2x - %2x - %2x - %2x - %2x\n", wep_key[0], wep_key[1], wep_key[2], wep_key[3], wep_key[4]);
    if (TestKey(wep_key)) {
      printf("Found the key here: 0x%10lx !!!\n", item);
      // cout << "Found the key here " << endl;
      gotTheKey = true;
      correctKey = item;
      break;
    }
  }
}

uint8_t PTWSearcher::getByteFromRange(uint64_t a, uint64_t b, uint64_t number) {
  uint64_t mask = 0;
  for (uint64_t i=a; i<=b; i++)
    mask |= (uint64_t)1 << i;
  return (uint8_t) ((number&mask)>>a);
}


std::string PTWSearcher::uint64_tToString(const uint64_t t)
{
    std::ostringstream ss;
    ss << t;
    return ss.str();
}

void PTWSearcher::recordTestedCandidates() {
  for (uint64_t i = 0; i < candidates_new.size(); i++)
  {
    candidates_tested.push_back(candidates_new[i]);
  }
}

void PTWSearcher::removeTestedCandidates() {
  if (candidates_tested.size()!=0&&candidates_tested.size()!=0)
  {
    printf("Removing tested ones...\n");
    for (uint64_t i = 0; i < candidates_tested.size(); i++) {
      candidates_new.erase(std::remove(candidates_new.begin(), candidates_new.end(), candidates_tested[i]), candidates_new.end());
    }
    printf("Still has %li new candidates\n", candidates_new.size());
  } else {
    printf("No tested keys to be removed...\n");
  }
}
