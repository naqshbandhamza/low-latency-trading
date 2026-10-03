YOUR CURRENT MARKET-DATA ARCHITECTURE
================================================================================================

┌─────────────────────────────────┐
│ NASDAQ ITCH 5.0 BinaryFILE      │
│                                 │
│ 01302019.NASDAQ_ITCH50          │
│ ~368M ITCH messages             │
└───────────────┬─────────────────┘
│
┌───────────────────┴────────────────────┐
│                                        │
│                                        │
▼                                        ▼
┌────────────────────────┐                ┌────────────────────────┐
│ FileItchRecoverySource │                │ itch_udp_replay_gap    │
│                        │                │                        │
│ Historical recovery    │                │ TEST / SIMULATION TOOL │
│ path                   │                │                        │
│                        │                │ BinaryFILE → UDP       │
│ .idx sparse index      │                │                        │
└────────────┬───────────┘                │ Can inject:            │
│                            │ • 1 packet gap         │
│ recovery request           │ • 10 packet gap        │
│                            │ • 100 packet gap       │
│                            │ • 1000 packet gap      │
│                            └────────────┬───────────┘
│                                         │
│                                         │ UDP
│                                         │
│                                         ▼
│                            ┌────────────────────────┐
│                            │ UdpItchMarketDataSource│
│                            │                        │
│                            │ socket / recvfrom()    │
│                            └────────────┬───────────┘
│                                         │
│                           ItchUdpPacket  │
│                                         ▼
│                            ┌────────────────────────┐
└───────────────────────────►│    ItchFeedHandler     │
│                        │
│ LIVE FEED CONTROL      │
│                        │
│ expectedSequence       │
│ gap detection          │
│ stale detection        │
│ recovery coordination  │
└────────────┬───────────┘
│
┌──────────────────────────┼────────────────────────┐
│                          │                        │
seq == expected            seq > expected           seq < expected
│                          │                        │
│                          │                        │
▼                          ▼                        ▼
PROCESS LIVE              GAP DETECTED                 IGNORE
│
▼
┌────────────────────┐
│ItchSequenceRecovery│
└─────────┬──────────┘
│
│ [expected, received)
▼
FileItchRecoverySource
│
│
recovered packets
│
▼
ItchFeedHandler
│
┌───────────────────────┘
│
▼
┌──────────────────────┐
│    ItchDispatcher    │
│                      │
│ Decode ITCH message  │
│ types                │
└──────────┬───────────┘
│
▼
┌──────────────────────┐
│   ItchMarketState    │
│                      │
│ Instruments          │
│ Orders               │
│ Order books          │
│ Executions/trades    │
└──────────┬───────────┘
│
market state changes
│
┌─────────────┴──────────────┐
│                            │
▼                            ▼
┌────────────────┐          ┌────────────────┐
│     Quote      │          │     Trade      │
│                │          │                │
│ best bid       │          │ price          │
│ best ask       │          │ quantity       │
│ instrument     │          │ instrument     │
│ timestamp      │          │ timestamp      │
└───────┬────────┘          └───────┬────────┘
│                           │
└─────────────┬─────────────┘
│
▼
┌──────────────────────┐
│ SpscRingBuffer       │
│ <MarketEvent, 4096>  │
│                      │
│ lock-free SPSC       │
│ acquire / release    │
│ preallocated storage │
└──────────┬───────────┘
│
│ consumer thread
▼
┌──────────────────────┐
│ MarketEvent Consumer │
│                      │
│ Quote                │
│ Trade                │
└──────────────────────┘
