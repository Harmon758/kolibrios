// Input streams -*- C++ -*-

// Copyright (C) 1997-1999, 2001 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

//
// ISO C++ 14882: 27.6.1  Input streams
//

#ifndef _CPP_ISTREAM
#define _CPP_ISTREAM	1

#pragma GCC system_header

#include <bits/std_ios.h>
#include <bits/std_limits.h> // For numeric_limits

namespace std
{
  // 27.6.1.1 Template class basic_istream
  template<typename _CharT, typename _Traits>
    class basic_istream : virtual public basic_ios<_CharT, _Traits>
    {
    public:
      // Types (inherited from basic_ios (27.4.4)):
      typedef _CharT                     		char_type;
      typedef typename _Traits::int_type 		int_type;
      typedef typename _Traits::pos_type 		pos_type;
      typedef typename _Traits::off_type 		off_type;
      typedef _Traits                    		traits_type;
      
      // Non-standard Types:
      typedef basic_streambuf<_CharT, _Traits> 		__streambuf_type;
      typedef basic_ios<_CharT, _Traits>		__ios_type;
      typedef basic_istream<_CharT, _Traits>		__istream_type;
      typedef istreambuf_iterator<_CharT, _Traits>	__istreambuf_iter;
      typedef num_get<_CharT, __istreambuf_iter>        __numget_type;
      typedef ctype<_CharT>           			__ctype_type;

    protected:
      // Data Members:
      streamsize 		_M_gcount;

    public:
      // 27.6.1.1.1 Constructor/destructor:
      explicit 
      basic_istream(__streambuf_type* __sb)
      { 
	this->init(__sb);
	_M_gcount = streamsize(0);
      }

      virtual 
      ~basic_istream() 
      { _M_gcount = streamsize(0); }

      // 27.6.1.1.2 Prefix/suffix:
      class sentry;
      friend class sentry;

      // 27.6.1.2 Formatted input:
      // 27.6.1.2.3 basic_istream::operator>>
      __istream_type&
      operator>>(__istream_type& (*__pf)(__istream_type&));

      __istream_type&
      operator>>(__ios_type& (*__pf)(__ios_type&));

      __istream_type&
      operator>>(ios_base& (*__pf)(ios_base&));
      
      // 27.6.1.2.2 Arithmetic Extractors
      __istream_type& 
      operator>>(bool& __n);
      
      __istream_type& 
      operator>>(short& __n);
      
      __istream_type& 
      operator>>(unsigned short& __n);

      __istream_type& 
      operator>>(int& __n);
      
      __istream_type& 
      operator>>(unsigned int& __n);

      __istream_type& 
      operator>>(long& __n);
      
      __istream_type& 
      operator>>(unsigned long& __n);

#ifdef _GLIBCPP_USE_LONG_LONG
      __istream_type& 
      operator>>(long long& __n);

      __istream_type& 
      operator>>(unsigned long long& __n);
#endif

      __istream_type& 
      operator>>(float& __f);

      __istream_type& 
      operator>>(double& __f);

      __istream_type& 
      operator>>(long double& __f);

      __istream_type& 
      operator>>(void*& __p);

      __istream_type& 
      operator>>(__streambuf_type* __sb);
      
      // 27.6.1.3 Unformatted input:
      inline streamsize 
      gcount(void) const 
      { return _M_gcount; }
      
      int_type 
      get(void);

      __istream_type& 
      get(char_type& __c);

      __istream_type& 
      get(char_type* __s, streamsize __n, char_type __delim);

      inline __istream_type& 
      get(char_type* __s, streamsize __n)
      { return get(__s, __n, this->widen('\n')); }

      __istream_type&
      get(__streambuf_type& __sb, char_type __delim);

      inline __istream_type&
      get(__streambuf_type& __sb)
      { return get(__sb, this->widen('\n')); }

      __istream_type& 
      getline(char_type* __s, streamsize __n, char_type __delim);

      inline __istream_type& 
      getline(char_type* __s, streamsize __n)
      { return getline(__s, __n, this->widen('\n')); }

      __istream_type& 
      ignore(streamsize __n = 1, int_type __delim = traits_type::eof());
      
      int_type 
      peek(void);
      
      __istream_type& 
      read(char_type* __s, streamsize __n);

      streamsize 
      readsome(char_type* __s, streamsize __n);
      
      __istream_type& 
      putback(char_type __c);

      __istream_type& 
      unget(void);

      int 
      sync(void);

      pos_type 
      tellg(void);

      __istream_type& 
      seekg(pos_type);

      __istream_type& 
      seekg(off_type, ios_base::seekdir);

    private:
#ifdef _GLIBCPP_RESOLVE_LIB_DEFECTS
      // Not defined.
      __istream_type& 
      operator=(const __istream_type&);

      basic_istream(const __istream_type&);
#endif
    };
  
  template<typename _CharT, typename _Traits>
    class basic_istream<_CharT, _Traits>::sentry
    {
    public:
      typedef _Traits 					traits_type;
      typedef basic_streambuf<_CharT, _Traits> 		__streambuf_type;
      typedef basic_istream<_CharT, _Traits> 		__istream_type;
      typedef __istream_type::__ctype_type 		__ctype_type;
      typedef typename _Traits::int_type		__int_type;

      explicit 
      sentry(basic_istream<_CharT, _Traits>& __is, bool __noskipws = false);

      operator bool() { return _M_ok; }

    private:
      bool _M_ok;
    };

  // 27.6.1.2.3 Character extraction templates
  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    operator>>(basic_istream<_CharT, _Traits>& __in, _CharT& __c);

  template<class _Traits>
    basic_istream<char, _Traits>&
    operator>>(basic_istream<char, _Traits>& __in, unsigned char& __c)
    { return (__in >> reinterpret_cast<char&>(__c)); }

  template<class _Traits>
    basic_istream<char, _Traits>&
    operator>>(basic_istream<char, _Traits>& __in, signed char& __c)
    { return (__in >> reinterpret_cast<char&>(__c)); }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    operator>>(basic_istream<_CharT, _Traits>& __in, _CharT* __s);
  
  template<class _Traits>
    basic_istream<char,_Traits>&
    operator>>(basic_istream<char,_Traits>& __in, unsigned char* __s)
    { return (__in >> reinterpret_cast<char*>(__s)); }

  template<class _Traits>
    basic_istream<char,_Traits>&
    operator>>(basic_istream<char,_Traits>& __in, signed char* __s)
    { return (__in >> reinterpret_cast<char*>(__s)); }

  // 27.6.1.5 Template class basic_iostream
  template<typename _CharT, typename _Traits>
    class basic_iostream
    : public basic_istream<_CharT, _Traits>,
      public basic_ostream<_CharT, _Traits>
    {
    public:
      // Non-standard Types:
      typedef basic_istream<_CharT, _Traits>		__istream_type;
      typedef basic_ostream<_CharT, _Traits>		__ostream_type;

      explicit 
      basic_iostream(basic_streambuf<_CharT, _Traits>* __sb)
      : __istream_type(__sb), __ostream_type(__sb)
      { }

      virtual 
      ~basic_iostream() { }
    };

  // 27.6.1.4 Standard basic_istream manipulators
  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    ws(basic_istream<_CharT, _Traits>& __is);
} // namespace std

#ifdef _GLIBCPP_NO_TEMPLATE_EXPORT
# define export
#ifdef  _GLIBCPP_FULLY_COMPLIANT_HEADERS
# include <bits/istream.tcc>
#endif
#endif

#endif	/* _CPP_ISTREAM */

