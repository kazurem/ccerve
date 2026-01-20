#include "http_server.h"
#include "logger.h"

using namespace std;

/*
TODO: 
URGENT: Refactor(better code structure and design)

AFTER:
1.if no index.html exists, look for any html file and serve it
2. adding comments
3. modify README.md
4. Learn more about HTTP requests and responses
5. dip your toes into asynchronous IO
6. add a command line argument of where you want to run the executable [http-server {path} {ipaddress} {port}]
7. fix the empty svg response
8. create a map of values in server_config.txt and then use that to do assigning
9. When accessing a non available file, sometimes I get gibberish data instead of not found page
- Make a dedicated not found page [optional]
-I encountered a "socket was not able to send data" error.I couldnt replicate it.
*/

int main(int argc, char *argv[])
{
    //default values
    std::string ip_address = "127.0.0.1";
    int port = 8000;

    if (argc == 3)
    {
        ip_address = argv[1];
        try
        {
            port = std::stoi(argv[2]);
            if (port > 65535)
            {
                std::cerr << "Port number must be less than 65,535" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "Invalid port number: " << argv[2] << std::endl;
            exit(EXIT_FAILURE);
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << "Port number out of range for type int. : " << argv[2] << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else if (argc != 1)
    {
        std::cerr << "Usage: " << argv[0] << " [ip_address port]" << std::endl;
        exit(EXIT_FAILURE);
    }

    http::HTTPServer server(ip_address, port);
    server.startListeningSession();
    return 0;
}