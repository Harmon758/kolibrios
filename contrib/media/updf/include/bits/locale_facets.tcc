// Locale support -*- C++ -*-

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

// Warning: this file is not meant for user inclusion.  Use <locale>.

#ifndef _CPP_BITS_LOCFACETS_TCC
#define _CPP_BITS_LOCFACETS_TCC 1

#include <bits/std_cerrno.h>
#include <bits/std_clocale.h>   // For localeconv
#include <bits/std_cstdlib.h>   // For strof, strtold
#include <bits/std_limits.h>    // For numeric_limits
#include <bits/std_memory.h>    // For auto_ptr
#include <bits/sbuf_iter.h>     // For streambuf_iterators
#include <bits/std_cctype.h>    // For isspace
#include <typeinfo> 		// For bad_cast
#include <bits/std_vector.h>	


namespace std
{
  template<typename _Facet>
    locale
    locale::combine(const locale& __other)
    {
      locale __copy(*this);
      __copy._M_impl->_M_replace_facet(__other._M_impl, &_Facet::id);
      return __copy;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    bool
    locale::operator()(const basic_string<_CharT, _Traits, _Alloc>& __s1,
                       const basic_string<_CharT, _Traits, _Alloc>& __s2) const
    {
      typedef std::collate<_CharT> __collate_type;
      const __collate_type* __fcoll = &use_facet<__collate_type>(*this);
      return (__fcoll->compare(__s1.data(), __s1.data() + __s1.length(),
                               __s2.data(), __s2.data() + __s2.length()) < 0);
    }

  template<typename _Facet>
    const _Facet&
    use_facet(const locale& __loc)
    {
      typedef locale::_Impl::__vec_facet        __vec_facet;
      size_t __i = _Facet::id._M_index;
      __vec_facet* __facet = __loc._M_impl->_M_facets;
      const locale::facet* __fp = (*__facet)[__i]; 
      if (__fp == 0 || __i >= __facet->size())
        __throw_bad_cast();
      return static_cast<const _Facet&>(*__fp);
    }

  template<typename _Facet>
    bool
    has_facet(const locale& __loc) throw()
    {
      typedef locale::_Impl::__vec_facet        __vec_facet;
      size_t __i = _Facet::id._M_index;
      __vec_facet* __facet = __loc._M_impl->_M_facets;
      return (__i < __facet->size() && (*__facet)[__i] != 0);
    }

  // __match_parallel
  // matches input __s against a set of __ntargs strings in __targets,
  // placing in __matches a vector of indices into __targets which
  // match, and in __remain the number of such matches. If it hits
  // end of sequence before it minimizes the set, sets __eof.
  // Empty strings are never matched.
  template<typename _InIter, typename _CharT>
    _InIter
    __match_parallel(_InIter __s, _InIter __end, int __ntargs,
                     const basic_string<_CharT>* __targets,
                     int* __matches, int& __remain, bool& __eof)
    {
      typedef basic_string<_CharT> __string_type;
      __eof = false;
      for (int __ti = 0; __ti < __ntargs; ++__ti)
        __matches[__ti] = __ti;
      __remain = __ntargs;
      size_t __pos = 0;
      do
        {
	  int __ti = 0;
	  while (__ti < __remain && __pos == __targets[__matches[__ti]].size())
	    ++__ti;
	  if (__ti == __remain)
	    {
	      if (__pos == 0) __remain = 0;
	      return __s;
	    }
          if (__s == __end)
            __eof = true;
          bool __matched = false;
          for (int __ti2 = 0; __ti2 < __remain; )
            {
              const __string_type& __target = __targets[__matches[__ti2]];
              if (__pos < __target.size())
                {
                  if (__eof || __target[__pos] != *__s)
                    {
                      __matches[__ti2] = __matches[--__remain];
                      continue;
                    }
                  __matched = true;
                }
              ++__ti2;
            }
          if (__matched)
            {
              ++__s;
              ++__pos;
            }
          for (int __ti3 = 0; __ti3 < __remain;)
            {
              if (__pos > __targets[__matches[__ti3]].size())
                {
                  __matches[__ti3] = __matches[--__remain];
                  continue;
                }
              ++__ti3;
            }
        }
      while (__remain);
      return __s;
    }

  template<typename _CharT>
    _Format_cache<_CharT>::_Format_cache()
    : _M_valid(true), _M_use_grouping(false)
    { }

  template<>
    _Format_cache<char>::_Format_cache();

  template<>
    _Format_cache<wchar_t>::_Format_cache();

  template<typename _CharT>
    void
    _Format_cache<_CharT>::_M_populate(ios_base& __io)
    {
      locale __loc = __io.getloc ();
      numpunct<_CharT> const& __np = use_facet<numpunct<_CharT> >(__loc);
      _M_truename = __np.truename();
      _M_falsename = __np.falsename();
      _M_thousands_sep = __np.thousands_sep();
      _M_decimal_point = __np.decimal_point();
      _M_grouping = __np.grouping();
      _M_use_grouping = _M_grouping.size() != 0 && _M_grouping.data()[0] != 0;
      _M_valid = true;
    }

  // This function is always called via a pointer installed in
  // an ios_base by ios_base::register_callback.
  template<typename _CharT>
    void
    _Format_cache<_CharT>::
    _S_callback(ios_base::event __ev, ios_base& __ios, int __ix) throw()
    {
      void*& __p = __ios.pword(__ix);
      switch (__ev)
        {
        case ios_base::erase_event:
          delete static_cast<_Format_cache<_CharT>*>(__p);
	  __p = 0;
          break;
        case ios_base::copyfmt_event:
          // If just stored zero, the callback would get registered again.
          try 
	    { __p = new _Format_cache<_CharT>; }
          catch(...) 
	    { }
          break;
        case ios_base::imbue_event:
          static_cast<_Format_cache<_CharT>*>(__p)->_M_valid = false;
          break;
        }
    }

  template<typename _CharT>
    _Format_cache<_CharT>*
    _Format_cache<_CharT>::_S_get(ios_base& __ios)
    {
      if (!_S_pword_ix)
        _S_pword_ix = ios_base::xalloc();  // XXX MT
      void*& __p = __ios.pword(_S_pword_ix);

      // XXX What if pword fails? must check failbit, throw.
      if (__p == 0)  // XXX MT?  maybe sentry takes care of it
        {
          auto_ptr<_Format_cache<_CharT> > __ap(new _Format_cache<_CharT>);
          __ios.register_callback(&_Format_cache<_CharT>::_S_callback,
                                  _S_pword_ix);
          __p = __ap.release();
        }
      _Format_cache<_CharT>* __ncp = static_cast<_Format_cache<_CharT>*>(__p);
      if (!__ncp->_M_valid)
        __ncp->_M_populate(__ios);

      return __ncp;
    }

  // This member function takes an (w)istreambuf_iterator object and
  // parses it into a generic char array suitable for parsing with
  // strto[l,ll,f,d]. The thought was to encapsulate the conversion
  // into this one function, and thus the num_get::do_get member
  // functions can just adjust for the type of the overloaded
  // argument and process the char array returned from _M_extract.
  // Other things were also considered, including a fused
  // multiply-add loop that would obviate the need for any call to
  // strto... at all: however, it would b e a bit of a pain, because
  // you'd have to be able to return either floating or integral
  // types, etc etc. The current approach seems to be smack dab in
  // the middle between an unoptimized approach using sscanf, and
  // some kind of hyper-optimized approach alluded to above.

  // XXX
  // Need to do partial specialization to account for differences
  // between character sets. For char, this is pretty
  // straightforward, but for wchar_t, the conversion to a plain-jane
  // char type is a bit more involved.
  template<typename _CharT, typename _InIter>
    void
    num_get<_CharT, _InIter>::
    _M_extract(_InIter /*__beg*/, _InIter /*__end*/, ios_base& /*__io*/,
               ios_base::iostate& /*__err*/, char* /*__xtrc*/,
               int& /*__base*/, bool /*__fp*/) const
    {
      // XXX Not currently done: need to expand upon char version below.
    }

  template<>
    void
    num_get<char, istreambuf_iterator<char> >::
    _M_extract(istreambuf_iterator<char> __beg, 
	       istreambuf_iterator<char> __end, ios_base& __io, 
	       ios_base::iostate& __err, char* __xtrc, int& __base, 
	       bool __fp) const;

#ifdef _GLIBCPP_RESOLVE_LIB_DEFECTS
  // NB: This is an unresolved library defect #17
  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, bool& __v) const
    {
      // Parse bool values as long
      if (!(__io.flags() & ios_base::boolalpha))
        {
          // NB: We can't just call do_get(long) here, as it might
          // refer to a derived class.

          // Stage 1: extract and determine the conversion specifier.
          // Assuming leading zeros eliminated, thus the size of 32 for
          // integral types.
          char __xtrc[32] = {'\0'};
          int __base;
          _M_extract(__beg, __end, __io, __err, __xtrc, __base, false);

          // Stage 2: convert and store results.
          char* __sanity;
          errno = 0;
          long __l = strtol(__xtrc, &__sanity, __base);
          if (!(__err & ios_base::failbit)
              && __l <= 1
              && __sanity != __xtrc && *__sanity == '\0' && errno == 0)
            __v = __l;
          else
            __err |= ios_base::failbit;
        }

      // Parse bool values as alphanumeric
      else
        {
          typedef _Format_cache<char_type> __fcache_type;
          __fcache_type* __fmt = __fcache_type::_S_get(__io);
          const char_type* __true = __fmt->_M_truename.c_str();
          const char_type* __false = __fmt->_M_falsename.c_str();
          const size_t __truelen =  __traits_type::length(__true) - 1;
          const size_t __falselen =  __traits_type::length(__false) - 1;

          for (size_t __pos = 0; __beg != __end; ++__pos)
            {
              char_type __c = *__beg++;
              bool __testf = __c == __false[__pos];
              bool __testt = __c == __true[__pos];
              if (!(__testf || __testt))
                {
                  __err |= ios_base::failbit;
                  break;
                }
              else if (__testf && __pos == __falselen)
                {
                  __v = 0;
                  break;
                }
              else if (__testt && __pos == __truelen)
                {
                  __v = 1;
                  break;
                }
            }
          if (__beg == __end)
            __err |= ios_base::eofbit;
        }

      return __beg;
    }
#endif

#ifdef _GLIBCPP_RESOLVE_LIB_DEFECTS
  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, short& __v) const
    {
      // Stage 1: extract and determine the conversion specifier.
      // Assuming leading zeros eliminated, thus the size of 32 for
      // integral types.
      char __xtrc[32]= {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, false);

      // Stage 2: convert and store results.
      char* __sanity;
      errno = 0;
      long __l = strtol(__xtrc, &__sanity, __base);
      if (!(__err & ios_base::failbit)
          && __sanity != __xtrc && *__sanity == '\0' && errno == 0
          && __l >= SHRT_MIN && __l <= SHRT_MAX)
        __v = static_cast<short>(__l);
      else
        __err |= ios_base::failbit;

      return __beg;
    }

  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, int& __v) const
    {
      // Stage 1: extract and determine the conversion specifier.
      // Assuming leading zeros eliminated, thus the size of 32 for
      // integral types.
      char __xtrc[32] = {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, false);

      // Stage 2: convert and store results.
      char* __sanity;
      errno = 0;
      long __l = strtol(__xtrc, &__sanity, __base);
      if (!(__err & ios_base::failbit)
          && __sanity != __xtrc && *__sanity == '\0' && errno == 0
          && __l >= INT_MIN && __l <= INT_MAX)
        __v = static_cast<int>(__l);
      else
        __err |= ios_base::failbit;

      return __beg;
    }
#endif

  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, long& __v) const
    {
      // Stage 1: extract and determine the conversion specifier.
      // Assuming leading zeros eliminated, thus the size of 32 for
      // integral types.
      char __xtrc[32]= {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, false);

      // Stage 2: convert and store results.
      char* __sanity;
      errno = 0;
      long __l = strtol(__xtrc, &__sanity, __base);
      if (!(__err & ios_base::failbit)
          && __sanity != __xtrc && *__sanity == '\0' && errno == 0)
        __v = __l;
      else
        __err |= ios_base::failbit;

      return __beg;
    }

#ifdef _GLIBCPP_USE_LONG_LONG
  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, long long& __v) const
    {
      // Stage 1: extract and determine the conversion specifier.
      // Assuming leading zeros eliminated, thus the size of 32 for
      // integral types.
      char __xtrc[32]= {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, false);

      // Stage 2: convert and store results.
      char* __sanity;
      errno = 0;
      long long __ll = strtoll(__xtrc, &__sanity, __base);
      if (!(__err & ios_base::failbit)
          && __sanity != __xtrc && *__sanity == '\0' && errno == 0)
        __v = __ll;
      else
        __err |= ios_base::failbit;

      return __beg;
    }
#endif

  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, unsigned short& __v) const
    {
      // Stage 1: extract and determine the conversion specifier.
      // Assuming leading zeros eliminated, thus the size of 32 for
      // integral types.
      char __xtrc[32]= {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, false);

      // Stage 2: convert and store results.
      char* __sanity;
      errno = 0;
      unsigned long __ul = strtoul(__xtrc, &__sanity, __base);
      if (!(__err & ios_base::failbit)
          && __sanity != __xtrc && *__sanity == '\0' && errno == 0
          && __ul <= USHRT_MAX)
        __v = static_cast<unsigned short>(__ul);
      else
        __err |= ios_base::failbit;

      return __beg;
    }

  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, unsigned int& __v) const
    {
      // Stage 1: extract and determine the conversion specifier.
      // Assuming leading zeros eliminated, thus the size of 32 for
      // integral types.
      char __xtrc[32]= {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, false);

      // Stage 2: convert and store results.
      char* __sanity;
      errno = 0;
      unsigned long __ul = strtoul(__xtrc, &__sanity, __base);
      if (!(__err & ios_base::failbit)
          && __sanity != __xtrc && *__sanity == '\0' && errno == 0
          && __ul <= UINT_MAX)
        __v = static_cast<unsigned int>(__ul);
      else
        __err |= ios_base::failbit;

      return __beg;
    }

  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, unsigned long& __v) const
    {
      // Stage 1: extract and determine the conversion specifier.
      // Assuming leading zeros eliminated, thus the size of 32 for
      // integral types.
      char __xtrc[32] = {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, false);

      // Stage 2: convert and store results.
      char* __sanity;
      errno = 0;
      unsigned long __ul = strtoul(__xtrc, &__sanity, __base);
      if (!(__err & ios_base::failbit)
          && __sanity != __xtrc && *__sanity == '\0' && errno == 0)
        __v = __ul;
      else
        __err |= ios_base::failbit;

      return __beg;
    }

#ifdef _GLIBCPP_USE_LONG_LONG
  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, unsigned long long& __v) const
    {
      // Stage 1: extract and determine the conversion specifier.
      // Assuming leading zeros eliminated, thus the size of 32 for
      // integral types.
      char __xtrc[32]= {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, false);

      // Stage 2: convert and store results.
      char* __sanity;
      errno = 0;
      unsigned long long __ull = strtoull(__xtrc, &__sanity, __base);
      if (!(__err & ios_base::failbit)
          && __sanity != __xtrc && *__sanity == '\0' && errno == 0)
        __v = __ull;
      else
        __err |= ios_base::failbit;

      return __beg;
    }
#endif

  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, float& __v) const
    {
      // Stage 1: extract and determine the conversion specifier.
      // Assuming leading zeros eliminated, thus the size of 256 for
      // floating-point types.
      char __xtrc[32]= {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, true);

      // Stage 2: convert and store results.
      char* __sanity;
      errno = 0;
#ifdef _GLIBCPP_USE_C99
      float __f = strtof(__xtrc, &__sanity);
#else
      float __f = static_cast<float>(strtod(__xtrc, &__sanity));
#endif
      if (!(__err & ios_base::failbit)
          && __sanity != __xtrc && *__sanity == '\0' && errno == 0)
        __v = __f;
      else
        __err |= ios_base::failbit;

      return __beg;
    }

  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, double& __v) const
    {
      // Stage 1: extract and determine the conversion specifier.
      // Assuming leading zeros eliminated, thus the size of 256 for
      // floating-point types.
      char __xtrc[32]= {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, true);

      // Stage 2: convert and store results.
      char* __sanity;
      errno = 0;
      double __d = strtod(__xtrc, &__sanity);
      if (!(__err & ios_base::failbit)
          && __sanity != __xtrc && *__sanity == '\0' && errno == 0)
        __v = __d;
      else
        __err |= ios_base::failbit;

      return __beg;
    }

#if defined(_GLIBCPP_USE_C99) && !defined(__hpux)
  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, long double& __v) const
    {
      // Stage 1: extract and determine the conversion specifier.
      // Assuming leading zeros eliminated, thus the size of 256 for
      // floating-point types.
      char __xtrc[32]= {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, true);

      // Stage 2: convert and store results.
      char* __sanity;
      errno = 0;
      long double __ld = strtold(__xtrc, &__sanity);
      if (!(__err & ios_base::failbit)
          && __sanity != __xtrc && *__sanity == '\0' && errno == 0)
        __v = __ld;
      else
        __err |= ios_base::failbit;

      return __beg;
    }
#else
  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, long double& __v) const
    {
      // Stage 1: extract
      char __xtrc[32]= {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, true);

      // Stage 2: determine a conversion specifier.
      ios_base::fmtflags __basefield = __io.flags() & ios_base::basefield;
      const char* __conv;
      if (__basefield == ios_base::oct)
        __conv = "%Lo";
      else if (__basefield == ios_base::hex)
        __conv = "%LX";
      else if (__basefield == 0)
        __conv = "%Li";
      else
        __conv = "%Lg";

      // Stage 3: store results.
      long double __ld;
      int __p = sscanf(__xtrc, __conv, &__ld);
      if (__p
          && static_cast<typename __traits_type::int_type>(__p)
        != __traits_type::eof())
        __v = __ld;
      else
        __err |= ios_base::failbit;

      return __beg;
    }
#endif

  template<typename _CharT, typename _InIter>
    _InIter
    num_get<_CharT, _InIter>::
    do_get(iter_type __beg, iter_type __end, ios_base& __io,
           ios_base::iostate& __err, void*& __v) const
    {
      // Prepare for hex formatted input
      typedef ios_base::fmtflags        fmtflags;
      fmtflags __fmt = __io.flags();
      fmtflags __fmtmask = ~(ios_base::showpos | ios_base::basefield
                             | ios_base::uppercase | ios_base::internal);
      __io.flags(__fmt & __fmtmask | (ios_base::hex | ios_base::showbase));

      // Stage 1: extract and determine the conversion specifier.
      // Assuming leading zeros eliminated, thus the size of 32 for
      // integral types.
      char __xtrc[32]= {'\0'};
      int __base;
      _M_extract(__beg, __end, __io, __err, __xtrc, __base, false);

      // Stage 2: convert and store results.
      char* __sanity;
      errno = 0;
      void* __vp = reinterpret_cast<void*>(strtoul(__xtrc, &__sanity, __base));
      if (!(__err & ios_base::failbit)
          && __sanity != __xtrc && *__sanity == '\0' && errno == 0)
        __v = __vp;
      else
        __err |= ios_base::failbit;

      // Reset from hex formatted input
      __io.flags(__fmt);
      return __beg;
    }

  // __pad is specialized for ostreambuf_iterator, random access iterator.
  template <typename _CharT, typename _OutIter>
    inline _OutIter
    __pad(_OutIter __s, _CharT __fill, int __padding);

  template <typename _CharT, typename _RaIter>
    _RaIter
    __pad(_RaIter __s, _CharT __fill, int __padding, 
	  random_access_iterator_tag)
    {
      fill_n(__s, __fill);
      return __s + __padding;
    }

  template <typename _CharT, typename _OutIter, typename _Tag>
    _OutIter
    __pad(_OutIter __s, _CharT __fill, int __padding, _Tag)
    {
      while (--__padding >= 0) { *__s = __fill; ++__s; }
      return __s;
    }

  template <typename _CharT, typename _OutIter>
    inline _OutIter
    __pad(_OutIter __s, _CharT __fill, int __padding)
    {
      return __pad(__s, __fill, __padding, 
		   typename iterator_traits<_OutIter>::iterator_category());
    }

  template <typename _CharT, typename _OutIter>
    _OutIter
    __pad_numeric(_OutIter __s, ios_base::fmtflags /*__flags*/,
		  _CharT /*__fill*/, int /*__width*/, 
		  _CharT const* /*__first*/, _CharT const* /*__middle*/, 
		  _CharT const* /*__last*/)
  {
      // XXX Not currently done: non streambuf_iterator
      return __s;
    }

  // Partial specialization for ostreambuf_iterator.
  template <typename _CharT>   
    ostreambuf_iterator<_CharT>
    __pad_numeric(ostreambuf_iterator<_CharT> __s, ios_base::fmtflags __flags,
		  _CharT __fill, int __width, _CharT const* __first,
		  _CharT const* __middle, _CharT const* __last)
    {
      typedef ostreambuf_iterator<_CharT> 	__out_iter;
      int __padding = __width - (__last - __first);
      if (__padding < 0)
        __padding = 0;
      ios_base::fmtflags __aflags = __flags & ios_base::adjustfield;
      bool __testfield = __padding == 0 || __aflags == ios_base::left
                         || __aflags == ios_base::internal;

      // This was needlessly complicated.
      if (__first != __middle)
        {
          if (!__testfield)
            {
              __pad(__s, __fill, __padding);
              __padding = 0;
            }
          copy(__first, __middle, __s);
        }
      __out_iter __s2 = __s;

      if (__padding && __aflags != ios_base::left)
        {
          __pad(__s2, __fill, __padding);
          __padding = 0;
        }
      __out_iter __s3 = copy(__middle, __last, __s2);
      if (__padding)
        __pad(__s3, __fill, __padding);
      return __s3;
    }

  template <typename _CharT, typename _OutIter>
    _OutIter
    num_put<_CharT, _OutIter>::
    do_put(iter_type __s, ios_base& __io, char_type __fill, bool __v) const
    {
      const _Format_cache<_CharT>* __fmt = _Format_cache<_CharT>::_S_get(__io);
      ios_base::fmtflags __flags = __io.flags();

      if ((__flags & ios_base::boolalpha) == 0)
        {
          unsigned long __uv = __v;
          return __output_integer(__s, __io, __fill, false, __uv);
        }
      else
        {
          const char_type* __first;
          const char_type* __last;
          if (__v)
            {
              __first = __fmt->_M_truename.data();
              __last = __first + __fmt->_M_truename.size();
            }
          else
            {
              __first = __fmt->_M_falsename.data();
              __last = __first + __fmt->_M_falsename.size();
            }
          copy(__first, __last, __s);
        }
      return __s;
    }

  // __group_digits inserts "group separator" characters into an array
  // of characters.  It's recursive, one iteration per group.  It moves
  // the characters in the buffer this way: "xxxx12345" -> "12,345xxx".
  // Call this only with __grouping != __grend.
  template <typename _CharT>
    _CharT*
    __group_digits(_CharT* __s, _CharT __grsep,  char const* __grouping,
                    char const* __grend, _CharT const* __first,
                    _CharT const* __last)
    {
      if (__last - __first > *__grouping)
        {
          __s = __group_digits(__s,  __grsep,
              (__grouping + 1 == __grend ? __grouping : __grouping + 1),
              __grend, __first, __last - *__grouping);
          __first = __last - *__grouping;
          *__s++ = __grsep;
        }
      do
        {
          *__s++ = *__first++;
        }
      while (__first != __last);
      return __s;
    }

  template <typename _CharT, typename _OutIter, typename _ValueT>
    _OutIter
    __output_integer(_OutIter __s, ios_base& __io, _CharT __fill, bool __neg,
              _ValueT __v)
    {
      // Leave room for "+/-," "0x," and commas.
      const long _M_room = numeric_limits<_ValueT>::digits10 * 2 + 4;
      _CharT __digits[_M_room];
      _CharT* __front = __digits + _M_room;
      ios_base::fmtflags __flags = __io.flags();
      const _Format_cache<_CharT>* __fmt = _Format_cache<_CharT>::_S_get(__io);
      char const* __table = __fmt->_S_literals + __fmt->_S_digits;

      ios_base::fmtflags __basefield = (__flags & __io.basefield);
      _CharT* __sign_end = __front;
      if (__basefield == ios_base::hex)
        {
          if (__flags & ios_base::uppercase)
            __table += 16;  // use ABCDEF
          do
            *--__front = __table[__v & 15];
          while ((__v >>= 4) != 0);
          __sign_end = __front;
          if (__flags & ios_base::showbase)
            {
              *--__front = __fmt->_S_literals[__fmt->_S_x +
                       ((__flags & ios_base::uppercase) ? 1 : 0)];
              *--__front = __table[0];
            }
        }
      else if (__basefield == ios_base::oct)
        {
          do
            *--__front = __table[__v & 7];
          while ((__v >>= 3) != 0);
          if (__flags & ios_base::showbase
              && static_cast<char>(*__front) != __table[0])
            *--__front = __table[0];
          __sign_end = __front;
        }
      else
        {
          // NB: This is _lots_ faster than using ldiv.
          do
            *--__front = __table[__v % 10];
          while ((__v /= 10) != 0);
          __sign_end = __front;
          // NB: ios_base:hex || ios_base::oct assumed to be unsigned.
          if (__neg || (__flags & ios_base::showpos))
            *--__front = __fmt->_S_literals[__fmt->_S_plus - __neg];
        }

      // XXX should specialize!
      if (!__fmt->_M_use_grouping && !__io.width())
        return copy(__front, __digits + _M_room, __s);

      if (!__fmt->_M_use_grouping)
        return __pad_numeric(__s, __flags, __fill, __io.width(0),
			     __front, __sign_end, __digits + _M_room);

      _CharT* __p = __digits;
      while (__front < __sign_end)
        *__p++ = *__front++;
      const char* __gr = __fmt->_M_grouping.data();
      __front = __group_digits(__p, __fmt->_M_thousands_sep, __gr,
        __gr + __fmt->_M_grouping.size(), __sign_end, __digits + _M_room);
      return __pad_numeric(__s, __flags, __fill, __io.width(0),
			   __digits, __p, __front);
    }

  template <typename _CharT, typename _OutIter>
    _OutIter
    num_put<_CharT, _OutIter>::
    do_put(iter_type __s, ios_base& __io, char_type __fill, long __v) const
    {
      unsigned long __uv = __v;
      bool __neg = false;
      if (__v < 0)
        {
          __neg = true;
          __uv = -__uv;
        }
      return __output_integer(__s, __io, __fill, __neg, __uv);
    }

  template <typename _CharT, typename _OutIter>
    _OutIter
    num_put<_CharT, _OutIter>::
    do_put(iter_type __s, ios_base& __io, char_type __fill,
           unsigned long __v) const
    { return __output_integer(__s, __io, __fill, false, __v); }

#ifdef _GLIBCPP_USE_LONG_LONG
  template <typename _CharT, typename _OutIter>
    _OutIter
    num_put<_CharT, _OutIter>::
    do_put(iter_type __s, ios_base& __b, char_type __fill, long long __v) const
    {
      unsigned long long __uv = __v;
      bool __neg = false;
      if (__v < 0)
        {
          __neg = true;
          __uv = -__uv;
        }
      return __output_integer(__s, __b, __fill, __neg, __uv);
    }

  template <typename _CharT, typename _OutIter>
    _OutIter
    num_put<_CharT, _OutIter>::
    do_put(iter_type __s, ios_base& __io, char_type __fill,
           unsigned long long __v) const
    { return __output_integer(__s, __io, __fill, false, __v); }
#endif

  // Generic helper function
  template<typename _CharT, typename _Traits, typename _OutIter>
    _OutIter
    __output_float(_OutIter __s, ios_base& __io, _CharT __fill,
                    const char* __sptr, size_t __slen)
    {
      // XXX Not currently done: non streambuf_iterator
      return __s;
    }

  // Partial specialization for ostreambuf_iterator.
  template<typename _CharT, typename _Traits>
    ostreambuf_iterator<_CharT, _Traits>
    __output_float(ostreambuf_iterator<_CharT, _Traits> __s, ios_base& __io, 
		   _CharT __fill, const char* __sptr, size_t __slen)
    {
      size_t __padding = __io.width() > streamsize(__slen) ?
                         __io.width() -__slen : 0;
      locale __loc = __io.getloc();
      ctype<_CharT> const& __ct = use_facet<ctype<_CharT> >(__loc);
      ios_base::fmtflags __adjfield = __io.flags() & ios_base::adjustfield;
      const char* const __eptr = __sptr + __slen;
      // [22.2.2.2.2.19] Table 61
      if (__adjfield == ios_base::internal)
       {
         // [22.2.2.2.2.14]; widen()
         if (__sptr < __eptr && (*__sptr == '+' || *__sptr == '-'))
           {
             __s = __ct.widen(*__sptr);
             ++__s;
             ++__sptr;
           }
         __s = __pad(__s, __fill, __padding);
         __padding = 0;
       }
      else if (__adjfield != ios_base::left)
        {
          __s = __pad(__s, __fill, __padding);
          __padding = 0;
        }
      // the "C" locale decimal character
      char __decimal_point = *(localeconv()->decimal_point);
      const _Format_cache<_CharT>* __fmt = _Format_cache<_CharT>::_S_get(__io);
      for (; __sptr != __eptr; ++__s, ++__sptr)
       {
         // [22.2.2.2.2.17]; decimal point conversion
         if (*__sptr == __decimal_point)
           __s = __fmt->_M_decimal_point;
         // [22.2.2.2.2.14]; widen()
         else
           __s = __ct.widen(*__sptr);
       }
      // [22.2.2.2.2.19] Table 61
      if (__padding)
        __pad(__s, __fill, __padding);
      __io.width(0);
      return __s;
    }

  bool
  __build_float_format(ios_base& __io, char* __fptr, char __modifier,
		       streamsize __prec);

  template <typename _CharT, typename _OutIter>
    _OutIter
    num_put<_CharT, _OutIter>::
    do_put(iter_type __s, ios_base& __io, char_type __fill, double __v) const
    {
      const streamsize __max_prec = numeric_limits<double>::digits10 + 3;
      streamsize __prec = __io.precision();
      // Protect against sprintf() buffer overflows.
      if (__prec > __max_prec)
        __prec = __max_prec;
      // The *2 provides for signs, exp, 'E', and pad.
      char __sbuf[__max_prec * 2];
      size_t __slen;
      // Long enough for the max format spec.
      char __fbuf[16];
      if (__build_float_format(__io, __fbuf, 0, __prec))
        __slen = sprintf(__sbuf, __fbuf, __prec, __v);
      else
        __slen = sprintf(__sbuf, __fbuf, __v);
      // [22.2.2.2.2] Stages 2-4.
      return __output_float(__s, __io, __fill, __sbuf, __slen);
    }

  template <typename _CharT, typename _OutIter>
    _OutIter
    num_put<_CharT, _OutIter>::
    do_put(iter_type __s, ios_base& __io, char_type __fill,
           long double __v) const
    {
      const streamsize __max_prec = numeric_limits<long double>::digits10 + 3;
      streamsize __prec = __io.precision();
      // Protect against sprintf() buffer overflows.
      if (__prec > __max_prec)
        __prec = __max_prec;
      // The *2 provides for signs, exp, 'E', and pad.
      char __sbuf[__max_prec * 2];
      size_t __slen;
      // Long enough for the max format spec.
      char __fbuf[16];
      // 'L' as per [22.2.2.2.2] Table 59
      if (__build_float_format(__io, __fbuf, 'L', __prec))
        __slen = sprintf(__sbuf, __fbuf, __prec, __v);
      else
        __slen = sprintf(__sbuf, __fbuf, __v);
      // [22.2.2.2.2] Stages 2-4
      return __output_float(__s, __io, __fill, __sbuf, __slen);
    }

  template <typename _CharT, typename _OutIter>
    _OutIter
    num_put<_CharT, _OutIter>::
    do_put(iter_type __s, ios_base& __io, char_type __fill,
           const void* __v) const
    {
      typedef ios_base::fmtflags        fmtflags;
      fmtflags __fmt = __io.flags();
      fmtflags __fmtmask = ~(ios_base::showpos | ios_base::basefield
                             | ios_base::uppercase | ios_base::internal);
      __io.flags(__fmt & __fmtmask | (ios_base::hex | ios_base::showbase));
      try {
        _OutIter __s2 = __output_integer(__s, __io, __fill, false,
                                  reinterpret_cast<unsigned long>(__v));
        __io.flags(__fmt);
        return __s2;
      }
      catch (...) {
        __io.flags(__fmt);
        __throw_exception_again;
      }
    }

  // Support for time_get:
  // Note that these partial specializations could, and maybe should,
  // be changed to full specializations (by eliminating the _Dummy
  // argument) and moved to a .cc file.
  template<typename _CharT, typename _Dummy = int>
    struct _Weekdaynames;

  template<typename _Dummy>
    struct _Weekdaynames<char, _Dummy>
    { static const char* const _S_names[14]; };

  template<typename _Dummy>
    const char* const
    _Weekdaynames<char, _Dummy>::_S_names[14] =
    {
      "Sun", "Sunday",
      "Mon", "Monday",   "Tue", "Tuesday", "Wed", "Wednesday",
      "Thu", "Thursday", "Fri", "Friday",  "Sat", "Saturday"
    };

#ifdef _GLIBCPP_USE_WCHAR_T
  template<typename _Dummy>
    struct _Weekdaynames<wchar_t, _Dummy>
    { static const wchar_t* const _S_names[14]; };

  template<typename _Dummy>
    const wchar_t* const
    _Weekdaynames<wchar_t, _Dummy>::_S_names[14] =
    {
      L"Sun", L"Sunday",
      L"Mon", L"Monday",   L"Tue", L"Tuesday", L"Wed", L"Wednesday",
      L"Thu", L"Thursday", L"Fri", L"Friday",  L"Sat", L"Saturday"
    };
#endif

  template<typename _CharT, typename _Dummy = int>
    struct _Monthnames;

  template<typename _Dummy>
    struct _Monthnames<char,_Dummy>
    { static const char* const _S_names[24]; };

  template<typename _Dummy>
    const char* const
    _Monthnames<char,_Dummy>::_S_names[24] =
    {
      "Jan", "January", "Feb", "February", "Mar", "March",
      "Apr", "April",   "May", "May",      "Jun", "June",
      "Jul", "July",    "Aug", "August",   "Sep", "September",
      "Oct", "October", "Nov", "November", "Dec", "December"
    };

#ifdef _GLIBCPP_USE_WCHAR_T
  template<typename _Dummy>
    struct _Monthnames<wchar_t, _Dummy>
    { static const wchar_t* const _S_names[24]; };

  template<typename _Dummy>
    const wchar_t* const
    _Monthnames<wchar_t,_Dummy>::_S_names[24] =
    {
      L"Jan", L"January", L"Feb", L"February", L"Mar", L"March",
      L"Apr", L"April",   L"May", L"May",      L"Jun", L"June",
      L"Jul", L"July",    L"Aug", L"August",   L"Sep", L"September",
      L"Oct", L"October", L"Nov", L"November", L"Dec", L"December"
    };
#endif

  template<typename _CharT, typename _InIter>
    _InIter
    time_get<_CharT, _InIter>::
    do_get_weekday(iter_type __s, iter_type __end,
                   ios_base& __io, ios_base::iostate& __err, tm* __t) const
    {
      if (!_M_daynames)
        {
          _M_daynames = new basic_string<_CharT>[14];
          for (int __i = 0; __i < 14; ++__i)
            _M_daynames[__i] = _Weekdaynames<_CharT>::_S_names[__i];
        }
      bool __at_eof = false;
      int __remain = 0;
      int __matches[14];
      iter_type __out = __match_parallel(__s, __end, 14, _M_daynames,
                                         __matches, __remain, __at_eof);
      __err = ios_base::iostate(0);
      if (__at_eof) __err |= __io.eofbit;
      if (__remain == 1 ||
          __remain == 2 && (__matches[0]>>1) == (__matches[1]>>1))
        __t->tm_wday = (__matches[0]>>1);
      else
        __err |= __io.failbit;
      return __out;
    }

  template<typename _CharT, typename _InIter>
    _InIter
    time_get<_CharT, _InIter>::
    do_get_monthname(iter_type __s, iter_type __end,
                     ios_base& __io, ios_base::iostate& __err, tm* __t) const
    {
      if (!_M_monthnames)
        {
          _M_monthnames = new basic_string<_CharT>[24];
          for (int __i = 0; __i < 24; ++__i)
            _M_monthnames[__i] = _Monthnames<_CharT>::_S_names[__i];
        }
      bool __at_eof = false;
      int __remain = 0;
      int __matches[24];
      iter_type __out = __match_parallel( __s, __end, 24, _M_monthnames,
                                          __matches, __remain, __at_eof);
      __err = ios_base::iostate(0);
      if (__at_eof) __err |= __io.eofbit;
      if (__remain == 1 ||
          __remain == 2 && (__matches[0]>>1) == (__matches[1]>>1))
        __t->tm_mon = (__matches[0]>>1);
      else
        __err |= __io.failbit;
      return __out;
    }
} // std::

#endif /* _CPP_BITS_LOCFACETS_TCC */

// Local Variables:
// mode:c++
// End:
