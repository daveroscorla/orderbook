#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>

#include "orderbook.hpp"

namespace orderbook {

    void Implementation::Reserve(std::size_t size) {
        orderbook_.reserve(size);
    }

    int Implementation::Process(const Order & order) {
        auto rc = SUCCESS;

        auto entry = orderbook_.find(order.SecurityID_);
        if (entry == orderbook_.end()) {
            // Insert a new entry for this security and return iterator to it
            auto ipair = orderbook_.emplace(std::make_pair(order.SecurityID_, Security{}));
            entry = ipair.first;
        }  

        switch (order.OrderAction_) {
            case OrderAction_t::Add:
                rc = Add(order, entry->second);
                break;
            case OrderAction_t::Modify:
                rc = Modify(order, entry->second);
                break;
            case OrderAction_t::Delete:
                Delete(order, entry->second);
                break;
            default:
                rc = ERR_ORD_ACTION;
                std::cerr<<"Unknown OrderAction "<<static_cast<std::underlying_type<OrderAction_t>::type>(order.OrderAction_)<<'\n'; 
                break;
        }

        return rc;
    }

    bool Implementation::Exists(const std::string & security) const {
        return orderbook_.find(security) != orderbook_.end();
    }

    size_t Implementation::BookDepth(const std::string & security, OrderDirection_t direction) const {
        size_t depth{0};
       
        auto entry = orderbook_.find(security);
        if (entry == orderbook_.end()) {
            std::cerr<<"Unknown security "<<security<<'\n';
        }  else {
            switch (direction) {
            case OrderDirection_t::BUY:
                depth = entry->second.bid_.size();
                break;
            case OrderDirection_t::SELL:
                depth = entry->second.ask_.size();
                break;
            default:
                std::cerr<<"Unhandled OrderDirection "<<static_cast<std::underlying_type<OrderDirection_t>::type>(direction)<<'\n'; 
                break;
            }
        }

        return depth;
    }
    
    std::pair<std::deque<Order>::const_iterator, std::deque<Order>::const_iterator> Implementation::Get(const std::string & security, OrderDirection_t direction, int index) const {
        static std::deque<Order> nullLevel;
        auto entry = orderbook_.find(security);
        if (entry == orderbook_.end()) {
            std::cerr<<"Unknown security "<<security<<'\n';
        }  else {
            switch (direction) {
            case OrderDirection_t::BUY:
                {
                    auto it = entry->second.bid_.begin();
                    std::advance(it, index);
                    return std::make_pair(it->second.begin(), it->second.end());
                }
                break;
            case OrderDirection_t::SELL:
                {
                    auto it = entry->second.ask_.begin();
                    std::advance(it, index);
                    return std::make_pair(it->second.begin(), it->second.end());
                }
                break;
            default:
                std::cerr<<"Unhandled OrderDirection "<<static_cast<std::underlying_type<OrderDirection_t>::type>(direction)<<'\n';
                break;
            }
        }
        return std::make_pair(nullLevel.begin(), nullLevel.end());
    }

    std::pair<std::deque<Order>::const_iterator, std::deque<Order>::const_iterator> Implementation::Top(const std::string & security, OrderDirection_t direction) const {
        return Get(security, direction, 0);
    }

    int Implementation::PrintBook(const std::string & security) const {
        static const std::string startred{"\033[1;31m"};
        static const std::string endred{"\033[0m"};
        static const std::string startgreen{"\u001b[32m"};
        static const std::string endgreen{"\033[0m"};

        auto entry = orderbook_.find(security);
        if (entry == orderbook_.end()) {
            std::cerr<<"Unknown security "<<security<<'\n';
            return ERR_ORD_SECID;
        } 

        std::cout << "Security: " << security << '\n';
        std::cout << std::left << std::setw(19) << std::setfill(' ') << "Bid volume";
        std::cout << '|';
        std::cout << std::left << std::setw(19) << std::setfill(' ') << "Bid price";
        std::cout << '|';
        std::cout << std::left << std::setw(19) << std::setfill(' ') << "Ask volume";
        std::cout << '|';
        std::cout << std::left << std::setw(19) << std::setfill(' ') << "Ask price" << '\n';
        std::cout << std::setw(80) << std::setfill('-') << '-' << '\n';

        auto bidit = entry->second.bid_.begin();
        auto askit = entry->second.ask_.begin();

        while(bidit != entry->second.bid_.end() || askit != entry->second.ask_.end()) {
            // Sum each level
            if(bidit != entry->second.bid_.end()) {
                int_fast64_t volume{0};
                auto & orders = bidit->second;
                for (const auto & order : orders) {
                    volume += order.Volume_;
                }
                std::cout << startgreen;
                std::cout << std::left << std::setw(19) << std::setfill(' ') << volume;
                std::cout << endgreen;
                std::cout << '|';
                std::cout << startgreen;
                std::cout << std::left << std::setw(19) << std::setfill(' ') << bidit->first;
                std::cout << endgreen;
                ++bidit;
            } else {
                std::cout << startgreen;
                std::cout << std::left << std::setw(19) << std::setfill(' ') << "N/A";
                std::cout << endgreen;
                std::cout << '|';
                std::cout << startgreen;
                std::cout << std::left << std::setw(19) << std::setfill(' ') << "N/A";
                std::cout << endgreen;
            }
            std::cout << '|';
            if(askit != entry->second.ask_.end()) {
                int_fast64_t volume{0};
                auto & orders = askit->second;
                for (const auto & order : orders) {
                    volume += order.Volume_;
                }
                std::cout << startred;
                std::cout << std::left << std::setw(19) << std::setfill(' ') << volume;
                std::cout << endred;
                std::cout << '|';
                std::cout << startred;
                std::cout << std::left << std::setw(19) << std::setfill(' ') << askit->first;
                std::cout << endred;
                ++askit;
            } else {
                std::cout << startred;
                std::cout << std::left << std::setw(19) << std::setfill(' ') << "N/A";
                std::cout << endred;
                std::cout << '|';
                std::cout << startred;
                std::cout << std::left << std::setw(19) << std::setfill(' ') << "N/A";
                std::cout << endred;
            }
            std::cout<<std::endl;
        }
    }

    // Perhaps a book the other way is upper_bound? Could this be a template function? Can we overload on something?
    int Implementation::Add(const Order & order, Security & security) {
        int rc = SUCCESS;

        switch (order.OrderDirection_) {
            case OrderDirection_t::BUY:
                AddToBook(order, security.bid_);
                break;
            case OrderDirection_t::SELL:
                AddToBook(order, security.ask_);
                break;
            default:
                rc = ERR_ORD_DIR;
                std::cerr<<"Unhandled OrderDirection "<<static_cast<std::underlying_type<OrderDirection_t>::type>(order.OrderDirection_)<<'\n'; 
                break;
        }

        return rc;      
    }

    int Implementation::Modify(const Order & order, Security & security) {
        int rc = SUCCESS;

        switch (order.OrderDirection_) {
            case OrderDirection_t::BUY:
                rc = ModifyBook(order, security.bid_);
                break;
            case OrderDirection_t::SELL:
                rc = ModifyBook(order, security.ask_);
                break;
            default:
                rc = ERR_ORD_DIR;
                std::cerr<<"Unhandled OrderDirection "<<static_cast<std::underlying_type<OrderDirection_t>::type>(order.OrderDirection_)<<'\n'; 
                break;
        }

        return rc;      
    }

    int Implementation::Delete(const Order & order, Security & security) {
        int rc = SUCCESS;

        switch (order.OrderDirection_) {
            case OrderDirection_t::BUY:
                rc = DeleteFromBook(order, security.bid_);
                break;
            case OrderDirection_t::SELL:
                rc = DeleteFromBook(order, security.ask_);
                break;
            default:
                rc = ERR_ORD_DIR;
                std::cerr<<"Unhandled OrderDirection "<<static_cast<std::underlying_type<OrderDirection_t>::type>(order.OrderDirection_)<<'\n'; 
                break;
        }

        return rc;      
    }

} // namespace orderbook
