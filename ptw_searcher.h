#ifndef WEEP_SUBCLASS_H
#define WEEP_SUBCLASS_H
#include <string>
#include "ptw.h"

// This is a subclass of the Ptw class. See ptw.h for the methods and variables
// that are already available.
class PTWSearcher : public Ptw {
  public:
    std::string Search();
  // If you want to add other methods or member variables, you may do so here.
    void generateCandidates(int k, int range);
    void removeTestedCandidates();
    void testNewCandidates();
    uint8_t getByteFromRange(uint64_t a, uint64_t b, uint64_t number);
    std::string uint64_tToString(const uint64_t t);
    void recordTestedCandidates();




  // For extra credit, include your own implementation of UpdateApproximation
  // that overrides the implementation in ptw.o/ptw.h
};

#endif
