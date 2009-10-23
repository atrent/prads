/* For prads, I guess cx_track needs to return a value, which can
 * be used for evaluating if we should do some fingerprinting
 */

/* void cx_track(uint64_t ip_src,uint16_t src_port,uint64_t ip_dst,uint16_t dst_port,
               uint8_t ip_proto,uint16_t p_bytes,uint8_t tcpflags,time_t tstamp, int af) { */

void cx_track(struct in6_addr ip_src,uint16_t src_port,struct in6_addr ip_dst,uint16_t dst_port,
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
   cxt = bucket[hash];
   head = cxt;

   while ( cxt != NULL ) {
      if (af = AF_INET) {
         if ( cxt->s_ip4 == ip_src && cxt->d_ip4 == ip_dst && cxt->s_port == src_port && cxt->d_port == dst_port ) {
            cxt->s_tcpFlags    |= tcpflags;
            cxt->s_total_bytes += p_bytes;
            cxt->s_total_pkts  += 1;
            cxt->last_pkt_time  = tstamp;
            /* Check if :
            * cxt->s_total_bytes > MAX_BYTE_CHECK
            *  or
            * cxt->s_total_pkts > MAX_PACKET_CHECK
            * If so, return value should indicate not to do fingerprinting.
            */
            return;
         }
         else if ( cxt->s_ip4 == ip_dst && cxt->d_ip4 == ip_src && cxt->d_port == src_port && cxt->s_port == dst_port ) {
            cxt->d_tcpFlags    |= tcpflags;
            cxt->d_total_bytes += p_bytes;
            cxt->d_total_pkts  += 1;
            cxt->last_pkt_time  = tstamp;
            /* Check if :
            * cxt->s_total_bytes > MAX_BYTE_CHECK
            *  or
            * cxt->s_total_pkts > MAX_PACKET_CHECK
            * If so, return value should indicate not to do fingerprinting.
            */
            return;
         }
      } else
      if (af = AF_INET) {
            if ( memcmp(&cxt->s_ip6,&ip_src,16) && memcmp(&cxt->d_ip6,&ip_dst,16) &&
                 cxt->s_port == src_port && cxt->d_port == dst_port ) {
               cxt->s_tcpFlags    |= tcpflags;
               cxt->s_total_bytes += p_bytes;
               cxt->s_total_pkts  += 1;
               cxt->last_pkt_time  = tstamp;
               return;
            }else
            if ( memcmp(&cxt->s_ip6,&ip_dst,16) && memcmp(&cxt->d_ip6,&ip_src,16) &&
                 cxt->d_port == src_port && cxt->s_port == dst_port ) {
               cxt->d_tcpFlags    |= tcpflags;
               cxt->d_total_bytes += p_bytes;
               cxt->d_total_pkts  += 1;
               cxt->last_pkt_time  = tstamp;
               return;
            }
      }
      cxt = cxt->next;
   }

   if ( cxt == NULL ) {
      cxtrackerid += 1;
      cxt = (connection*) calloc(1, sizeof(connection));
      if (head != NULL ) {
         head->prev = cxt;
      }
      /* printf("[*] New connection...\n"); */
      cxt->cxid           = cxtrackerid;
      cxt->ipversion      = af;
      cxt->s_tcpFlags     = tcpflags;
      cxt->d_tcpFlags     = 0x00;
      cxt->s_total_bytes  = p_bytes;
      cxt->s_total_pkts   = 1;
      cxt->d_total_bytes  = 0;
      cxt->d_total_pkts   = 0;
      cxt->start_time     = tstamp;
      cxt->last_pkt_time  = tstamp;

/* ADD IF CHECK FOR IPv4 or IPv6 */
      cxt->s_ip4          = ip_src;
      cxt->s_ip6.s6_addr32[0]          = 0;
      /* cxt->s_ip6.s6_addr32[1]          = 0; */
      /* cxt->s_ip6.s6_addr32[2]          = 0; */
      /* cxt->s_ip6.s6_addr32[3]          = 0; */
      cxt->s_port         = src_port;
      cxt->d_ip4          = ip_dst;
      cxt->d_ip6.s6_addr32[0]          = 0;
      /* cxt->d_ip6.s6_addr32[1]          = 0; */
      /* cxt->d_ip6.s6_addr32[2]          = 0; */
      /* cxt->d_ip6.s6_addr32[3]          = 0; */

      cxt->d_port         = dst_port;
      cxt->proto          = ip_proto;
      cxt->next           = head;
      cxt->prev           = NULL;

      /* New connections are pushed on to the head of bucket[s_hash] */
      bucket[hash] = cxt;

      if ( ((tstamp - timecnt) > TIMEOUT) ) {
         timecnt = time(NULL);
         end_sessions();
      }
      /* Return value should be X, telling to do fingerprinting */
      return;
   }
   /* Should never be here! */
   return;
}