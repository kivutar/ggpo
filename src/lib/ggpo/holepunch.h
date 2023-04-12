#ifndef _PUNCH_H
#define _PUNCH_H

#include "types.h"
#include "poll.h"
#include "network/udp_proto.h"

class Punch : public Udp::Callbacks {
public:
   Punch();

   public:
      virtual void OnMsg(sockaddr_in &from, UdpMsg *msg, int len);
};

#endif
