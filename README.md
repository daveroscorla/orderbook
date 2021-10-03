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

## Implementation

### Defining an order

The first attempt at an order book struct is defined as follows:

```
    enum class OrderAction_t {
        Add, Modify, Delete  
    };

    enum class OrderDirection_t {
        BUY, SELL
    };

    enum class OrderConstraints_t {
        LIMIT, MARKET  
    };

    struct Order {
        int_fast64_t OrderID_;
        OrderAction_t OrderAction_;
        OrderDirection_t OrderDirection_;
        OrderConstraints_t OrderConstraints_;
        std::string SecurityID_;
        float Price_;
        int_fast64_t Volume_;
    };
 ```
    
`OrderID_` and `Volume_` were assigned `int_fast64_t`, as you would imagine that large volumes of trades might exceed the limits of a 32 bit int, which would be catastrophic. Details of the order are stored as enums. This potentially could be improved, as discussed later. <link in here> `SecurityID_` has been made a string, however again as the default size for a string is generally larger than an identifier such as an ISIN, this could be made smaller. `Price` is a float, which should be sufficient to represent a price, however as floating point comparisons can sometimes be innacurate, some more thought could be given here.

### Order book structure

The main lookup of the security has been implemented as an `unordered_map`:

```
    using Book = std::unordered_map<std::string, Security>;
```

The rationale is that while a regular `std::map` will perform well for small collections, even logarithmic complexity will have a cost as the number of elements becomes large. An unordered_map, while comsuming extra memory, should have a near constant time look-up. A `Reserve` function was added to the implementation to size the map appropriately at the start of the day rather than trigger a lot of re-allocation of memory and copies until we receive all symbols that comprise a market. 

The sides of the book have been aggregated in a `Security` struct and are implemented as std::maps:

```
    using BidLevels = std::map<float, std::deque<Order>, std::greater<float>>;
    using AskLevels = std::map<float, std::deque<Order>, std::less<float>>;
```

The thinking here is that book depths are pretty finite, so look-ups on price should be relatively quick. Insertions and deletions from the book should also have logarithmic complexity if not at the top of the book, if at the top or the end, then it should be constant. Resizing of the container will not be a problem as the map is generally implemented as a red black binary tree. Using an unordered map here is not an option as we need to maintain ordering, here using `std::greater` and `std::less` to make sure that bids are ordered in ascending price and offers in descending.

At each price level, we have a `std::deque`, to aggregate the orders:

```
  std::deque<Order>
```
  
Here I've assumed order number will be sequentially incrementing, so a queue is a good structure in order to store these sequential Orders. Insertions will be at the front and deletions will be from the end, which is fast for a deque. As the orders are ordered, look-ups can be done using a binary search, which will have O(log n) complexity. Another reason for using a deque over a set for instance is a deque should be more cache friendly.
  
## Testing the implementation
  
To validate the orderbook, a test 'feed' was implemented, using 2 securities, Facebook and Google. Bid and offers were added, modified and deleted with the results checked. While the values were the same in both runs, the price ordering was reversed between the two symbols, to confirm that the logic for the book order was correct. To confirm, some simple unit tests were written to look at how many symbols the book had, the book depths and the individual Orders at each price level.

  <pictures here?
            
## Improvements
            
The 21 million securities question is an interesting one. In terms of the algorithm, I think it's pretty efficient. The used of an `unordered_map` alleviated most of the cost of a lot of securities as far as speed is concerned. 
                                                                                                
In terms of the data structures themselves, it's clear that while nice an readable, the original `Order` structure is certainly not optimised for size. Another attempt was made to see what scope there was to minimise this footprint and an alternative implementation was made using bitfields and fixed length arrays:
            
```
    constexpr unsigned short ORDER_ACTION_ADD = 1;
    constexpr unsigned short ORDER_ACTION_MODIFY = 2;
    constexpr unsigned short ORDER_ACTION_DELETE = 3;

    constexpr unsigned short ORDER_DIRECTION_BUY = 0;
    constexpr unsigned short ORDER_DIRECTION_SELL = 1;

    constexpr unsigned short ORDER_CONSTRAINT_LIMIT = 0;
    constexpr unsigned short ORDER_CONSTRAINT_MARKET = 1;

    constexpr unsigned int ID_LEN = 13;

    struct Order {
        int_fast64_t OrderID_;
        unsigned short OrderAction_ : 2;
        unsigned short OrderDirection_ : 1;
        unsigned short OrderConstraints_ : 1;
        char SecurityID_ [ID_LEN]; 
        float Price_; 
        int_fast64_t Volume_;
    };
```  

In test the following space savings were seen:
            
            <picture>
  
## Conclusions
  
With limited time, it's probably not the ultimate orderbook implementation however seems reasonably efficient. With testing, the relative efficiencies of the various containers would become clearer and could be benchmarked. Also, a bit of business knowledge can also help to optimise i.e. what's the initial size to set the main map to avoid resizing and re-allocation/coprying? 





    
