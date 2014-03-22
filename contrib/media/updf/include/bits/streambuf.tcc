// Stream buffer classes -*- C++ -*-

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
// ISO C++ 14882: 27.5  Stream buffers
//

#ifndef _CPP_BITS_STREAMBUF_TCC
#define _CPP_BITS_STREAMBUF_TCC 1

namespace std {

  template<typename _CharT, typename _Traits>
    basic_streambuf<_CharT, _Traits>::int_type
    basic_streambuf<_CharT, _Traits>::
    sbumpc()
    {
      int_type __ret;
      if (_M_in_cur && _M_in_cur < _M_in_end)
	{
	  char_type __c = *gptr();
	  _M_in_cur_move(1);
	  __ret = traits_type::to_int_type(__c);
	}
      else 
	__ret = this->uflow();
      return __ret;
    }

  template<typename _CharT, typename _Traits>
    basic_streambuf<_CharT, _Traits>::int_type
    basic_streambuf<_CharT, _Traits>::
    sputbackc(char_type __c) 
    {
      int_type __ret;
      bool __testpos = _M_in_cur && _M_in_beg < _M_in_cur;
      bool __testne = _M_in_cur && !traits_type::eq(__c, this->gptr()[-1]);
      if (!__testpos || __testne)
	__ret = pbackfail(traits_type::to_int_type(__c));
      else 
	{
	  _M_in_cur_move(-1);
	  __ret = traits_type::to_int_type(*this->gptr());
	}
      return __ret;
    }
  
  template<typename _CharT, typename _Traits>
    basic_streambuf<_CharT, _Traits>::int_type
    basic_streambuf<_CharT, _Traits>::
    sungetc()
    {
      int_type __ret;
      if (_M_in_cur && _M_in_beg < _M_in_cur)
	{
	  _M_in_cur_move(-1);
	  __ret = traits_type::to_int_type(*_M_in_cur);
	}
      else 
	__ret = this->pbackfail();
      return __ret;
    }

  // Don't test against _M_buf + _M_buf_size, because _M_buf reflects
  // allocated space, and on certain (rare but entirely legal)
  // situations, there will be no allocated space yet the internal
  // buffers will still be valid. (This happens if setp is used to set
  // the internal buffer to say some externally-allocated sequence.)
  template<typename _CharT, typename _Traits>
    basic_streambuf<_CharT, _Traits>::int_type
    basic_streambuf<_CharT, _Traits>::
    sputc(char_type __c)
    {
      int_type __ret;
      if (_M_out_buf_size())
	{
	  *_M_out_cur = __c;
	  _M_out_cur_move(1);
	  __ret = traits_type::to_int_type(__c);
	}
      else
	__ret = this->overflow(traits_type::to_int_type(__c));
      return __ret;
    }

  template<typename _CharT, typename _Traits>
    streamsize
    basic_streambuf<_CharT, _Traits>::
    xsgetn(char_type* __s, streamsize __n)
    {
      streamsize __ret = 0;
      while (__ret < __n)
	{
	  size_t __buf_len = _M_in_end - _M_in_cur;
	  if (__buf_len > 0)
	    {
	      size_t __remaining = __n - __ret;
	      size_t __len = min(__buf_len, __remaining);
	      traits_type::copy(__s, _M_in_cur, __len);
	      __ret += __len;
	      __s += __len;
	      _M_in_cur_move(__len);
	    }
	  
	  if (__ret < __n)
	    {
	      int_type __c = this->uflow();  
	      if (__c != traits_type::eof())
		{
		  traits_type::assign(*__s++, traits_type::to_char_type(__c));
		  ++__ret;
		}
	      else
		break;
	    }
	}
      return __ret;
    }

  // Don't test against _M_buf + _M_buf_size, because _M_buf reflects
  // allocated space, and on certain (rare but entirely legal)
  // situations, there will be no allocated space yet the internal
  // buffers will still be valid. (This happens if setp is used to set
  // the internal buffer to say some externally-allocated sequence.)
  template<typename _CharT, typename _Traits>
    streamsize
    basic_streambuf<_CharT, _Traits>::
    xsputn(const char_type* __s, streamsize __n)
    {
      streamsize __ret = 0;
      while (__ret < __n)
	{
	  off_type __buf_len = _M_out_buf_size();
	  if (__buf_len > 0)
	    {
	      off_type __remaining = __n - __ret;
	      off_type __len = min(__buf_len, __remaining);
	      traits_type::copy(_M_out_cur, __s, __len);
	      __ret += __len;
	      __s += __len;
	      _M_out_cur_move(__len);
	    }

	  if (__ret < __n)
	    {
	      int_type __c = this->overflow(traits_type::to_int_type(*__s));
	      if (__c != traits_type::eof())
		{
		  ++__ret;
		  ++__s;
		}
	      else
		break;
	    }
	}
      return __ret;
    }

  // Conceivably, this could be used to implement buffer-to-buffer
  // copies, if this was ever desired in an un-ambiguous way by the
  // standard. If so, then checks for __ios being zero would be
  // necessary.
  template<typename _CharT, typename _Traits>
    streamsize
    __copy_streambufs(basic_ios<_CharT, _Traits>& __ios,
		      basic_streambuf<_CharT, _Traits>* __sbin,
		      basic_streambuf<_CharT, _Traits>* __sbout) 
  {
      typedef typename _Traits::int_type	int_type;

      streamsize __ret = 0;
      streamsize __bufsize = __sbin->in_avail();
      streamsize __xtrct;
      bool __testput = __sbout->_M_mode & ios_base::out;
      try {
	while (__testput && __bufsize != -1)
	  {
	    __xtrct = __sbout->sputn(__sbin->gptr(), __bufsize);
	    __ret += __xtrct;
	    __sbin->_M_in_cur_move(__xtrct);
	    if (__xtrct == __bufsize)
	      {
		int_type __c = __sbin->sgetc();
		if (__c == _Traits::eof())
		  {
		    __ios.setstate(ios_base::eofbit);
		    break;
		  }
		__bufsize = __sbin->in_avail();
	      }
	    else
	      break;
	  }
      }
      catch(exception& __fail) {
	if ((__ios.exceptions() & ios_base::failbit) != 0)
	  __throw_exception_again;
      }
      return __ret;
    }
} // namespace std

#endif // _CPP_BITS_STREAMBUF_TCC

