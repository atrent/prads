void end_sessions();
int  cx_track(struct in6_addr ip_src,uint16_t src_port,struct in6_addr ip_dst,uint16_t dst_port,
               uint8_t ip_proto,uint16_t p_bytes,uint8_t tcpflags,time_t tstamp, int af);
void del_connection (connection*, connection**);
void move_connection (connection*, connection**);

/* For prads, I guess cx_track needs to return a value, which can
 * be used for evaluating if we should do some fingerprinting
 */

/* void cx_track(uint64_t ip_src,uint16_t src_port,uint64_t ip_dst,uint16_t dst_port,
               uint8_t ip_proto,uint16_t p_bytes,uint8_t tcpflags,time_t tstamp, int af) { */

int cx_track(struct in6_addr ip_src,uint16_t src_port,struct in6_addr ip_dst,uint16_t dst_port,
               uint8_t ip_proto,uint16_t p_bytes,uint8_t tcpflags,time_t tstamp, int af) {

   connection *cxt = NULL;
   connection *head = NULL;
   uint64_t hash;

   if (af = AF_INET) {
      hash = (( ip_src.s6_addr32[0] + ip_dst.s6_addr32[0] )) % BUCKET_SIZE;
   } else 
   if (af = AF_INET6) {
      hash = ((  ip_src.s6_addr32[0] + ip_src.s6_addr32[1] + ip_src.s6_addr32[2] + ip_src.s6_addr32[3]
               + ip_dst.s6_addr32[0] + ip_dst.s6_addr32[1] + ip_dst.s6_addr32[2] + ip_dst.s6_addr32[3]
             )) % BUCKET_SIZE;
   }
   extern connection *bucket[BUCKET_SIZE];
   cxt = bucket[hash];
   head = cxt;

   while ( cxt != NULL ) {
      if (af = AF_INET) {
//         if ( cxt->s_ip.s6_addr32[0] == ip_src.s6_addr32[0] && cxt->d_ip.s6_addr32[0] == ip_dst.s6_addr32[0] 
//              && cxt->s_port == src_port && cxt->d_port == dst_port ) {
         if ( memcmp(&cxt->s_ip,&ip_src,4) && memcmp(&cxt->d_ip,&ip_dst,4) && cxt->s_port == src_port && cxt->d_port == dst_port ) {
            cxt->s_tcpFlags    |= tcpflags;
            cxt->s_total_bytes += p_bytes;
            cxt->s_total_pkts  += 1;
            cxt->last_pkt_time  = tstamp;
            if (cxt->s_total_bytes > MAX_BYTE_CHECK || cxt->s_total_pkts > MAX_PKT_CHECK) {
               return 1;
            }
            return 0;
         }
//         else if ( cxt->s_ip6.s6_addr32[0] == ip_dst && cxt->d_ip6.s6_addr32[0] == ip_src
//                   && cxt->d_port == src_port && cxt->s_port == dst_port ) {
         else if ( memcmp(&cxt->s_ip,&ip_dst,4) && memcmp(&cxt->d_ip,&ip_src,4) && cxt->d_port == src_port && cxt->s_port == dst_port ) {
            cxt->d_tcpFlags    |= tcpflags;
            cxt->d_total_bytes += p_bytes;
            cxt->d_total_pkts  += 1;
            cxt->last_pkt_time  = tstamp;
            if (cxt->d_total_bytes > MAX_BYTE_CHECK || cxt->d_total_pkts > MAX_PKT_CHECK) {
               return 1;
            }
            return 0;
         }
      } else
      if (af = AF_INET) {
            if ( memcmp(&cxt->s_ip,&ip_src,16) && memcmp(&cxt->d_ip,&ip_dst,16) &&
                 cxt->s_port == src_port && cxt->d_port == dst_port ) {
               cxt->s_tcpFlags    |= tcpflags;
               cxt->s_total_bytes += p_bytes;
               cxt->s_total_pkts  += 1;
               cxt->last_pkt_time  = tstamp;
               if (cxt->s_total_bytes > MAX_BYTE_CHECK || cxt->s_total_pkts > MAX_PKT_CHECK) {
                  return 1;
               }
               return 0;
            } else
            if ( memcmp(&cxt->s_ip,&ip_dst,16) && memcmp(&cxt->d_ip,&ip_src,16) &&
                 cxt->d_port == src_port && cxt->s_port == dst_port ) {
               cxt->d_tcpFlags    |= tcpflags;
               cxt->d_total_bytes += p_bytes;
               cxt->d_total_pkts  += 1;
               cxt->last_pkt_time  = tstamp;
               if (cxt->d_total_bytes > MAX_BYTE_CHECK || cxt->d_total_pkts > MAX_PKT_CHECK) {
                  return 1;
               }
               return 0;
            }
      }
      cxt = cxt->next;
   }

   if ( cxt == NULL ) {
      extern u_int64_t cxtrackerid;
      cxtrackerid += 1;
      cxt = (connection*) calloc(1, sizeof(connection));
      if (head != NULL ) {
         head->prev = cxt;
      }
      /* printf("[*] New connection...\n"); */
      cxt->cxid           = cxtrackerid;
      cxt->af             = af;
      cxt->s_tcpFlags     = tcpflags;
      cxt->d_tcpFlags     = 0x00;
      cxt->s_total_bytes  = p_bytes;
      cxt->s_total_pkts   = 1;
      cxt->d_total_bytes  = 0;
      cxt->d_total_pkts   = 0;
      cxt->start_time     = tstamp;
      cxt->last_pkt_time  = tstamp;

      cxt->s_ip          = ip_src;
      cxt->d_ip          = ip_dst;

      /* if (af = AF_INET) { */
      /* cxt->s_ip6.s6_addr32[1]          = 0; */
      /* cxt->s_ip6.s6_addr32[2]          = 0; */
      /* cxt->s_ip6.s6_addr32[3]          = 0; */
      /* cxt->d_ip6.s6_addr32[1]          = 0; */
      /* cxt->d_ip6.s6_addr32[2]          = 0; */
      /* cxt->d_ip6.s6_addr32[3]          = 0; */
      /* } */

      cxt->s_port         = src_port;
      cxt->d_port         = dst_port;
      cxt->proto          = ip_proto;
      cxt->next           = head;
      cxt->prev           = NULL;

      /* New connections are pushed on to the head of bucket[s_hash] */
      bucket[hash] = cxt;

      /* Return value should be 0, telling to do service fingerprinting */
      return 0;
   }
   /* Should never be here! */
   return 0;
}
/*
 This sub marks sessions as ENDED on different criterias:
*/

void end_sessions() {

   connection *cxt;
   time_t check_time;
   check_time = time(NULL);
   int cxkey, xpir;
   uint32_t curcxt  = 0;
   uint32_t expired = 0;
   extern connection *cxtbuffer,*bucket[BUCKET_SIZE];
   cxtbuffer = NULL;

   for ( cxkey = 0; cxkey < BUCKET_SIZE; cxkey++ ) {
      cxt = bucket[cxkey];
      xpir = 0;
      while ( cxt != NULL ) {
         curcxt++;
         /* TCP */
         if ( cxt->proto == IP_PROTO_TCP ) {
           /* FIN from both sides */
           if ( cxt->s_tcpFlags & TF_FIN && cxt->d_tcpFlags & TF_FIN && (check_time - cxt->last_pkt_time) > 5 ) {
              xpir = 1;
           }
           /* RST from eather side */
           else if ( (cxt->s_tcpFlags & TF_RST || cxt->d_tcpFlags & TF_RST) && (check_time - cxt->last_pkt_time) > 5) {
              xpir = 1;
           }
           /* if not a complete TCP 3-way handshake */
           else if ( !cxt->s_tcpFlags&TF_SYNACK || !cxt->d_tcpFlags&TF_SYNACK && (check_time - cxt->last_pkt_time) > 10) {
              xpir = 1;
           }
           /* Ongoing timout */
           else if ( (cxt->s_tcpFlags&TF_SYNACK || cxt->d_tcpFlags&TF_SYNACK) && ((check_time - cxt->last_pkt_time) > 120)) {
              xpir = 1;
           }
           else if ( (check_time - cxt->last_pkt_time) > 600 ) {
              xpir = 1;
           }
         }
         else if ( cxt->proto == IP_PROTO_UDP && (check_time - cxt->last_pkt_time) > 60 ) {
            xpir = 1;
         }
         else if ( cxt->proto == IP_PROTO_ICMP || cxt->proto == IP6_PROTO_ICMP ) {
            if ( (check_time - cxt->last_pkt_time) > 60 ) {
               xpir = 1;
            }
         }
         else if ( (check_time - cxt->last_pkt_time) > 300 ) {
            xpir = 1;
         }

         if ( xpir == 1 ) {
            expired++;
            xpir = 0;
            connection *tmp = cxt;
            if (cxt == cxt->next) {
               cxt->next == NULL;
            }
            cxt = cxt->next;
            del_connection(tmp, &bucket[cxkey]);
         }else{
            cxt = cxt->next;
         }
      }
   }
   /* printf("Expired: %u of %u total connections:\n",expired,curcxt); */
}

void del_connection (connection *cxt, connection **bucket_ptr ){
   /* remove cxt from bucket */
   connection *prev = cxt->prev; /* OLDER connections */
   connection *next = cxt->next; /* NEWER connections */
   if(prev == NULL){
      // beginning of list
      *bucket_ptr = next;
      // not only entry
      if(next)
         next->prev = NULL;
   } else if(next == NULL){
      // at end of list!
      prev->next = NULL;
   } else {
      // a node.
      prev->next = next;
      next->prev = prev;
   }

   /* Free and set to NULL */
   free(cxt);
   cxt=NULL;
}

void move_connection (connection* cxt, connection **bucket_ptr ){
   /* remove cxt from bucket */
   extern connection *cxtbuffer;
   connection *prev = cxt->prev; /* OLDER connections */
   connection *next = cxt->next; /* NEWER connections */
   if(prev == NULL){
      // beginning of list
      *bucket_ptr = next;
      // not only entry
      if(next)
         next->prev = NULL;
   } else if(next == NULL){
      // at end of list!
      prev->next = NULL;
   } else {
      // a node.
      prev->next = next;
      next->prev = prev;
   }

   /* add cxt to expired list cxtbuffer 
    - if head is null -> head = cxt;
    */
   cxt->next = cxtbuffer; // next = head
   cxt->prev = NULL;
   cxtbuffer = cxt;       // head = cxt. result: newhead = cxt->oldhead->list...
}

void end_all_sessions() {
   connection *cxt;
   int cxkey;
   int expired = 0;
   extern connection *bucket[BUCKET_SIZE];

   for ( cxkey = 0; cxkey < BUCKET_SIZE; cxkey++ ) {
      cxt = bucket[cxkey];
      while ( cxt != NULL ) {
         expired++;
         connection *tmp = cxt;
         cxt = cxt->next;
         move_connection(tmp, &bucket[cxkey]);
         if ( cxt == NULL ) {
            bucket[cxkey] = NULL;
         }
      }
   }
   /* printf("Expired: %d.\n",expired); */
}


