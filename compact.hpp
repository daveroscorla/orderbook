#pragma once

namespace compact {

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

} // namespace compact
