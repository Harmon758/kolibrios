// basic_ios locale and locale-related member functions -*- C++ -*-

// Copyright (C) 1999, 2001 Free Software Foundation, Inc.
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

#ifndef _CPP_BITS_BASICIOS_TCC
#define _CPP_BITS_BASICIOS_TCC 1

namespace std
{
  template<typename _CharT, typename _Traits>
    basic_streambuf<_CharT, _Traits>* 
    basic_ios<_CharT, _Traits>::rdbuf(basic_streambuf<_CharT, _Traits>* __sb)
    {
      basic_streambuf<_CharT, _Traits>* __old = _M_streambuf;
      _M_streambuf = __sb;
      this->clear();
      return __old;
    }

  template<typename _CharT, typename _Traits>
    basic_ios<_CharT, _Traits>&
    basic_ios<_CharT, _Traits>::copyfmt(const basic_ios& __rhs)
    {
      // Per 27.1.1.1, do not call imbue, yet must trash all caches
      // associated with imbue()

      // Alloc any new word array first, so if it fails we have "rollback".
      _Words* __words = (__rhs._M_word_limit <= _S_local_words) ?
	_M_word_array : new _Words[__rhs._M_word_limit];

      // XXX This is the only reason _Callback_list was defined
      // inline. The suspicion is that this increased compilation
      // times dramatically for functions that use this member
      // function (inserters_extractors, ios_manip_fmtflags). FIX ME,
      // clean this stuff up. Callbacks are broken right now, anyway.

      // Bump refs before doing callbacks, for safety.
      _Callback_list* __cb = __rhs._M_callbacks;
      if (__cb) 
	__cb->_M_add_reference();
      _M_call_callbacks(erase_event);
      if (_M_words != _M_word_array) 
	delete [] _M_words;
      _M_dispose_callbacks();

      _M_callbacks = __cb;  // NB: Don't want any added during above.
      for (int __i = 0; __i < __rhs._M_word_limit; ++__i)
	__words[__i] = __rhs._M_words[__i];
      if (_M_words != _M_word_array) 
	delete [] _M_words;
      _M_words = __words;
      _M_word_limit = __rhs._M_word_limit;

      this->flags(__rhs.flags());
      this->width(__rhs.width());
      this->precision(__rhs.precision());
      this->tie(__rhs.tie());
      this->fill(__rhs.fill());
      // The next is required to be the last assignment.
      this->exceptions(__rhs.exceptions());
      
      _M_call_callbacks(copyfmt_event);
      return *this;
    }

  template<typename _CharT, typename _Traits>
    char
    basic_ios<_CharT, _Traits>::narrow(char_type __c, char __dfault) const
    { return _M_ios_fctype->narrow(__c, __dfault); }

  template<typename _CharT, typename _Traits>
    _CharT
    basic_ios<_CharT, _Traits>::widen(char __c) const
    { return _M_ios_fctype->widen(__c); }

  // Locales:
  template<typename _CharT, typename _Traits>
    locale
    basic_ios<_CharT, _Traits>::imbue(const locale& __loc)
    {
      locale __old(this->getloc());
      ios_base::imbue(__loc);
      _M_cache_facets(__loc);
      if (this->rdbuf() != 0)
	this->rdbuf()->pubimbue(__loc);
      return __old;
    }

  template<typename _CharT, typename _Traits>
    void
    basic_ios<_CharT, _Traits>::init(basic_streambuf<_CharT, _Traits>* __sb)
    {
      // NB: This may be called more than once on the same object.
      ios_base::_M_init();
      _M_cache_facets(_M_ios_locale);
      _M_tie = 0;
      _M_fill = this->widen(' ');
      _M_exception = goodbit;
      _M_streambuf = __sb;
      _M_streambuf_state = __sb ? goodbit : badbit;
    }

  template<typename _CharT, typename _Traits>
    void
    basic_ios<_CharT, _Traits>::_M_cache_facets(const locale& __loc)
    {
      if (has_facet<__ctype_type>(__loc))
	_M_ios_fctype = &use_facet<__ctype_type>(__loc);
      // Should be filled in by ostream and istream, respectively.
      if (has_facet<__numput_type>(__loc))
	_M_fnumput = &use_facet<__numput_type>(__loc); 
      if (has_facet<__numget_type>(__loc))
	_M_fnumget = &use_facet<__numget_type>(__loc); 
    }
} // namespace std

#endif // _CPP_BITS_BASICIOS_TCC



