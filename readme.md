NASDAQ REAL-TIME MARKET-DATA INFRASTRUCTURE ARCHITECTURE
========================================================

![1791098256762](images/readme/1791098256762.png)


### Lock-Free SPSC Throughput

The SPSC benchmark transports pre-created `MarketEvent` objects between
one producer and one consumer thread using a 4,096-slot preallocated queue.
The workload contains an even 50/50 mix of `Quote` and `Trade` events.


|     Events |    Quotes |    Trades | Runs | Median Time |    Median Throughput |
| ---------: | --------: | --------: | ---: | ----------: | -------------------: |
| 10,000,000 | 5,000,000 | 5,000,000 |    5 |     0.392 s | **25.5M events/sec** |

Measured using a Release build. Event construction is performed before
the timed section.
