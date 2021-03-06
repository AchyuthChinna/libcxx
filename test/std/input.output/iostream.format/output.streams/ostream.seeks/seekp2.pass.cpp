//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <ostream>

// template <class charT, class traits = char_traits<charT> >
//   class basic_ostream;

// basic_ostream<charT,traits>& seekp(off_type off, ios_base::seekdir dir);

#include <ostream>
#include <cassert>

int seekoff_called = 0;

template <class CharT>
struct testbuf
    : public std::basic_streambuf<CharT>
{
    typedef std::basic_streambuf<CharT> base;
    testbuf() {}

protected:

    typename base::pos_type
    seekoff(typename base::off_type off, std::ios_base::seekdir way,
                                         std::ios_base::openmode which)
    {
        ++seekoff_called;
        assert(way == std::ios_base::beg);
        assert(which == std::ios_base::out);
        return off;
    }
};

int main(int, char**)
{
    {
        seekoff_called = 0;
        std::ostream os((std::streambuf*)0);
        assert(&os.seekp(5, std::ios_base::beg) == &os);
        assert(seekoff_called == 0);
    }
    {
        seekoff_called = 0;
        testbuf<char> sb;
        std::ostream os(&sb);
        assert(&os.seekp(10, std::ios_base::beg) == &os);
        assert(seekoff_called == 1);
        assert(os.good());
        assert(&os.seekp(-1, std::ios_base::beg) == &os);
        assert(seekoff_called == 2);
        assert(os.fail());
    }
    { // See https://bugs.llvm.org/show_bug.cgi?id=21361
        seekoff_called = 0;
        testbuf<char> sb;
        std::ostream os(&sb);
        os.setstate(std::ios_base::eofbit);
        assert(&os.seekp(10, std::ios_base::beg) == &os);
        assert(seekoff_called == 1);
        assert(os.rdstate() == std::ios_base::eofbit);
    }

  return 0;
}
