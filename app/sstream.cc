// -*-C++-*- srccxx/sstream.cc
// <!!----------------------------------------------------------------------> 
// <!! Copyright (C) 1997 Dietmar Kuehl, Claas Solutions GmbH > 
// <!!> 
// <!! Permission to use, copy, modify, distribute and sell this > 
// <!! software for any purpose is hereby granted without fee, provided > 
// <!! that the above copyright notice appears in all copies and that > 
// <!! both that copyright notice and this permission notice appear in > 
// <!! supporting documentation. Dietmar Kuehl and Claas Solutions make no > 
// <!! representations about the suitability of this software for any > 
// <!! purpose. It is provided "as is" without express or implied warranty. > 
// <!!----------------------------------------------------------------------> 

// Author: Dietmar Kuehl dietmar.kuehl@claas-solutions.de 
// Title:  Implementation of the string stream classes 

// -------------------------------------------------------------------------- 

#if !defined(SSTREAM_CC)
#define SRCCXX_SSTREAM_CC 1

#if !defined(SSTREAM_H)
#  include <sstream.h>
#endif

#include <string>

// -------------------------------------------------------------------------

template <class _CS_cT, class _CS_Tr, class _CS_Al>
basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::basic_stringbuf(ios_base::openmode _CS_m):
  _CS_m_mode(_CS_m),
  _CS_m_beg(0),
  _CS_m_end(0)
{
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::basic_stringbuf(string_type const &_CS_s,
							 ios_base::openmode _CS_m):
  _CS_m_mode(_CS_m),
  _CS_m_beg(_CS_s.size() > 0? new _CS_cT[_CS_s.size()]: 0),
  _CS_m_end(_CS_m_beg + _CS_s.size())
{
  traits_type::copy(_CS_m_beg, _CS_s.data(), _CS_s.size());
  if (_CS_m & ios_base::in)
    setg(_CS_m_beg, _CS_m_beg, _CS_m_end);
  if (_CS_m & ios_base::out)
    setp(_CS_m_beg, _CS_m_end);
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::~basic_stringbuf()
{
  delete[] _CS_m_beg;
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
basic_string<_CS_cT, _CS_Tr, _CS_Al>
basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::str() const
{
  if (pbase() == epptr())
    if (eback() == egptr())
      return basic_string<_CS_cT, _CS_Tr, _CS_Al>();
    else
      return basic_string<_CS_cT, _CS_Tr, _CS_Al>(eback(), egptr());
  else
    return basic_string<_CS_cT, _CS_Tr, _CS_Al>(pbase(), epptr());
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
void
basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::str(string_type const &_CS_s)
{
  _CS_cT* _CS_nv = new _CS_cT[_CS_s.size()];
  traits_type::copy(_CS_nv, _CS_s.data(), _CS_s.size());
  delete[] _CS_m_beg;

  _CS_m_beg = _CS_nv;
  _CS_m_end = _CS_m_beg + _CS_s.size();
  if (_CS_m_mode & ios_base::in)
    setg(_CS_m_beg, _CS_m_beg, _CS_m_end);
  if (_CS_m_mode & ios_base::out)
    setp(_CS_m_beg, _CS_m_end);
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
typename basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::int_type
basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::underflow()
{
  return gptr() == egptr()? traits_type::eof()
                          : traits_type::to_int_type(*gptr());
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
typename basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::int_type
basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::pbackfail(int_type _CS_c)
{
  if (eback() != gptr())
    {
      if (traits_type::eq_int_type(_CS_c, traits_type::eof()))
        {
	  gbump(-1);
          return traits_type::not_eof(_CS_c);
        }
      else
	if (traits_type::eq(traits_type::to_char_type(_CS_c), gptr()[-1])
	    || (_CS_m_mode & ios_base::out) != 0)
	  {
	    gbump(-1);
	    traits_type::assign(*gptr(), traits_type::to_char_type(_CS_c));
            return _CS_c;
	  }
    }
  // fails if
  // - no read position is available
  // - c is not eof, does not match the last read char, and the stream is
  //   read-only
  return traits_type::eof();
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
typename basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::int_type
basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::overflow(int_type _CS_c)
{
  if (traits_type::eq_int_type(_CS_c, traits_type::eof()))
    return traits_type::not_eof(_CS_c);
  else if (_CS_m_mode & ios_base::out)
    {
      if (_CS_m_beg == _CS_m_end)
	{
	  delete [] _CS_m_beg;
	  _CS_m_beg = new _CS_cT[64];
	  _CS_m_end = _CS_m_beg + 64;
	  setp(_CS_m_beg, _CS_m_beg);
	}

      if (pptr() != epptr())
	{
	  traits_type::assign(*pptr(), traits_type::to_char_type(_CS_c));
	  pbump(1);
	}
      else if (epptr() != _CS_m_end)
	{
	  traits_type::assign(*pptr(), traits_type::to_char_type(_CS_c));
	  setp(pbase(), epptr() + 1);
	  pbump(epptr() - pbase());
	}
      else
        {
	  streamsize _CS_size = _CS_m_end - _CS_m_beg;
	  _CS_cT* _CS_nv = new _CS_cT[2 * _CS_size];
	  traits_type::copy(_CS_nv, _CS_m_beg, _CS_size);
	  delete[] _CS_m_beg;
	  _CS_m_beg = _CS_nv;
	  _CS_m_end = _CS_nv + 2 * _CS_size;
	  traits_type::assign(_CS_m_beg[_CS_size], traits_type::to_char_type(_CS_c));
	  setp(_CS_m_beg, _CS_m_beg + _CS_size + 1);
	  pbump(_CS_size + 1);
        }

      if (_CS_m_mode & ios_base::in)
	setg(pbase(), pbase() + (gptr() - eback()), pptr());
      return _CS_c;
    }

  return traits_type::eof();
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
basic_streambuf<_CS_cT, _CS_Tr>*
basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::setbuf(char_type*, streamsize)
{
  //-dk:TODO document that setbuf() has no effect! (implementation defined)
  return this;
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
typename basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::pos_type
basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::seekoff(off_type _CS_off,
                                                                   ios_base::seekdir _CS_dir,
                                                                   ios_base::openmode _CS_which)
{
  _CS_cT* _CS_np = 0;
  _CS_cT* _CS_e = _CS_m_mode & ios_base::out? epptr(): egptr();

  switch (_CS_dir)
    {
    case ios_base::beg:
      if (_CS_off < 0 || (_CS_e - _CS_m_beg) < _CS_off)
        return pos_type(off_type(-1));
      _CS_np = _CS_m_beg + _CS_off;
      break;
    case ios_base::end:
      if (_CS_off > 0 || (_CS_e - _CS_m_beg) < -_CS_off)
        return pos_type(off_type(-1));
      _CS_np = _CS_e + _CS_off;
      break;
    case ios_base::cur:
      if ((_CS_which & (ios_base::in | ios_base::out)) == (ios_base::in | ios_base::out)
	  && pptr() != gptr())
        return pos_type(off_type(-1));
      _CS_np = (_CS_which & ios_base::in)? gptr(): pptr();

      if (_CS_off < -(_CS_np - _CS_m_beg)
          || _CS_e - _CS_np < _CS_off)
        return pos_type(off_type(-1));
      _CS_np += _CS_off;
      break;
    }

  switch (_CS_which & (ios_base::in | ios_base::out))
    {
    case ios_base::in:
      if (_CS_m_mode & ios_base::in)
	gbump(_CS_np - gptr());
      else
	return pos_type(off_type(-1));
      break;
    case ios_base::out:
	if (_CS_m_mode & ios_base::out)
	  pbump(_CS_np - pptr());
	else
	  return pos_type(off_type(-1));
	break;
    case ios_base::in | ios_base::out:
      if ((_CS_m_mode & (ios_base::in | ios_base::out)) == (ios_base::in | ios_base::out))
	{
	  gbump(_CS_np - gptr());
	  pbump(_CS_np - pptr());
	}
      else
	return pos_type(off_type(-1));
      break;
    default:
      return pos_type(off_type(-1));
    }

  return pos_type(_CS_np - _CS_m_beg);
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
typename basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::pos_type
basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::seekpos(pos_type _CS_pos,
								   ios_base::openmode _CS_which)
{
  _CS_cT* _CS_p = (_CS_m_mode & ios_base::out)? epptr(): egptr();
  if (_CS_pos < 0 || (_CS_p - _CS_m_beg) < _CS_pos)
    return pos_type(off_type(-1));

  _CS_p = _CS_m_beg + _CS_pos;

  switch (_CS_which & (ios_base::in | ios_base::out) & _CS_m_mode)
    {
    case ios_base::in:
      gbump(_CS_p - gptr());
      break;
    case ios_base::out:
      pbump(_CS_p - pptr());
      break;
    case ios_base::in | ios_base::out:
      gbump(_CS_p - gptr());
      pbump(_CS_p - pptr());
      break;
    default:
      return pos_type(off_type(-1));
    }

  return _CS_pos;
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
int
basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>::sync()
{
  // do nothing, except signaling success
  return traits_type::not_eof(traits_type::eof());
}

// --------------------------------------------------------------------------

template <class _CS_cT, class _CS_Tr, class _CS_Al>
_CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>::
    _CS_basic_sstream_base( ios_base::openmode _CS_mode):
      _CS_m_ssbuf(_CS_mode)
{
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
_CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>::
    _CS_basic_sstream_base(_CS_string_type const& _CS_str,
				 ios_base::openmode _CS_mode):
      _CS_m_ssbuf(_CS_str, _CS_mode)
{
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
basic_string<_CS_cT, _CS_Tr, _CS_Al>
_CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>::str() const
{
  return _CS_m_ssbuf.str();
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
void
_CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>::str(
    _CS_string_type const& _CS_str)
{
  _CS_m_ssbuf.str(_CS_str);
}

// --------------------------------------------------------------------------

template <class _CS_cT, class _CS_Tr, class _CS_Al>
basic_istringstream<_CS_cT, _CS_Tr, _CS_Al>::basic_istringstream(
    ios_base::openmode _CS_mode):
  _CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>(_CS_mode | ios_base::in),
  ios(&_CS_m_ssbuf),
  basic_istream<_CS_cT, _CS_Tr>(&_CS_m_ssbuf)
{
  init(&_CS_m_ssbuf);
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
basic_istringstream<_CS_cT, _CS_Tr, _CS_Al>::basic_istringstream(
    _CS_string_type const& _CS_str,
    ios_base::openmode _CS_mode):
  _CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>(_CS_str, _CS_mode | ios_base::in),
  ios(&_CS_m_ssbuf)
{
  init(&_CS_m_ssbuf);
}

// --------------------------------------------------------------------------

template <class _CS_cT, class _CS_Tr, class _CS_Al>
basic_ostringstream<_CS_cT, _CS_Tr, _CS_Al>::basic_ostringstream(
    ios_base::openmode _CS_mode):
  _CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>(_CS_mode | ios_base::out)
{
  init(&_CS_m_ssbuf);
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
basic_ostringstream<_CS_cT, _CS_Tr, _CS_Al>::basic_ostringstream(
    _CS_string_type const& _CS_str,
    ios_base::openmode _CS_mode):
  _CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>(_CS_str,
									 _CS_mode | ios_base::out),
  ios(&_CS_m_ssbuf),
  basic_ostream<_CS_cT, _CS_Tr>(&_CS_m_ssbuf)
{
  init(&_CS_m_ssbuf);
}

// --------------------------------------------------------------------------

template <class _CS_cT, class _CS_Tr, class _CS_Al>
basic_stringstream<_CS_cT, _CS_Tr, _CS_Al>::basic_stringstream(
    ios_base::openmode _CS_mode):
  _CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>(_CS_mode | in | out),
  ios(&_CS_m_ssbuf),
  basic_iostream<_CS_cT, _CS_Tr>(&_CS_m_ssbuf)
{
  init(&_CS_m_ssbuf);
}

template <class _CS_cT, class _CS_Tr, class _CS_Al>
basic_stringstream<_CS_cT, _CS_Tr, _CS_Al>::basic_stringstream(
    _CS_string_type const& _CS_str,
    ios_base::openmode _CS_mode):
  _CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>(_CS_str, _CS_mode | in | out),
  ios(&_CS_m_ssbuf),
  basic_iostream<_CS_cT, _CS_Tr>(&_CS_m_ssbuf)
{
  init(&_CS_m_ssbuf);
}

// --------------------------------------------------------------------------

#endif /* SSTREAM_CC */
