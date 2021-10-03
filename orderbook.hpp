#pragma once

#include <deque>
#include <iostream>
#include <map>
#include <unordered_map>

namespace orderbook {

    static constexpr int SUCCESS           = 0; 
    static constexpr int ERR_ORD_ACTION    = 1; 
    static constexpr int ERR_ORD_DIR       = 2; 
    static constexpr int ERR_ORD_SECID     = 3; 
    static constexpr int ERR_ORD_PRICE     = 4; 
    static constexpr int ERR_ORD_ORDERID   = 5; 

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

    using BidLevels = std::map<float, std::deque<Order>, std::greater<float>>;
    using AskLevels = std::map<float, std::deque<Order>, std::less<float>>;

    struct Security {
        BidLevels bid_;
        AskLevels ask_;
    };

    using Book = std::unordered_map<std::string, Security>;
    
    class Implementation {
        public:
            void Reserve(std::size_t size);
            int Process(const Order & order);
            bool Exists(const std::string & security) const;
            size_t BookDepth(const std::string & security, OrderDirection_t direction) const;
            std::pair<std::deque<Order>::const_iterator, std::deque<Order>::const_iterator> Get(const std::string & security, OrderDirection_t direction, int index) const;
            std::pair<std::deque<Order>::const_iterator, std::deque<Order>::const_iterator> Top(const std::string & security, OrderDirection_t direction) const;
            int PrintBook(const std::string & security) const;

        private:    
            int Add(const Order & order, Security & security);
            int Modify(const Order & order, Security & security);
            int Delete(const Order & order, Security & security);

            template<typename T>
            int AddToBook(const Order & order, T & levels);
            template<typename T>
            int ModifyBook(const Order & order, T & levels);
            template<typename T>
            int DeleteFromBook(const Order & order, T & levels);
            
            Book orderbook_;
    };

    // Assuming order ids are incrementing, we can do a binary search to find existing orders for modification or deletion
    static auto ordercompare = [] (const orderbook::Order & lhs, const orderbook::Order & rhs) { return (lhs.OrderID_ < rhs.OrderID_); };

    template<typename T>
    int Implementation::AddToBook(const Order & order, T & levels) {
        auto pos = levels.find(order.Price_);

        if( pos != levels.end() ) {
            pos->second.emplace_back(std::move(order));;
        } else {
            levels[order.Price_] = std::deque<Order>{};
            levels[order.Price_].emplace_back(std::move(order));
        }
        return SUCCESS; 
    }

    template<typename T>
    int Implementation::ModifyBook(const Order & order, T & levels) {
        auto pos = levels.find(order.Price_);

        if( pos != levels.end() ) {
            auto entry = std::lower_bound(pos->second.begin(), pos->second.end(), order, ordercompare);
            if( entry != pos->second.end() && entry->OrderID_ == order.OrderID_) {
                *entry = std::move(order); 
            } else {
                std::cerr<<"Could not find entry for Price ["<< order.SecurityID_<<"] Price: ["<<order.Price_<<"] OrderID ["<<order.OrderID_<<"]\n";
                return ERR_ORD_ORDERID;
            }
        } else {
            std::cerr<<"Could not find entry for SecurityID ["<< order.SecurityID_<<"] Price: ["<<order.Price_<<"]\n";
            return ERR_ORD_PRICE;
        }
        return SUCCESS;
    }

    template<typename T>
    int Implementation::DeleteFromBook(const Order & order, T & levels) {
        auto pos = levels.find(order.Price_);

        if( pos != levels.end() ) {
            auto entry = std::lower_bound(pos->second.begin(), pos->second.end(), order, ordercompare);
            if( entry != pos->second.end() ) {
                pos->second.erase(entry);
                if (pos->second.empty()) {
                    levels.erase(pos); // We can remove this price level
                }
            } else {
                std::cerr<<"Could not find entry for Price ["<< order.SecurityID_<<"] Price: ["<<order.Price_<<"] OrderID ["<<order.OrderID_<<'\n';
                return ERR_ORD_ORDERID;
            }
        } else {
            std::cerr<<"Could not find entry for SecurityID ["<< order.SecurityID_<<"] Price: ["<<order.Price_<<"]"<<'\n';
            return ERR_ORD_PRICE;
        }
        return SUCCESS;
    }                

}
