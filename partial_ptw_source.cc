// Copyright 2015, University of North Carolina
// Authors: Kevin Snow, Andrew White, Nathan Otterness
//
// This file contains a restricted portion of the source code for ptw.o. This
// file is for your reference only; the implementation of UpdateApproximation
// for extra credit should be placed in ptw_sublcass.cc/.h instead.

#include <pcap/pcap.h>
#include <string.h>
#include <stdint.h>
#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include "rc4.h"
#include "ptw.h"

using std::cout;
using std::endl;
using std::string;
using std::set;
using std::queue;
using std::vector;

// plaintext of an ARP request
static uint8_t ArpRequestPlaintext[16] = {
  0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x08, 0x06,
  0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01
};

// plaintext of an ARP response
static uint8_t ArpResponsePlaintext[16] = {
  0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x08, 0x06,
  0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x02
};

void Ptw::Initialize(size_t iv_size, size_t key_size) {
  iv_size_ = iv_size;
  key_size_ = key_size;
  // Clear any existing entries
  ptw_table_.clear();
  ivs_seen_.clear();
  // Initialize a vector of approximation entries
  PTWEntry entry;
  PTWEntryList tmp;
  for (int i = 0; i < 256; ++i) {
    entry.approximation = i;
    entry.votes = 0;
    tmp.push_back(entry);
  }
  // Initialize vector to store approximations for each key byte
  for (size_t i = 0; i < key_size; ++i) {
    ptw_table_.push_back(tmp);
  }
}

void Ptw::UpdateApproximation(const uint8_t* iv, const uint8_t* plaintext,
    const uint8_t* ciphertext, size_t text_size) {
  // test if we have seen this IV before
  if (CheckAndLogIv(iv)) return;
  // The rest of this function has been omitted. It's extra credit, see
  // ptw_subclass.h.
}

void Ptw::ProcessNetworkTrace(const string& filename) {
  pcap_t* pcap;
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap = pcap_open_offline(filename.c_str(), errbuf);
  if (!pcap) {
    cout << "Failed to open: " << filename << ", " << errbuf << endl;
    exit(1);
  }
  int data_link_layer = pcap_datalink(pcap);
  if (data_link_layer != DLT_IEEE802_11) {
    cout << "This doesn't appear to be 802.11 data captured in monitor mode"
         << " (got " << pcap_datalink_val_to_description(data_link_layer)
         << " instead) " << endl;
    exit(1);
  }
  const uint8_t *packet;
  pcap_pkthdr pcap_header;
  uint32_t count = 0;
  while ((packet = pcap_next(pcap, &pcap_header)) != NULL) {
    // Throw away packets too short to be useful
    if (pcap_header.len < 28) continue;
    // Skip packet if the captured size is too small
    if (pcap_header.caplen < pcap_header.len) continue;

    // Decompose the various parts of the packet
    // The Wifi Header occupies bytes 0 to 24
    const WifiHeader* wifi_header = (WifiHeader*) packet;
    // The WEP header occupies bytes 24 to 28
    const WepHeader* wep_header = (WepHeader*) ((char*)packet +
      sizeof(WifiHeader));
    // The remainder is the payload
    const uint8_t* payload = ((uint8_t*) wep_header) + sizeof(WepHeader);
    size_t payload_size = pcap_header.len -
      ((size_t) payload - (size_t) packet) - 4;
    const uint8_t* icv = packet + pcap_header.len - 4;
    HandleWepPacket(wifi_header, wep_header, payload, payload_size, icv);
    count++;
  }
  cout << "Processed " << count << " packets." << endl;
}

void Ptw::HandleWepPacket(const WifiHeader* wifi_header,
    const WepHeader* wep_header, const uint8_t* payload,
    size_t payload_size, const uint8_t* icv) {
  static uint8_t first_bssid[6] = {0};
  static bool got_first_bssid = false;
  const uint8_t *bssid, *dst;
  // Skip the packet if it isn't an 802.11 data packet
  if (!(wifi_header->frame_control_1 & 0x08)) return;
  // Figure out which addresses represent BSSID vs. dest based on the 802.11
  // frame control bits.
  if (wifi_header->frame_control_2 & 0x01) {
    bssid = wifi_header->address_1;
    dst = wifi_header->address_3;
  } else if (wifi_header->frame_control_2 & 0x02) {
    bssid = wifi_header->address_2;
    dst = wifi_header->address_1;
  } else if (wifi_header->frame_control_2 & 0x00) {
    bssid = wifi_header->address_3;
    dst = wifi_header->address_1;
  } else {
    return;
  }
  // Only process the packets for the first BSSID
  if (!got_first_bssid) {
    for (int i = 0; i < 6; i++) {
      first_bssid[i] = bssid[i];
    }
    got_first_bssid = true;
  }
  for (int i=0; i<6; i++) {
    if (first_bssid[i] != bssid[i]) return;
  }
  // Assume the encrypted packet is ARP if it's 54 bytes
  if (payload_size == 54) {
    if (dst[0] == 0xFF && dst[1] == 0xFF && dst[2] == 0xFF &&
        dst[3] == 0xFF && dst[4] == 0xFF && dst[5] == 0xFF) {
      // Broadcast destination address means this is an ARP request
      UpdateApproximation(wep_header->iv, ArpRequestPlaintext, payload, 16);
      return;
    } else {
      // Unicast destination address means this is an ARP response
      UpdateApproximation(wep_header->iv, ArpResponsePlaintext, payload, 16);
      return;
    }
  }
}

bool Ptw::TestKey(const uint8_t wep_key[]) {
  if (!test_iv_.length()) {
    cout << "Error: tried to test a key without first setting the test IV and"
         << " keystream." << endl;
    exit(1);
  }
  string key = test_iv_ + string((char*)wep_key, key_size_);
  Rc4 rc4;
  rc4.Init((uint8_t*)key.c_str(), key.length());
  for (size_t i = 0; i < test_keystream_.length(); ++i) {
    if (rc4.Generate() != (uint8_t)test_keystream_[i])
      return false;
  }
  return true;
}

bool Ptw::CheckAndLogIv(const uint8_t iv[]) {
  uint32_t iv_int = (iv[0] << 16) | (iv[1] << 8) | (iv[2]);
  IvSet::iterator iv_it = ivs_seen_.find(iv_int);
  if (iv_it != ivs_seen_.end()) return true;
  ivs_seen_.insert(iv_int);
  return false;
}

void Ptw::PrintBytes(const uint8_t* bytes, int len) {
  for (int i = 0; i < len; i++) printf("%02X ", bytes[i]);
  printf("\n");
}

void Ptw::PrintPtwTable() {
  for (PTWTable::iterator it = ptw_table_.begin(); it < ptw_table_.end();
    ++it) {
    for (int i=0; i<8; ++i) {
      printf("%02X (%d)  ", (*it)[i].approximation, (*it)[i].votes);
    }
    printf("\n");
  }
}
