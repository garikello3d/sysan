// Copyright 2013 The WebRTC Project Authors. All rights reserved.

// Taken from /external/chromium_org/third_party/webrtc/base/

#pragma once
#include <stdio.h>
#include <sys/socket.h>

// Implementation of getifaddrs for Android.
// Fills out a list of ifaddr structs (see below) which contain information
// about every network interface available on the host.
// See 'man getifaddrs' on Linux or OS X (nb: it is not a POSIX function).
struct ifaddrs {
  struct ifaddrs* ifa_next;
  char* ifa_name;
  unsigned int ifa_flags;
  struct sockaddr* ifa_addr;
  struct sockaddr* ifa_netmask;
  // Real ifaddrs has broadcast, point to point and data members.
  // We don't need them (yet?).
};

int getifaddrs(struct ifaddrs** result);
void freeifaddrs(struct ifaddrs* addrs);
