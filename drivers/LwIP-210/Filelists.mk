# COREFILES, CORE4FILES: The minimum set of files needed for lwIP.
COREFILES=	init.o \
			def.o \
			dns.o \
			inet_chksum.o \
			ip.o \
			mem.o \
			memp.o \
			netif.o \
			pbuf.o \
			raw.o \
			stats.o \
			sys.o \
			altcp.o \
			altcp_alloc.o \
			altcp_tcp.o \
			tcp.o \
			tcp_in.o \
			tcp_out.o \
			timeouts.o \
			udp.o#
COREDIR=${LWIPDIR}/src/core#

CORE4FILES=	autoip.o \
			dhcp.o \
			etharp.o \
			icmp.o \
			igmp.o \
			ip4_frag.o \
			ip4.o \
			ip4_addr.o
CORE4DIR=${LWIPDIR}/src/core/ipv4#

# APIFILES: The files which implement the sequential and socket APIs.
APIFILES=	api_lib.o \
			api_msg.o \
			err.o \
			if_api.o \
			netbuf.o \
			netdb.o \
			netifapi.o \
			sockets.o \
			tcpip.o
APIDIR=${LWIPDIR}/src/api#

# NETIFFILES: Files implementing various generic network interface functions
NETIFFILES=	ethernet.o \
			bridgeif.o \
			bridgeif_fdb.o \
			slipif.o
NETIFDIR=${LWIPDIR}/src/netif#

# SIXLOWPAN: 6LoWPAN
SIXLOWPAN=	lowpan6_common.o \
        	lowpan6.o \
			lowpan6_ble.o \
			zepif.o

# PPPFILES: PPP
PPPFILES=	auth.o \
			ccp.o \
			chap-md5.o \
			chap_ms.o \
			chap-new.o \
			demand.o \
			eap.o \
			ecp.o \
			eui64.o \
			fsm.o \
			ipcp.o \
			ipv6cp.o \
			lcp.o \
			magic.o \
			mppe.o \
			multilink.o \
			ppp.o \
			pppapi.o \
			pppcrypt.o \
			pppoe.o \
			pppol2tp.o \
			pppos.o \
			upap.o \
			utils.o \
			vj.o \
			arc4.o \
			des.o \
			md4.o \
			md5.o \
			sha1.o#
PPPDIR=${LWIPDIR}/src/netif/ppp ${LWIPDIR}/src/netif/ppp/polarssl#

# LWIPNOAPPSFILES: All LWIP files without apps
LWIPNOAPPSFILES=$(COREFILES) \
	$(CORE4FILES) \
	$(APIFILES) \
	$(NETIFFILES) \
	$(PPPFILES) \
	$(SIXLOWPAN) \
#	coverity.o

LWIPNOAPPSDIRS=$(COREDIR) \
	$(CORE4DIR) \
	$(APIDIR) \
	$(NETIFDIR) \
	$(PPPDIR) \
	$(SIXLOWPANDIR) \
	${LWIPDIR}/system/OS \
#	${LWIPDIR}/../contrib/Coverity#
