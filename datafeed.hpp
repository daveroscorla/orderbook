#pragma once

#include <assert.h> 
#include <limits>
#include <math.h>
#include <vector>

#include "orderbook.hpp"

namespace orderbook {

std::vector<orderbook::Order> datafeed { 
{ OrderID_: 1, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::BUY, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US30303M1027", Price_:100.1, Volume_: 10000 },
{ OrderID_: 2, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::BUY, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US30303M1027", Price_:100.0, Volume_: 5000 },
{ OrderID_: 3, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::BUY, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US30303M1027", Price_:100.0, Volume_: 5000 },
{ OrderID_: 4, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::BUY, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US30303M1027", Price_:99.9, Volume_: 2000 },
{ OrderID_: 2, OrderAction_: orderbook::OrderAction_t::Modify, OrderDirection_: orderbook::OrderDirection_t::BUY, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US30303M1027", Price_:100.0, Volume_: 500 },
{ OrderID_: 1, OrderAction_: orderbook::OrderAction_t::Delete, OrderDirection_: orderbook::OrderDirection_t::BUY, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US30303M1027", Price_:100.1, Volume_: 10000 },
{ OrderID_: 7, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::SELL, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US30303M1027", Price_:100.1, Volume_: 10000 },
{ OrderID_: 8, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::SELL, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US30303M1027", Price_:100.5, Volume_: 7000 },
{ OrderID_: 9, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::SELL, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US30303M1027", Price_:100.8, Volume_: 500 },
// Reverse the order and make sure we get the same book entries as previously
{ OrderID_: 11, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::BUY, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US02079K1079", Price_:99.9, Volume_: 2000 },
{ OrderID_: 12, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::BUY, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US02079K1079", Price_:100.0, Volume_: 5000 },
{ OrderID_: 13, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::BUY, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US02079K1079", Price_:100.0, Volume_: 5000 },
{ OrderID_: 14, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::BUY, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US02079K1079", Price_:100.1, Volume_: 10000 },
{ OrderID_: 12, OrderAction_: orderbook::OrderAction_t::Modify, OrderDirection_: orderbook::OrderDirection_t::BUY, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US02079K1079", Price_:100.0, Volume_: 500 },
{ OrderID_: 14, OrderAction_: orderbook::OrderAction_t::Delete, OrderDirection_: orderbook::OrderDirection_t::BUY, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US02079K1079", Price_:100.1, Volume_: 10000 },
{ OrderID_: 15, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::SELL, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US02079K1079", Price_:100.1, Volume_: 10000 },
{ OrderID_: 16, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::SELL, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US02079K1079", Price_:100.8, Volume_: 500 },
{ OrderID_: 17, OrderAction_: orderbook::OrderAction_t::Add, OrderDirection_: orderbook::OrderDirection_t::SELL, OrderConstraints_: orderbook::OrderConstraints_t::MARKET, SecurityID_: "US02079K1079", Price_:100.5, Volume_: 7000 }
};

void TestBook(const orderbook::Implementation & book) {
    // Symbol existence
    assert(book.Exists("US30303M1027") && "Symbol US30303M1027 does not exist in the orderbook");
    assert(book.Exists("US02079K1079") && "Symbol US02079K1079 does not exist in the orderbook");
    assert(!book.Exists("UK01234X5789") && "Symbol UK01234X5789 should not exist in the orderbook");

    // Check the depth and values
    assert(book.BookDepth("US30303M1027", OrderDirection_t::BUY) == 2 && "The book depth should be 2");
    assert(book.BookDepth("US30303M1027", OrderDirection_t::SELL) == 3 && "The book depth should be 3");

    // Bid side - US30303M1027

    // 2 entries at 100.0
    auto buyElement1 = book.Get("US30303M1027", OrderDirection_t::BUY, 0).first;
    assert(buyElement1->Price_ == 100.0f && "The price should be 100.0");
    assert(buyElement1->Volume_ == 500 && "The volume should be 500");
    auto buyElement2 = buyElement1 + 1;
    assert(buyElement2->Price_ == 100.0f && "The price should be 100.0");
    assert(buyElement2->Volume_ == 5000 && "The volume should be 5000");

    // 1 entry at 99.9
    buyElement1 = book.Get("US30303M1027", OrderDirection_t::BUY, 1).first;
    assert(buyElement1->Price_ == 99.9f && "The price should be 99.9");
    assert(buyElement1->Volume_ == 2000 && "The volume should be 2000");

    // Ask side - US30303M1027

    // 1 entry at 100.1
    buyElement1 = book.Get("US30303M1027", OrderDirection_t::SELL, 0).first;
    assert(buyElement1->Price_ == 100.1f && "The price should be 100.1");
    assert(buyElement1->Volume_ == 10000 && "The volume should be 10000");

    // 1 entry at 100.5
    buyElement1 = book.Get("US30303M1027", OrderDirection_t::SELL, 1).first;
    assert(buyElement1->Price_ == 100.5f && "The price should be 100.5");
    assert(buyElement1->Volume_ == 7000 && "The volume should be 7000");

    // 1 entry at 100.8
    buyElement1 = book.Get("US30303M1027", OrderDirection_t::SELL, 2).first;
    assert(buyElement1->Price_ == 100.8f && "The price should be 100.8");
    assert(buyElement1->Volume_ == 500 && "The volume should be 500");
    // Bid side - US02079K1079

    // 2 entries at 100.0
    buyElement1 = book.Get("US02079K1079", OrderDirection_t::BUY, 0).first;
    assert(buyElement1->Price_ == 100.0f && "The price should be 100.0");
    assert(buyElement1->Volume_ == 500 && "The volume should be 500");
    buyElement2 = buyElement1 + 1;
    assert(buyElement2->Price_ == 100.0f && "The price should be 100.0");
    assert(buyElement2->Volume_ == 5000 && "The volume should be 5000");

    // 1 entry at 99.9
    buyElement1 = book.Get("US02079K1079", OrderDirection_t::BUY, 1).first;
    assert(buyElement1->Price_ == 99.9f && "The price should be 99.9");
    assert(buyElement1->Volume_ == 2000 && "The volume should be 2000");

    // Ask side - US02079K1079

    // 1 entry at 100.1
    buyElement1 = book.Get("US02079K1079", OrderDirection_t::SELL, 0).first;
    assert(buyElement1->Price_ == 100.1f && "The price should be 100.1");
    assert(buyElement1->Volume_ == 10000 && "The volume should be 10000");

    // 1 entry at 100.5
    buyElement1 = book.Get("US02079K1079", OrderDirection_t::SELL, 1).first;
    assert(buyElement1->Price_ == 100.5f && "The price should be 100.5");
    assert(buyElement1->Volume_ == 7000 && "The volume should be 7000");

    // 1 entry at 100.8
    buyElement1 = book.Get("US02079K1079", OrderDirection_t::SELL, 2).first;
    assert(buyElement1->Price_ == 100.8f && "The price should be 100.8");
    assert(buyElement1->Volume_ == 500 && "The volume should be 500");

    std::cout<<"All tests passed...."<<std::endl;
}    
    
} // namespace orderbook


