# MTH9815-Final-Project-Junliang-Zhou
## Final Project: A Trading System



### General Instructions

A bond trading system for US Treasuries with six securities: 2Y, 3Y, 5Y, 7Y, 10Y, and 30Y with the CUSIPS, coupons, and maturity dates for each security.

A ServiceListener is a listener to events on the service where data is added to the service, updated on the service, or removed from the service. A Connector is a class that flows data into the Service from some connectivity source (e.g. a socket, file, etc) via the Service.OnMessage() method. The Publish() method on the Connector publishes data to the connectivity source and can be invoked from a Service. Some Connectors are publish-only that do not invoke Service.OnMessage(). Some Connectors are subscribe-only where Publish() does nothing. Other Connectors can do both publish and subscribe.

Fractional notation is used for US Treasuries prices when reading from the file and writing back to a file in the HistoricalDataService (with the smallest tick being 1/256th). Example of fractional is 100-xyz, with xy being from 0 to 31 and z being from 0 to 7 (replace z=4 with +). The xy number gives the decimal out of 32 and the z number gives the remainder out of 256. So 100-001 is 100.00390625 in decimal and 100-25+ is 100.796875 in decimal. Output files should have timestamps on each line with millisecond precision.



### PricingService

This reads data from prices.txt. Create 1,000 prices for each security (so a total of 6,000 prices across all 6 securites). The file creates prices which oscillate between 99 and 101 (bearing in mind that US Treasuries trade in 1/256th increments). The bid/offer spread oscillates between 1/128 and 1/64.

The input file format is "Product ID, Bid Price, Offer Price".



### TradeBookingService

This reads data from trades.txt. Create 10 trades for each security (so a total of 60 trades across all 6 securities) in the file with the relevant trade attributes. Positions are across books TRSY1, TRSY2, and TRSY3. The TradeBookingService should be linked to a PositionService via a ServiceListener and send all trades there via the AddTrade() method.

The input file format is "Product ID, Trade ID, Price, Book, Quantity, Side".



### PositionService

The PositionService data flows via ServiceListener from the TradeBookingService. The PositionService should be linked to a RiskService via a ServiceListenher and send all positions to the RiskService via the AddPosition() method.

Output file format is "Time Stamp, Product ID, Book, Quantity, Book, Quantity, Book, Quantity".



### RiskService

The RiskService data flows via ServiceListener from the PositionService.

Output file format is "Time Stamp, Product ID, PV01, Quantity".



### MarketDataService

This reads data from marketdata.txt. Create 1,000 order book updates for each security (so a total of 6,000 prices) each with 5 orders deep on both bid and offer stacks. The top level has a size of 10 million, second level 20 million, 30 million for the third, 40 million for the fourth, and 50 million for the fifth. The file creates mid prices which oscillate between 99 and 101 with a bid/offer spread starting at 1/128th on the top of book (and widening in the smallest increment from there for subsequent levels in the order book). The top of book spread itself widens out on successive updates by 1/128th until it reaches 1/32nd, and then decreases back down to 1/128th in 1/128th intervals (i.e. spread of 1/128th at top of book, then 1/64th, then 3/128th, then 1/32nd, and then back down again to 1/128th, and repeat).

Input file format is "Product ID, Price, Quantity, Pricing Side".



### AlgoExecutionService

The AlgoExecution is a class with a reference to an ExecutionOrder object. AlgoExecutionService registers a ServiceListener on the MarketDataService and aggresses the top of the book, alternating between bid and offer (taking the opposite side of the order book to cross the spread) and only aggressing when the spread is at its tightest (i.e. 1/128th) to reduce the cost of crossing the spread. It sends this order to the ExecutionService via a ServiceListener and the ExecuteOrder() method.



### AlgoStreamingService

The AlgoStream is a class with a reference to a PriceStream object. AlgoStreamingService registers a ServiceListener on the PricingService and sends the bid/offer prices to the StreamingService via a ServiceListener and the PublishPrice() method.



### GUIService

The GUIService is a GUI component that listens to streaming prices that should be throttled. Define the GUIService with a 300 millisecond throttle. It registers a ServiceListener on the PricingService with the specified throttle, which notifies back to the GUIService at that throttle interval. The GUIService outputs those updates with a timestamp with millisecond precision to a file gui.txt.

Output file format is "Time Stamp, Product ID, Mid Price, Spread".



### ExecutionService

The ExecutionService data flows via ServiceListener from the AlgoExecutionService. Each execution results in a trade into the TradeBookingService via ServiceListener on ExectionService – cycling through the books above in order TRSY1, TRSY2, TRSY3.

Output file format is "Time Stamp, Product ID, Pricing Side, Order ID, Order Type, Price, Visible Quantity, Hidden Quantity, Parent Order ID, Child Order".



### StreamingService

The BondStreamingService does not need a Connector since data should flow via ServiceListener from the BondAlgoStreamingService.

Output file format is "Time Stamp, Product ID, Price, Visible Quantity, Hidden Quantity, Pricing Side, Price, Visible Quantity, Hidden Quantity, Pricing Side".



### InquiryService

This reads inquiries from a file called inquiries.txt with attributes for each inquiry (with state of RECEIVED). Create 10 inquiries for each security (so 60 in total across all 6 securities). The InquiryService sends a quote back to a Connector via the Publish() method. The Connector transitions the inquiry to the QUOTED state and sends it back to the InquiryService via the OnMessage() method with the supplied price. It then immediately sends an update of the Inquiry object with a DONE state. Then it moves on to the next inquiry from the file and we repeat the process.

Input file format is "Inquiry ID, Product ID, Side, Quantity, Price, State".

Output file format is "Inquiry ID, Product ID, Side, Quantity, Price, State".



### HistoricalDataService

This service registers a ServiceListener on the following: PositionService, RiskService, ExecutionService, StreamingService, and InquiryService. It persists objects it receives from these services back into files positions.txt, risk.txt, executions.txt, streaming.txt, and allinquiries.txt via special Connectors for each type with a Publish() method on each Connector. There is a HistoricalDataService corresponding to each data type. When persisting positions, It persists each position for a given book. When persisting risk, it persists risk for each security. Use realistic PV01 values for each security. 

### Utility Functions

In order to realize some funtionality of this trading system, some utility functions are created.
