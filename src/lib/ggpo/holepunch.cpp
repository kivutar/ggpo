#include "holepunch.h"
#include "../../../../netplay.h"

extern int  local_idx;
extern char local_ip[32];
extern int  local_port;
extern char remote_ip[32];
extern int  remote_port;

Punch::Punch() {}

void
Punch::OnMsg(sockaddr_in &from, UdpMsg *msg, int len)
{
   switch (msg->hdr.type) {
      case UdpMsg::OwnIP: {
         strcpy(local_ip, strtok(msg->u.own_ip.ip, ":"));
         local_port = atoi(strtok(NULL, ":"));
         local_idx = msg->u.own_ip.pid;
         printf("Got own IP! Player ID: %d, Player IP: %s:%d\n", local_idx, local_ip, local_port);
         return;
      }
      break;
      case UdpMsg::PeerIP: {
         strcpy(remote_ip, strtok(msg->u.peer_ip.ip, ":"));
         remote_port = atoi(strtok(NULL, ":"));
         printf("Got peer IP! Player ID: %d, Player IP: %s:%d\n", msg->u.peer_ip.pid, remote_ip, remote_port);
         return;
      }
      break;
   }
}

GGPOErrorCode
ggpo_hole_punch(int num_players,
                const char *rdvaddr,
                int rdvport)
{
   Poll poll;
   Punch punch;

   Udp *_rdv = new Udp();
   _rdv->Init(0, &poll, &punch, 0);
   sockaddr_in rdv_addr;
   rdv_addr.sin_family = AF_INET;
   rdv_addr.sin_port = htons(rdvport);
   rdv_addr.sin_addr.s_addr = inet_addr(rdvaddr);

   UdpMsg *msg = new UdpMsg(UdpMsg::Join);
   msg->u.join.crc = 3333; // TODO unhardcode this
   _rdv->SendTo((char *)msg, 16, 0, (struct sockaddr *)&rdv_addr, sizeof(rdv_addr));
   delete msg;

   for (int i = 0; i < num_players; i++) {
      _rdv->PollOnce();
   }

   printf("Sending handshake\n");
   sockaddr_in peer_addr;
   peer_addr.sin_family = AF_INET;
   peer_addr.sin_port = htons(remote_port);
   peer_addr.sin_addr.s_addr = inet_addr(remote_ip);

   UdpMsg *hds = new UdpMsg(UdpMsg::HandShake);
   _rdv->SendTo((char *)hds, 8, 0, (struct sockaddr *)&peer_addr, sizeof(peer_addr));
   delete hds;

   for (int i = 0; i < num_players - 1; i++) {
      _rdv->PollOnce();
   }

   delete _rdv;

   printf("Done with hole punching\n");

   return GGPO_OK;
}
