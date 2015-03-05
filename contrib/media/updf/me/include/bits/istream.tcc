// Copyright (C) 1997, 1998, 1999, 2000, 2001 Free Software Foundation, Inc.
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
// ISO C++ 14882: 27.6.2  Output streams
//

#include <bits/std_locale.h>
#include <bits/std_ostream.h> // for flush()

namespace std 
{
  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>::sentry::
    sentry(basic_istream<_CharT, _Traits>& __in, bool __noskipws)
    {
      if (__in.good()) 
	{
	  if (__in.tie())
	    __in.tie()->flush();
	  if (!__noskipws && (__in.flags() & ios_base::skipws))
	    {	  
	      const __int_type __eof = traits_type::eof();
	      const __ctype_type* __ctype = __in._M_get_fctype_ios();
	      __streambuf_type* __sb = __in.rdbuf();
	      __int_type __c = __sb->sgetc();
	      
	      while (__c != __eof && __ctype->is(ctype_base::space, __c))
		__c = __sb->snextc();

#ifdef _GLIBCPP_RESOLVE_LIB_DEFECTS
//195.  Should basic_istream::sentry's constructor ever set eofbit? 
	      if (__c == __eof)
		__in.setstate(ios_base::eofbit);
#endif
	    }
	}

      if (__in.good())
	_M_ok = true;
      else
	{
	  _M_ok = false;
	  __in.setstate(ios_base::failbit);
	}
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(__istream_type& (*__pf)(__istream_type&))
    {
      __pf(*this);
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(__ios_type& (*__pf)(__ios_type&))
    {
      __pf(*this);
      return *this;
    }
  
  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(ios_base& (*__pf)(ios_base&))
    {
      __pf(*this);
      return *this;
    }
  
  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(bool& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(short& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(unsigned short& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(int& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(unsigned int& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(long& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(unsigned long& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

#ifdef _GLIBCPP_USE_LONG_LONG
  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(long long& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
	      __throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(unsigned long long& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }
#endif

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(float& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(double& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(long double& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(void*& __n)
    {
      sentry __cerb(*this, false);
      if (__cerb) 
	{
	  try 
	    {
	      ios_base::iostate __err = ios_base::iostate(ios_base::goodbit);
	      if (_M_check_facet(_M_fnumget))
		_M_fnumget->get(*this, 0, *this, __err, __n);
	      this->setstate(__err);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>& 
    basic_istream<_CharT, _Traits>::
    operator>>(__streambuf_type* __sbout)
    {
      streamsize __xtrct = 0;
      __streambuf_type* __sbin = this->rdbuf();
      sentry __cerb(*this, false);
      if (__sbout && __cerb)
	__xtrct = __copy_streambufs(*this, __sbin, __sbout);
      if (!__sbout || !__xtrct)
	this->setstate(ios_base::failbit);
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>::int_type
    basic_istream<_CharT, _Traits>::
    get(void)
    {
      const int_type __eof = traits_type::eof();
      int_type __c = __eof;
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb) 
	{
	  try 
	    {
	      __c = this->rdbuf()->sbumpc();
	      // 27.6.1.1 paragraph 3
	      if (__c != __eof)
		_M_gcount = 1;
	      else
		this->setstate(ios_base::eofbit | ios_base::failbit);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.3 paragraph 1
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return __c;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    basic_istream<_CharT, _Traits>::
    get(char_type& __c)
    {
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb) 
	{
 	  try 
	    {
	      const int_type __eof = traits_type::eof();
	      int_type __bufval = this->rdbuf()->sbumpc();
	      // 27.6.1.1 paragraph 3
	      if (__bufval != __eof)
		{
		  _M_gcount = 1;
		  __c = traits_type::to_char_type(__bufval);
		}
	      else
		this->setstate(ios_base::eofbit | ios_base::failbit);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.3 paragraph 1
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    basic_istream<_CharT, _Traits>::
    get(char_type* __s, streamsize __n, char_type __delim)
    {
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb && __n > 1) 
	{
	  try 
	    {
	      const int_type __idelim = traits_type::to_int_type(__delim);
	      const int_type __eof = traits_type::eof();
	      __streambuf_type* __sb = this->rdbuf();
	      int_type __c = __sb->sbumpc();	
	      bool __testdelim = __c == __idelim;
	      bool __testeof =  __c == __eof;
	      
	      while (_M_gcount < __n - 1 && !__testeof && !__testdelim)
		{
		  *__s++ = traits_type::to_char_type(__c);
		  ++_M_gcount;
		  __c = __sb->sbumpc();
		  __testeof = __c == __eof;
		  __testdelim = __c == __idelim;
		}
	      if (__testdelim || _M_gcount == __n - 1)
		__sb->sputbackc(__c);
	      if (__testeof)
		this->setstate(ios_base::eofbit);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.3 paragraph 1
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      *__s = char_type();
      if (!_M_gcount)
	this->setstate(ios_base::failbit);
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    basic_istream<_CharT, _Traits>::
    get(__streambuf_type& __sb, char_type __delim)
    {
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb) 
	{
	  int_type __c;
	  __streambuf_type* __this_sb = this->rdbuf();
	  try 
	    {
	      const int_type __idelim = traits_type::to_int_type(__delim);
	      const int_type __eof = traits_type::eof();	      
	      __c = __this_sb->sbumpc();
	      bool __testdelim = __c == __idelim;
	      bool __testeof =  __c == __eof;
	      bool __testput = true;
	      
	      while (!__testeof && !__testdelim 
		    && (__testput = __sb.sputc(traits_type::to_char_type(__c)) 
			 != __eof))
		{
		  ++_M_gcount;
		  __c = __this_sb->sbumpc();
		  __testeof = __c == __eof;
		  __testdelim = __c == __idelim;
		}
	      if (__testdelim || !__testput)
		__this_sb->sputbackc(traits_type::to_char_type(__c));
	      if (__testeof)
		this->setstate(ios_base::eofbit);
	    }
	  catch(exception& __fail)
	    {
	      // Exception may result from sputc->overflow.
	      __this_sb->sputbackc(traits_type::to_char_type(__c));
	    }
	}
      if (!_M_gcount)
	this->setstate(ios_base::failbit);
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    basic_istream<_CharT, _Traits>::
    getline(char_type* __s, streamsize __n, char_type __delim)
    {
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb) 
	{
          try 
	    {
	      __streambuf_type* __sb = this->rdbuf();
	      int_type __c = __sb->sbumpc();
	      ++_M_gcount;
	      const int_type __idelim = traits_type::to_int_type(__delim);
	      const int_type __eof = traits_type::eof();
	      bool __testdelim = __c == __idelim;
	      bool __testeof =  __c == __eof;
	    
	      while (_M_gcount < __n && !__testeof && !__testdelim)
		{
		  *__s++ = traits_type::to_char_type(__c);
		  __c = __sb->sbumpc();
		  ++_M_gcount;
		  __testeof = __c == __eof;
		  __testdelim = __c == __idelim;
		}
	      
	      if (__testeof)
		{
		  --_M_gcount;
		  this->setstate(ios_base::eofbit);
		}
	      else if (!__testdelim)
		{
		  --_M_gcount;
		  __sb->sputbackc(traits_type::to_char_type(__c));
		  this->setstate(ios_base::failbit);
		}
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.3 paragraph 1
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      *__s = char_type();
      if (!_M_gcount)
	this->setstate(ios_base::failbit);
      return *this;
    }
  
  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    basic_istream<_CharT, _Traits>::
    ignore(streamsize __n, int_type __delim)
    {
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb && __n > 0) 
	{
	  try 
	    {
	      const int_type __idelim = traits_type::to_int_type(__delim);
	      const int_type __eof = traits_type::eof();
	      __streambuf_type* __sb = this->rdbuf();
	      int_type __c = __sb->sbumpc();	
	      bool __testdelim = __c == __idelim;
	      bool __testeof =  __c == __eof;
	      
	      __n = min(__n, numeric_limits<streamsize>::max());
	      while (_M_gcount < __n - 1 && !__testeof && !__testdelim)
		{
		  ++_M_gcount;
		  __c = __sb->sbumpc();
		  __testeof = __c == __eof;
		  __testdelim = __c == __idelim;
		}
	      if ((_M_gcount == __n - 1 && !__testeof) || __testdelim)
		++_M_gcount;
	      if (__testeof)
		this->setstate(ios_base::eofbit);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.3 paragraph 1
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }
  
  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>::int_type
    basic_istream<_CharT, _Traits>::
    peek(void)
    {
      int_type __c = traits_type::eof();
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb)
	{
	  try 
	    { __c = this->rdbuf()->sgetc(); }
	  catch(exception& __fail)
	    {
	      // 27.6.1.3 paragraph 1
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	} 
      return __c;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    basic_istream<_CharT, _Traits>::
    read(char_type* __s, streamsize __n)
    {
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb) 
	{
	  if (__n > 0)
	    {
	      try 
		{
		  const int_type __eof = traits_type::eof();
		  __streambuf_type* __sb = this->rdbuf();
		  int_type __c = __sb->sbumpc();	
		  bool __testeof =  __c == __eof;
		  
		  while (_M_gcount < __n - 1 && !__testeof)
		    {
		      *__s++ = traits_type::to_char_type(__c);
		      ++_M_gcount;
		      __c = __sb->sbumpc();
		      __testeof = __c == __eof;
		    }
		  if (__testeof)
		    this->setstate(ios_base::eofbit | ios_base::failbit);
		  else
		    {
		      // _M_gcount == __n - 1
		      *__s++ = traits_type::to_char_type(__c);
		      ++_M_gcount;
		    }	    
		}
	      catch(exception& __fail)
		{
		  // 27.6.1.3 paragraph 1
		  // Turn this on without causing an ios::failure to be thrown.
		  this->setstate(ios_base::badbit);
		  if ((this->exceptions() & ios_base::badbit) != 0)
		    __throw_exception_again;
		}
	    }
	}
      else
	this->setstate(ios_base::failbit);
      return *this;
    }
  
  template<typename _CharT, typename _Traits>
    streamsize 
    basic_istream<_CharT, _Traits>::
    readsome(char_type* __s, streamsize __n)
    {
      const int_type __eof = traits_type::eof();
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb) 
	{
	  if (__n > 0)
	    {
	      try 
		{
		  streamsize __num = this->rdbuf()->in_avail();
		  if (__num != static_cast<streamsize>(__eof))
		    {
		      __num = min(__num, __n);
		      _M_gcount = this->rdbuf()->sgetn(__s, __num);
		    }
		  else
		    this->setstate(ios_base::eofbit);		    
		}
	      catch(exception& __fail)
		{
		  // 27.6.1.3 paragraph 1
		  // Turn this on without causing an ios::failure to be thrown.
		  this->setstate(ios_base::badbit);
		  if ((this->exceptions() & ios_base::badbit) != 0)
		    __throw_exception_again;
		}
	    }
	}
      else
	this->setstate(ios_base::failbit);
      return _M_gcount;
    }
      
  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    basic_istream<_CharT, _Traits>::
    putback(char_type __c)
    {
      sentry __cerb(*this, true);
      if (__cerb) 
	{
	  try 
	    {
	      const int_type __eof = traits_type::eof();
	      __streambuf_type* __sb = this->rdbuf();
	      if (!__sb || __sb->sputbackc(__c) == __eof) 
		this->setstate(ios_base::badbit);		    
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.3 paragraph 1
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      else
	this->setstate(ios_base::failbit);
      return *this;
    }
  
  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    basic_istream<_CharT, _Traits>::
    unget(void)
    {
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb) 
	{
	  try 
	    {
	      const int_type __eof = traits_type::eof();
	      __streambuf_type* __sb = this->rdbuf();
	      if (!__sb || __eof == __sb->sungetc())
		this->setstate(ios_base::badbit);		    
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.3 paragraph 1
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      else
	this->setstate(ios_base::failbit);
      return *this;
    }
  
  template<typename _CharT, typename _Traits>
    int
    basic_istream<_CharT, _Traits>::
    sync(void)
    {
      int __ret = traits_type::eof();
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb) 
	{
	  try 
	    {
	      __streambuf_type* __sb = this->rdbuf();
	      if (!__sb || __ret == __sb->pubsync())
		this->setstate(ios_base::badbit);		    
	      else 
		__ret = 0;
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.3 paragraph 1
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return __ret;
    }
  
  template<typename _CharT, typename _Traits>
    typename basic_istream<_CharT, _Traits>::pos_type
    basic_istream<_CharT, _Traits>::
    tellg(void)
    {
      pos_type __ret = pos_type(-1);
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb) 
	{
	  try 
	    {
	     __ret = this->rdbuf()->pubseekoff(0, ios_base::cur, ios_base::in);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.3 paragraph 1
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return __ret;
    }


  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    basic_istream<_CharT, _Traits>::
    seekg(pos_type __pos)
    {
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb) 
	{
	  try 
	    {
#ifdef _GLIBCPP_RESOLVE_LIB_DEFECTS
// 136.  seekp, seekg setting wrong streams?
	      pos_type __err = this->rdbuf()->pubseekpos(__pos, ios_base::in);

// 129. Need error indication from seekp() and seekg()
	      if (__err == pos_type(off_type(-1)))
		this->setstate(failbit);
#endif
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.3 paragraph 1
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    basic_istream<_CharT, _Traits>::
    seekg(off_type __off, ios_base::seekdir __dir)
    {
      _M_gcount = 0;
      sentry __cerb(*this, true);
      if (__cerb) 
	{
	  try 
	    {
#ifdef _GLIBCPP_RESOLVE_LIB_DEFECTS
// 136.  seekp, seekg setting wrong streams?
	      pos_type __err = this->rdbuf()->pubseekoff(__off, __dir, 
							 ios_base::in);

// 129. Need error indication from seekp() and seekg()
	      if (__err == pos_type(off_type(-1)))
		this->setstate(failbit);
#endif
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.3 paragraph 1
	      // Turn this on without causing an ios::failure to be thrown.
	      this->setstate(ios_base::badbit);
	      if ((this->exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      return *this;
    }

  // 27.6.1.2.3 Character extraction templates
  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    operator>>(basic_istream<_CharT, _Traits>& __in, _CharT& __c)
    {
      typedef basic_istream<_CharT, _Traits> 		__istream_type;
      typename __istream_type::sentry __cerb(__in, false);
      if (__cerb)
	{
	  try 
	    { __in.get(__c); }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      __in.setstate(ios_base::badbit);
	      if ((__in.exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      else
	__in.setstate(ios_base::failbit);
      return __in;
    }

  template<typename _CharT, typename _Traits>
    basic_istream<_CharT, _Traits>&
    operator>>(basic_istream<_CharT, _Traits>& __in, _CharT* __s)
    {
      typedef basic_istream<_CharT, _Traits> 		__istream_type;
      typedef typename __istream_type::__streambuf_type __streambuf_type;
      typedef typename _Traits::int_type 		int_type;
      typedef _CharT                     		char_type;
      typedef ctype<_CharT>     			__ctype_type;
      streamsize __extracted = 0;

      typename __istream_type::sentry __cerb(__in, false);
      if (__cerb)
	{
	  try 
	    {
	      // Figure out how many characters to extract.
	      streamsize __num = __in.width();
	      if (__num == 0)
		__num = numeric_limits<streamsize>::max();
	      
	      __streambuf_type* __sb = __in.rdbuf();
	      const __ctype_type* __ctype = __in._M_get_fctype_ios();
	      int_type __c = __sb->sbumpc();
	      const int_type __eof = _Traits::eof();
	      bool __testsp = __ctype->is(ctype_base::space, __c);
	      bool __testeof =  __c == __eof;
	      
	      while (__extracted < __num - 1 && !__testeof && !__testsp)
		{
		  *__s++ = __c;
		  ++__extracted;
		  __c = __sb->sbumpc();
		  __testeof = __c == __eof;
		  __testsp = __ctype->is(ctype_base::space, __c);
		}
	      
	      if (!__testeof)
		__sb->sputbackc(__c);
	      else
		__in.setstate(ios_base::eofbit);

#ifdef _GLIBCPP_RESOLVE_LIB_DEFECTS
//68.  Extractors for char* should store null at end
	      *__s = char_type();
#endif
	      __in.width(0);
	    }
	  catch(exception& __fail)
	    {
	      // 27.6.1.2.1 Common requirements.
	      // Turn this on without causing an ios::failure to be thrown.
	      __in.setstate(ios_base::badbit);
	      if ((__in.exceptions() & ios_base::badbit) != 0)
		__throw_exception_again;
	    }
	}
      if (!__extracted)
	__in.setstate(ios_base::failbit);
      return __in;
    }

  // 27.6.1.4 Standard basic_istream manipulators
  template<typename _CharT, typename _Traits>
    basic_istream<_CharT,_Traits>& 
    ws(basic_istream<_CharT,_Traits>& __in)
    {
      typedef basic_istream<_CharT, _Traits> 		__istream_type;
      typedef typename __istream_type::__streambuf_type __streambuf_type;
      typedef typename __istream_type::__ctype_type 	__ctype_type;
      typedef typename __istream_type::int_type 	__int_type;
      typedef typename __istream_type::char_type 	__char_type;

      __streambuf_type* __sb = __in.rdbuf();
      const __ctype_type* __ctype = __in._M_get_fctype_ios();
      const __int_type __eof = _Traits::eof();	      
      __int_type __c;
      bool __testeof;
      bool __testsp;

      do 
	{
	  __c = __sb->sbumpc();
	  __testeof = __c == __eof;
	  __testsp = __ctype->is(ctype_base::space, __c);
	}
      while (!__testeof && __testsp);

      if (!__testeof && !__testsp)
	__sb->sputbackc(__c);
      else
	__in.setstate(ios_base::eofbit);

      return __in;
    }

  // 21.3.7.9 basic_string::getline and operators
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_istream<_CharT, _Traits>&
    operator>>(basic_istream<_CharT, _Traits>& __in,
	       basic_string<_CharT, _Traits, _Alloc>& __str)
    {
      typedef basic_istream<_CharT, _Traits> 		__istream_type;
      typedef typename __istream_type::int_type 	__int_type;
      typedef typename __istream_type::__streambuf_type __streambuf_type;
      typedef typename __istream_type::__ctype_type 	__ctype_type;
      typedef basic_string<_CharT, _Traits, _Alloc> 	__string_type;
      typedef typename __string_type::size_type		__size_type;
      __size_type __extracted = 0;

      typename __istream_type::sentry __cerb(__in, false);
      if (__cerb) 
	{
	  __str.erase();
	  streamsize __w = __in.width();
	  __size_type __n;
	  __n = __w > 0 ? static_cast<__size_type>(__w) : __str.max_size();

	  __streambuf_type* __sb = __in.rdbuf();
	  const __ctype_type* __ctype = __in._M_get_fctype_ios();
	  __int_type __c = __sb->sbumpc();
	  const __int_type __eof = _Traits::eof();
	  bool __testsp = __ctype->is(ctype_base::space, __c);
	  bool __testeof =  __c == __eof;

	  while (__extracted < __n && !__testeof && !__testsp)
	    {
	      __str += _Traits::to_char_type(__c);
	      ++__extracted;
	      __c = __sb->sbumpc();
	      __testeof = __c == __eof;
	      __testsp = __ctype->is(ctype_base::space, __c);
	    }
	  if (!__testeof)
	    __sb->sputbackc(__c);
	  else
	    __in.setstate(ios_base::eofbit);
	  __in.width(0);
	}
#ifdef _GLIBCPP_RESOLVE_LIB_DEFECTS
// 2000-02-01 Number to be determined
      if (!__extracted)
	__in.setstate (ios_base::failbit);
#endif
      return __in;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_istream<_CharT, _Traits>&
    getline(basic_istream<_CharT, _Traits>& __in,
	    basic_string<_CharT, _Traits, _Alloc>& __str, _CharT __delim)
    {
      typedef basic_istream<_CharT, _Traits> 		__istream_type;
      typedef typename __istream_type::int_type 	__int_type;
      typedef typename __istream_type::__streambuf_type __streambuf_type;
      typedef typename __istream_type::__ctype_type 	__ctype_type;
      typedef basic_string<_CharT, _Traits, _Alloc> 	__string_type;
      typedef typename __string_type::size_type		__size_type;

      __size_type __extracted = 0;
      bool __testdelim = false;
      typename __istream_type::sentry __cerb(__in, true);
      if (__cerb) 
	{
	  __str.erase();
	  __size_type __n = __str.max_size();

	  __int_type __idelim = _Traits::to_int_type(__delim);
	  __streambuf_type* __sb = __in.rdbuf();
	  __int_type __c = __sb->sbumpc();
	  const __int_type __eof = _Traits::eof();
	  __testdelim = __c ==  __idelim;
	  bool __testeof =  __c == __eof;

	  while (__extracted <= __n && !__testeof && !__testdelim)
	    {
	      __str += _Traits::to_char_type(__c);
	      ++__extracted;
	      __c = __sb->sbumpc();
	      __testeof = __c == __eof;
	      __testdelim = __c == __idelim;
	    }
	  if (__testeof)
	    __in.setstate(ios_base::eofbit);
	}
      if (!__extracted && !__testdelim)
	__in.setstate(ios_base::failbit);
      return __in;
    }

  template<class _CharT, class _Traits, class _Alloc>
    inline basic_istream<_CharT,_Traits>&
    getline(basic_istream<_CharT, _Traits>& __in, 
	    basic_string<_CharT,_Traits,_Alloc>& __str)
    { return getline(__in, __str, __in.widen('\n')); }
} // namespace std

// Local Variables:
// mode:C++
// End:

