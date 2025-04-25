message("--- lwip 210")
set(LWIP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/drivers/lwIP-210)


    target_include_directories(${TARGET_NAME} PUBLIC
        ${LWIP_PATH}/src/include
        ${LWIP_PATH}/src/include/netif/ppp
        ${LWIP_PATH}/src/include/lwip
        ${LWIP_PATH}/src/include/lwip/apps
        ${LWIP_PATH}/src/include/lwip/priv
        ${LWIP_PATH}/src/include/lwip/prot
        ${LWIP_PATH}/src/include/netif#
        ${LWIP_PATH}/src/include/compat/posix#
        ${LWIP_PATH}/src/include/compat/posix/arpa#
        ${LWIP_PATH}/src/include/compat/posix/net#
        ${LWIP_PATH}/src/include/compat/posix/sys#
        ${LWIP_PATH}/src/include/compat/stdc#
        ${LWIP_PATH}/port/lan8742
        ${LWIP_PATH}/port/lwip/App
        ${LWIP_PATH}/port/lwip/arch
        ${LWIP_PATH}/port/lwip/Target
        ${LWIP_PATH}/port/lwip/Target/Base
       #${LWIP_PATH}/port/lwip/Target/FreeRTOS
    )


    set(COREDIR  ${LWIP_PATH}/src/core)
    set(CORE4DIR ${LWIP_PATH}/src/core/ipv4)
    set(APIDIR   ${LWIP_PATH}/src/api)
    set(NETIFDIR ${LWIP_PATH}/src/netif)
    set(PPPDIR   ${LWIP_PATH}/src/netif/ppp)

    target_sources(${TARGET_NAME} PUBLIC
    #COREFILES
    ${COREDIR}/init.c
    ${COREDIR}/def.c
    ${COREDIR}/dns.c
    ${COREDIR}/inet_chksum.c
    ${COREDIR}/ip.c
    ${COREDIR}/mem.c
    ${COREDIR}/memp.c
    ${COREDIR}/netif.c
    ${COREDIR}/pbuf.c
    ${COREDIR}/raw.c
    ${COREDIR}/stats.c
    ${COREDIR}/sys.c
    ${COREDIR}/altcp.c
    ${COREDIR}/altcp_alloc.c
    ${COREDIR}/altcp_tcp.c
    ${COREDIR}/tcp.c
    ${COREDIR}/tcp_in.c
    ${COREDIR}/tcp_out.c
    ${COREDIR}/timeouts.c
    ${COREDIR}/udp.c
    #CORE4FILES
    ${CORE4DIR}/autoip.c   
    ${CORE4DIR}/dhcp.c
    ${CORE4DIR}/etharp.c
    ${CORE4DIR}/icmp.c
    ${CORE4DIR}/igmp.c
    ${CORE4DIR}/ip4_frag.c
    ${CORE4DIR}/ip4.c
    ${CORE4DIR}/ip4_addr.c
    #APIFILES
    ${APIDIR}/api_lib.c
    ${APIDIR}/api_msg.c
    ${APIDIR}/err.c
    ${APIDIR}/if_api.c
    ${APIDIR}/netbuf.c
    ${APIDIR}/netdb.c
    ${APIDIR}/netifapi.c
    ${APIDIR}/sockets.c
    ${APIDIR}/tcpip.c
    #NETIFFILES
    ${LWIP_PATH}/src/netif/ethernet.c
    ${LWIP_PATH}/src/netif/bridgeif.c
    ${LWIP_PATH}/src/netif/bridgeif_fdb.c
    ${LWIP_PATH}/src/netif/slipif.c
    #TARGET
    ${LWIP_PATH}/port/lan8742/lan8742.c
    ${LWIP_PATH}/port/LwIP/App/lwipApp.c
    ${LWIP_PATH}/port/LwIP/App/ithandler.c
    ${LWIP_PATH}/port/LwIP/App/gd32f4xx_enet_eval.c
    ${LWIP_PATH}/port/LwIP/App/netconf.c
    ${LWIP_PATH}/port/LwIP/arch/sys_arch.c

    ${LWIP_PATH}/port/LwIP/App/gd32f4xx_it.c

    #${LWIP_PATH}/port/LwIP/Target/FreeRTOS/ethernetif.c
    ${LWIP_PATH}/port/LwIP/Target/Base/ethernetif.c
    )


    