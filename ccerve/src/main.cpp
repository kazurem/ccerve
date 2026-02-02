/**
 * @file main.cpp
 * @brief Entrypoint of the program.
 * HttpServer instance is created within the main function.
 */

#include "http_server.hpp"
using namespace std;


/*
TODO:

- if no index.html exists, look for any html file and serve it (DONE)

- adding comments (it's a ongoing journey... and it will remain that way)

- Learn more about HTTP requests and responses

- dip your toes into asynchronous IO

- add a command line argument of where you want to run the executable
[http-server {path} {ipaddress} {port}]

- fix the empty svg response

- create a map of values in server_config.txt and then use that to do assigning

- Display response code in logs too (DONE)

- If only directory is requested in HTTP request, make a HTML page listing all
the directory contents (micmicing the behavior of python's http server module)

! NOTE: The code is still uncomplete, unoptimized with possible (most certainly)
! tremendous amount of bugs
*/

int main (int argc, char* argv[]) {
    // default values
    std::string ip_address = "127.0.0.1";
    int port               = 8000;

    try {
        if (argc == 3) // if ip_address and port both are specified in the
                       // command (e.g ./cerve 0.0.0.0 10000)
        {
            ip_address = argv[1];
            port = std::stoi (argv[2]); // this can throw a invalid_argument exception
            if (port > 65535 || port < 0) {
                std::cerr << "Port number must be in range 0-65535" << std::endl;
                exit (EXIT_FAILURE);
            }
        } else if (argc != 1) // 2, 4, 5 ... arguments
        {
            std::cerr << "Usage: " << argv[0] << " [ip_address port]" << std::endl;
            exit (EXIT_FAILURE);
        }

        ccerve::HttpServer server (ip_address, port); // this can throw server specific exceptions
        server.startListeningSession ();
    } catch (const std::invalid_argument& excpt) // for non-integer ports (e.g ./cerve 127.0.0.1 hello)
    {
        std::cerr << "Invalid port number: " << argv[2] << std::endl;
        exit (EXIT_FAILURE);
    } catch (const ccerve::exception::ServerSockCreationFailure& excpt) {
        std::cerr << excpt.what () << "\n";
        exit (EXIT_FAILURE);
    } catch (const ccerve::exception::ServerSockBindFailure& excpt) {
        std::cerr << excpt.what () << "\n";
        exit (EXIT_FAILURE);
    }

    return 0;
}
