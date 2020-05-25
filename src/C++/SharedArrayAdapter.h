/* -*- C++ -*- */

/****************************************************************************
** Copyright (c) 2001-2014
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifndef SHAREDARRAYADAPTER_H
#define SHAREDARRAYADAPTER_H

namespace FIX
{
  template< typename T >
  struct make_shared_array_strategy
  {
    typedef std::shared_ptr< T[] > type;

    type make_shared_array(std::size_t size)
    {
      return std::make_shared< T[] >( size );
    }
  };

  template< typename T >
  struct shared_ptr_array_strategy
  {
    typedef std::shared_ptr< T[] > type;

    type make_shared_array(std::size_t size)
    {
      std::shared_ptr< T[] > array( new T[size] );
      return array;
    }
  };

  template< typename T >
  struct shared_ptr_custom_deleter
  {
    typedef std::shared_ptr< T > type;

    static type make_shared_array(std::size_t size)
    {
      std::shared_ptr< T > array( new T[size], std::default_delete< T[] >());
      return array;
    }
  };

  template< template < typename > class AS, typename T>
  class shared_array_adapter
  {
    typedef AS<T> ArrayStrategy;
    typedef typename ArrayStrategy::type ArrayType;
  public:
    shared_array_adapter() = default;

    operator T * () const
    { return m_array.get(); }

    bool empty() const
    { return static_cast< bool >( m_array ); }

    static shared_array_adapter create(std::size_t size)
    {
      return shared_array_adapter( ArrayStrategy::make_shared_array( size ) );
    }

  private:
    shared_array_adapter( ArrayType array )
      : m_array( std::move( array ) )
    { }

    ArrayType m_array;
  };

  template< template < typename  > class AS >
  struct shared_array_adapter_helper
  {
    template< typename T >
    using type = shared_array_adapter< AS, T >;
  };
}

#endif // SHAREDARRAYADAPTER_H
