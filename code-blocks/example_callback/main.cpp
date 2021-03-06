#include <iostream>
#include <mt-bridge.hpp>
#include <xtime.hpp>

int main() {
    const uint32_t port = 5555;
    mt_bridge::MtBridge iMT(
            port,
            10,
            [&](const std::map<std::string, mt_bridge::MtCandle> &candles,
                const mt_bridge::MtBridge::EventType event,
                const uint64_t timestamp) {
        mt_bridge::MtCandle candle = mt_bridge::MtBridge::get_candle("EURUSD", candles);
        std::cout << "utc: " << xtime::get_str_date_time() << std::endl;
        std::cout << "server utc: " << xtime::get_str_date_time(iMT.get_server_ftimestamp()) << std::endl;
        std::cout << "server raw: " << xtime::get_str_date_time(iMT.get_raw_server_timestamp()) << std::endl;
        std::cout << "offset: " << iMT.get_offset_timestamp() << std::endl;

        switch(event) {
        case mt_bridge::MtBridge::EventType::HISTORICAL_DATA_RECEIVED:
            std::cout << "history bar: " << xtime::get_str_date_time(timestamp) << " minute day: " << ((timestamp / 60) % 1440) << std::endl;
            if(mt_bridge::MtBridge::check_candle(candle)) {
                std::cout << "EURUSD, close: " << candle.close
                    << " volume: " << candle.volume
                    << " t: " << xtime::get_str_date_time(candle.timestamp) << std::endl;
            } else {
                std::cout << "EURUSD, error"<< std::endl;
            }
            break;
        case mt_bridge::MtBridge::EventType::NEW_TICK:
            std::cout << "new tick: " << xtime::get_str_date_time(timestamp) << std::endl;
            if(mt_bridge::MtBridge::check_candle(candle)) {
                std::cout << "EURUSD, close: " << candle.close
                    << " volume: " << candle.volume
                    << " t: " << xtime::get_str_date_time(candle.timestamp) << std::endl;
            } else {
                std::cout << "EURUSD, error"<< std::endl;
            }
            break;
        };

        /* просто проверяем */
        std::map<std::string, mt_bridge::MtCandle> candles_2 = iMT.get_candles(timestamp);
        mt_bridge::MtCandle candle_2 = mt_bridge::MtBridge::get_candle("EURUSD", candles_2);
        std::cout << "new candle: " << xtime::get_str_date_time(timestamp) << std::endl;
        if(mt_bridge::MtBridge::check_candle(candle_2)) {
            std::cout << "EURUSD, close: " << candle_2.close
                << " volume: " << candle_2.volume
                << " t: " << xtime::get_str_date_time(candle_2.timestamp) << std::endl;
        } else {
            std::cout << "EURUSD, error (candle_2)"<< std::endl;
        }
    });

    if(!iMT.wait()) {
        std::cout << "no connection" << std::endl;
        return 0;
    }
    std::cout << "connection established" << std::endl;

#if(0)
    /* list all symbols */
    std::vector<std::string> symbol_list = iMT.get_symbol_list();
    std::for_each(symbol_list.begin(), symbol_list.end(), [&](std::string &symbol) {
        static int n = 0;
        std::cout
            << "symbol["
            << std::to_string(n++)
            << "]: "
            << symbol
            << std::endl;
    });
    std::cout << "mt-bridge version: " << iMT.get_mt_bridge_version() << std::endl;
#endif

    /* stop */
    while(true) {
        std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}
