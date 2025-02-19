
ADD_DEFINITIONS( -DUUB_LDR_ADR=0x8000000L)  # 0x8010000 - loader address           - sector 0 and 1 

ADD_DEFINITIONS( -DUUB_PRM_ADR=0x8010000L)  # 0x8000000 - setup  storage           - sector 4 

ADD_DEFINITIONS(-DUUB_INF2_ADR=0x8020000L) #  0x8020000 - loader info              - sector 5 and 6
ADD_DEFINITIONS( -DUUB_BAS_ADR=0x8021000L)  # 0x8021000 - program address         - 

ADD_DEFINITIONS(-DUUB_INF1_ADR=0x8060000L) #  0x8060000 - loader info              - sector 7 and 8
ADD_DEFINITIONS( -DUUB_STR_ADR=0x8061000L)  # 0x8061000 - program storage address - 