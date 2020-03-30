/*
 * Parse and load a MIME message read from standard input and print 
 * its structure
 */
#include <iostream>
#include <mimetic/mimetic.h>

using namespace std;
using namespace mimetic;

void printMimeStructure(MimeEntity* pMe, int tabcount = 0)
{
    Header& h = pMe->header();                   // get header object
    for(int c = tabcount; c > 0; --c)            // indent nested entities
        cout << "    ";                      //
    cout << h.contentType() << endl;             // prints Content-Type
    MimeEntityList& parts = pMe->body().parts(); // list of sub entities obj
    // cycle on sub entities list and print info of every item
    MimeEntityList::iterator mbit = parts.begin(), meit = parts.end();
    for(; mbit != meit; ++mbit)
        printMimeStructure(*mbit, 1 + tabcount);
}

int main()
{
    ios_base::sync_with_stdio(false);        // optimization
    istreambuf_iterator<char> bit(cin), eit; // get stdin iterators
    MimeEntity me(bit,eit);                       // parse and load message
    printMimeStructure(&me);                      // print msg structure
    return 0;
}