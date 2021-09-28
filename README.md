# orderbook


## Synopsis
Order is a BUY or SELL instruction, which contains the following attributes:
OrderID
OrderAction [Add, Modify, Delete]
OrderDirection [BUY, SELL]
OrderConstraints [LIMIT, MARKET,etc…]
SecurityID
Price
Volume

## Tasks

1. Write an implementation for an Order Book which is ordered “Market-by-
price”.
Define the data types for the attributes of an Order.
Investigate and lookup terminology as needed to help you better understand
the domain.

2. How would you store the data? What data structure(s) would be best?
Consider resiliency and performance.
You can assume that all open Orders are cleared at the end of the day.

3. How would you improve the algorithm if you had to support 21 million
securities?
What data structures would you use and does that change your architecture?
