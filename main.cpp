#include <unistd.h>

#include "datafeed.hpp"
#include "orderbook.hpp"
#include "compact.hpp"

int main(int argc, char *argv[])
{
    int opt;
    bool printopt = false;
    bool testopt = false;
    bool sizeopt = false;
    std::string symbol;

    while ((opt = getopt(argc, argv, "p:tsh")) != -1) {
        switch (opt) {
            case 'p':
                printopt = true;
                symbol = optarg;
            break;
            case 't':
                testopt = true;
            break;
            case 's':
                sizeopt = true;
            break;
            case 'h':
            default:
                std::cout<<argv[0]<<" -t (test) -s (sizecomparison) -p (printbook) <symbol> "<<'\n';
            break;
        }
    }

    orderbook::Implementation book;

    // Loop over each order, processing in turn
    for (auto & order : orderbook::datafeed) 
    {
        book.Process(order);
    }

    if(printopt) {
        book.PrintBook(symbol);
    }

    if(testopt) {
        TestBook(book);
    }

    if(sizeopt) {
        auto defaultsize = sizeof(orderbook::Order);
        auto compactsize = sizeof(compact::Order);
        float saving = static_cast<float>(compactsize) / static_cast<float>(defaultsize);
        std::cout<<"Size of default order: "<<defaultsize<<'\n';
        std::cout<<"Size of compact order: "<<compactsize<<'\n';
        std::cout<<"Space saving of "<<(100.0 - saving * 100)<<"%"<<'\n';
    }

   return 0;
}
