
#ifndef CPPUTILS_PROPERTY_H
#define CPPUTILS_PROPERTY_H

#include <EnumFactory.h>
#include <type_traits>
#include <functional>
#include <memory>

/**
 * This is almost entirely based on the implementation found at
 * https://blogs.msdn.microsoft.com/gpalem/2008/06/19/tracking-c-variable-state-changes/
 * The only change is switching the callbacks to use lambdas and using the type traits
 * from std instead of Loki.
 */
namespace CppUtils
{
  /**
   * Property - A template to track changes to the underlying type
   *
   * Instantiation of Properties on user defined types will in the property
   * deriving from the underlying type. For predefined types, we can't derive
   * so we create a dummy class (EmptyType) to derive from and instead have a
   * member variable of the underlying type and the Property acts as a wrapper
   */


  /**
   * EmptyType - this has a copy constructor
   */
  class EmptyType
  {
  public:
    template<typename T>
    EmptyType(const T&) { }
    EmptyType() { };
  };

  /**
   * Tests if the given type is a class. If so ResultantType is set to T else to
   * the EmptyType
   *
   * @tparam T - Must Default and Copy Constructable
   */
  template<typename T>
  struct IsClassType
  {
    using ResultantType = typename std::conditional<std::is_class<T>::value == true, T, EmptyType>::type;
  };

  /**
   * Tests if the give type is a pre-defined type. If so ResultantType is T
   * else EmptyType
   *
   * @tparam T
   */
  template<typename T>
  struct IsPredefinedType
  {
    using ResultantType = typename std::conditional<std::is_class<T>::value == false, T, EmptyType>::type;
  };

#define TRACKED_OPS_E( XX )\
  XX( eFunUnknown, = 0 )\
  XX( eFunConstructor, )\
  XX( eFunCopyConstructor, )\
  XX( eFunDestructor, )\
  XX( eOpAssign, )\
  XX( eOpPlusAssign, )\
  XX( eOpMinusAssign, )\
  XX( eOpStarAssign, )\
  XX( eOpSlashAssign, )\
  XX( eOpModAssign, )\
  XX( eOpAndAssign, )\
  XX( eOpOrAssign, )\
  XX( eOpXorAssign, )\
  XX( eOpLShiftAssign, )\
  XX( eOpRShiftAssign, )\
  XX( eOpPreIncrement, )\
  XX( eOpPostIncrement, )\
  XX( eOpPreDecrement, )\
  XX( eOpPostDecrement, )

  DECLARE_ENUM_NONMEMBER( ETrackedOps, TRACKED_OPS_E );

  template<typename T> struct type2type { using underlying_type_t = T; };
  /**
   * T Must be copy constructable
   * @tparam T
   */
  template<typename T>
  class Property : public IsClassType<T>::ResultantType
  {
  public:
    using ClassType = typename IsClassType<T>::ResultantType;
    using PredefinedType = typename IsPredefinedType<T>::ResultantType;

    enum { IsClass = std::is_class<T>::value };
    using ValueChangedFn = std::function<void( ETrackedOps op, const T& newValue, std::unique_ptr<T> oldValue )>;
    using ValueChangingFn = std::function<bool( ETrackedOps op, const T& newValue, std::unique_ptr<T> oldValue )>;
  protected:
    PredefinedType m_data;
    ValueChangingFn m_valueChanging;
    ValueChangedFn m_valueChanged;

    template<typename CType>
    inline void Constructor( ETrackedOps op, type2type<CType> )
    {
      if( m_valueChanged ) {
        m_valueChanged( op, *this, nullptr );
      }
    }

    inline void Constructor( ETrackedOps op, type2type<EmptyType> )
    {
      if( m_valueChanged ) {
        m_valueChanged( op, m_data, nullptr );
      }
    }
    template<typename TypeSelect>
    inline void Destructor( type2type<TypeSelect> )
    {
      if( m_valueChanged) {
        m_valueChanged( eFunDestructor, *this, nullptr );
      }
    }

    inline void Destructor( type2type<EmptyType> )
    {
      if( m_valueChanged ) { m_valueChanged( eFunDestructor, m_data, nullptr ); }

    }

  public:

    inline Property() :
        ClassType{}, m_data{}, m_valueChanged{ }, m_valueChanging{ }
    { Constructor( eFunConstructor, type2type<ClassType>() ); }

    template<typename U> inline Property( const U& other ) :
        ClassType{ other }, m_data{ other }, m_valueChanged { }, m_valueChanging{ }
    {
      Constructor( eFunCopyConstructor, type2type<ClassType>() );
    }
    template<typename U> inline Property( const Property<U>& other ) :
        ClassType{ dynamic_cast<U&>(other) },
        m_valueChanging{ other.m_valueChanging },
        m_valueChanged{ other.m_valueChanged },
        m_data{ (U)other }
    {
      Constructor( eFunCopyConstructor, type2type<ClassType>() );
    }

    inline ~Property() { Destructor( type2type<ClassType>() ); }

    void SubscribeChanged( ValueChangedFn fn ) { m_valueChanged = fn; }
    void SubscribeChanging( ValueChangingFn fn ) { m_valueChanging = fn; }

#define OVERLOAD_BINARY_OPERATOR( operator_func, DATA_OP, OP_FUNC_CTX )\
    protected:\
    template<typename U, typename TypeSelect>\
    inline void Func_##OP_FUNC_CTX( const U& other, type2type<TypeSelect> )\
    {\
      std::unique_ptr<T> oldVal{ new T{ *(( T * ) this ) } };\
      bool should_change = true;\
      if( m_valueChanging ) {\
        should_change = m_valueChanging( OP_FUNC_CTX, other, move( oldVal ) );\
      }\
      if( should_change ) {\
        ClassType::operator_func( other );\
        if( m_valueChanged ) {\
          m_valueChanged( OP_FUNC_CTX, *(( T * ) this ), move( oldVal ) );\
        }\
      }\
    }\
    template<typename U> inline void Func_##OP_FUNC_CTX( const U& other, type2type<EmptyType> )\
    {\
      std::unique_ptr<T> oldVal{ new T{ m_data } };\
      bool should_change = true;\
      if( m_valueChanging ) {\
        should_change = m_valueChanging( OP_FUNC_CTX, other, move( oldVal ) );\
      }\
      if( should_change ) {\
        std::unique_ptr<T> oldVal2{ new T{ m_data } };\
        m_data DATA_OP other;\
        if( m_valueChanged ) {\
          m_valueChanged( OP_FUNC_CTX, m_data, move( oldVal2 ) );\
        }\
      }\
    }\
    template<typename U, typename TypeSelect>\
    inline void Func_##OP_FUNC_CTX( const Property<U>& other, type2type<TypeSelect> passThru )\
    {\
      Func_##OP_FUNC_CTX( dynamic_cast<const U&>(other), passThru );\
    }\
    template<typename U>\
    inline void Func_##OP_FUNC_CTX( const Property<U>& other, type2type<EmptyType>passThru )\
    {\
      Func_##OP_FUNC_CTX( dynamic_cast<const U&>(other), passThru );\
    }\
  public:\
    template<typename U>\
    inline Property& operator_func( const U& other )\
    {\
      Func_##OP_FUNC_CTX( other, type2type<ClassType>() );\
      return *this;\
    }

    OVERLOAD_BINARY_OPERATOR( operator=, =, eOpAssign )
    OVERLOAD_BINARY_OPERATOR( operator+=, +=, eOpPlusAssign )
    OVERLOAD_BINARY_OPERATOR( operator-=, -=, eOpMinusAssign )
    OVERLOAD_BINARY_OPERATOR( operator*=, *=, eOpStarAssign )
    OVERLOAD_BINARY_OPERATOR( operator/=, /=, eOpSlashAssign )
    OVERLOAD_BINARY_OPERATOR( operator&=, &=, eOpAndAssign )
    OVERLOAD_BINARY_OPERATOR( operator|=, |=, eOpOrAssign )
    OVERLOAD_BINARY_OPERATOR( operator^=, ^=, eOpXorAssign )
    OVERLOAD_BINARY_OPERATOR( operator<<=, <<=, eOpLShiftAssign )
    OVERLOAD_BINARY_OPERATOR( operator>>=, >>=, eOpRShiftAssign )

  };
}

#endif //CPPUTILS_PROPERTY_H
