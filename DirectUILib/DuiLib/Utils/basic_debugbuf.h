#ifndef BASIC_DEBUGBUF_H
#define BASIC_DEBUGBUF_H
#include <sstream>

namespace dbg
{

template<class CharT, class TraitsT = std::char_traits<CharT> >
class basic_dostream : 
    public std::basic_ostream<CharT, TraitsT>
{
public:
    basic_dostream();
    ~basic_dostream();
};

typedef basic_dostream<char>    dostream;
typedef basic_dostream<wchar_t> wdostream;
extern dostream cout;
extern wdostream wcout;

}
#endif//BASIC_DEBUGBUF_H