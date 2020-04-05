/*
 * Parse and load a MIME message read from standard input and print 
 * its structure
 */
#include <iostream>
#include "../src/IMAP/Message.hpp"

int main()
{
    std::ios_base::sync_with_stdio(false);        // optimization
    IMAPProvider::Message mesg(std::cin, 0, "1/10/2020", {"\\Seen", "\\Recent"});
    std::cout << "(ENVELOPE " << mesg.envelope() << ")" << std::endl;
    std::cout << std::endl;
    std::cout << "(BODYSTRUCTURE " << mesg.bodyStructure() << ")" << std::endl;
    std::cout << std::endl;
    std::cout << "(BODY " << mesg.body() << ")" << std::endl;
    std::cout << mesg.body("Header.Fields (lISt-unsubscribe)", 0) << std::endl;

    return 0;
}