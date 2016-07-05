// Copyright (c) 2012-2013 UNC Chapel Hill. All rights reserved.
// Authors: Kevin Snow, Andrew White, Nathan Otterness
//
// Weep parses ARP packets from an 802.11 pcap and uses the PTW
// attack to crack the WEP key.
#include <stdlib.h>
#include <iostream>
#include "ptw_searcher.h"

using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << "Usage: " << argv[0] << " <pcap filename>" << endl;
    exit(1);
  }
  PTWSearcher ptw;
  // Initialize PTW state; 3 byte IV targeting a 5 byte key
  ptw.Initialize(3, 5);
  // Parse packets, updating PTW approximations along the way
  ptw.ProcessNetworkTrace(argv[1]);
  // Search for the WEP key
  string key = ptw.Search();
  if (key.length() > 0) cout << "Success!" << endl;
  return 0;
}
