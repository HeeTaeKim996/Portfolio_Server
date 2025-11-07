/*-----------------------------------------
		�ڵ� ���ۼ� ���ϰ� �׳� ����
-----------------------------------------*/



#pragma once

#include "Types.h"


// static_cast�� �������� ����(����ȯ �õ��� ���н� ���� �߻�). dynamic_cast�� ĳ��Ʈ ���н�
// ������ �ƴ� nullptr�� ����. dynamic_cast�� ������ ���� ������ ����
// TypeCast �Ʒ� �ڵ�� ��Ÿ���� �ƴ� ������ �� ������ �߻��ϵ��� �ϴ�, ������? �� ���� ����ȯ
// �� ���� �ڵ�

// �� ������ ���� ����. ���� �����ϸ� ��Ÿ���� �ƴ� ������ �� ������ �߻��ϵ��� ����� �ڵ�
//   -> ��Ÿ�ӿ��� �����ϴ� �ڵ� (for.. ) �� ������� �ʰ�, template�� ����Լ� �� �����,
//      �����Ͽ��� ����ȯ ������ �߰��ϵ��� �ϴ� �ڵ�.

// �� �Ʒ�ó�� template �� Ȱ���Ͽ� ������ �� ������ ������ �ϴ� ����� ������, 
//   Moderen C++ å? (����å�̶� ��) �� �����ϴ� �� ���� ��	

#pragma region TyeList
template<typename...T>
struct TypeList;
// ���� ����� ����� ����

template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;
};


template<typename T, typename... U>
struct TypeList<T, U...>
{
	using Head = T;
	using Tail = TypeList<U...>;
};
// ��� ȣ���� ���� ����. TypeList<A, B, C> => TypeList<A, TypeList<B, C>> 


#pragma endregion


#pragma region Length
template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	enum {value = 0};
};

template<typename T, typename ...U>
struct Length<TypeList<T, U...>>
{
	enum { value = 1 + Length<TypeList<U...>>::value };
};

// => Length<TypeList<Mage, Knight, Archer>>::value = (1 + (1 + ( 1 + 0 ) ) ) = 3
#pragma endregion


#pragma region TypeAt
template<typename TL, int32 index>
struct TypeAt;

template<typename Head, typename...Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
	using Result = Head;
};

template<typename Head, typename... Tail, int32 index>
struct TypeAt<TypeList<Head, Tail...>, index>
{
	using Result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};
	 
// => ���������� ���ȣ��. 
//  using TL = TyeList<Mage, Knight, Archer>;
//  TypeAt<TL, 2>::Result = TypeAt< TypeList<Knight, Archer>, 1> = TypeAt<Archer, 0> = Archer
#pragma endregion


#pragma region IndexOf
template<typename TL, typename T>
struct IndexOf;

template<typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
	enum{value = 0};
};


template<typename T>
struct IndexOf<TypeList<>, T>
{
	enum{value = -1};
};


template<typename Head, typename ...Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T>
{
private:
	enum{temp =IndexOf<TypeList<Tail...>, T>::value};

public:
	enum { value = (temp == -1) ? -1 : temp + 1 };

};
// => Header�� ã�� T�� ������ 0 ��ȯ. T�� TL�� �ƾ� ���ٸ� -1 ��ȯ.
//    �� �� �ƴҽ�, ���ȣ��� index ã��. temp���� Head�� �߶�, �������� ��� ȣ��.
//    ��� ȣ��� ���� temp ���� -1�Ͻ�, 1 ��ȯ. -1�� �ƴҽ�, ��� ȣ�⿡�� ������ + 1��
//    �ݺ��Ͽ�, value�� ��ȯ. 
// (Header�� �ڸ��� ���ȣ�⿡�� Index�� -1 �� �и��Ƿ�. value���� ������ +1 �� ���? ȣ���Ͽ� ��ȯ) 
#pragma endregion


#pragma region Conversion
template<typename From, typename To>
class Conversion
{
private:
	using Small = __int8;
	using Big = __int32;

	static Small Test(const To&) { return 0; }
	static Big Test(...) { return 0; } // ���⼭ ... �� '�ƹ� Ÿ���̳� �޴´�' �� �ǹ�
	static From MakeFrom() { return 0; }

	// �� �� �Լ��� { } (����)�� ��� �ȴ�. Conversion�� �Լ� �����ε��� ������ �̿��� ���̱� ����

public:
	enum
	{
		exists = sizeof(Test(MakeFrom())) == sizeof(Small)
	};
};

// => �Լ��� �����ε� ������ �̿��� �ڵ�.
//    ���÷�, 
//    1) Coversion<Player, Knight>:: exists
//     -> MakeFrom() -> Player. Test(Player) -> Small Test(const Knight&) ȣ��.
//     -> Small ��ȯ. -> exists = sizeof(Small) == sizeof(Small). -> True ��ȯ.

//    2) Conversion<Knight, Player>:: exists
//     -> MakeFrom() -> Knight. Test(Knight) -> Big Test(...) ȣ��.
//       ( !! �Ű������� ���ڿ� ��ĳ���� �Ұ�)
//     -> Big ��ȯ. -> exists = sizeof(Big) == sizeof(Small) -> False ��ȯ.

//    3) Conversion<Knight, Dog>::exists
//      -> 2)�� ���� ������ false ��ȯ
#pragma endregion




#pragma region TypeCast

// �Ʒ� s_convert[i][j] �� for������ ó������ �ʰ�, �Ʒ� �ڵ��ó�� ��� ���ȣ��� ó���ϴ� ������, 
// �Ʒ� �ڵ�� ������� ���Ǵ� s_convert[i][j]�� ��Ÿ���� �ƴ� ������ Ÿ�� ������ ������� 
// ���̺��� �����ϱ� ����


template<int32 v>
struct Int2Type
{
	enum{value = v};
};


template<typename TL>
class TypeConversion
{
public:
	enum
	{
		length = Length<TL>::value
	};

	TypeConversion()
	{
		MakeTable(Int2Type<0>(), Int2Type<0>());
	}

	template<int32 i, int32 j>
	static void MakeTable(Int2Type<i>, Int2Type<j>)
	{
		using FromType = typename TypeAt<TL, i>::Result;
		using ToType = typename TypeAt<TL, j>::Result;

		s_convert[i][j] = Conversion<const FromType*, const ToType*>::exists ?
			true : false;
#pragma region FromType*, ToType* ���� pt �� �ִ� ����
		// - �켱 A*, B*�� pt�� unsigned int�� �޸� �ּ� ���� �Ӹ� �ƴ�, A, B�� Ŭ���� ����?��
		//   ��� �ִ�.
		//  -> A*, B* �� ���ؼ� A�� B�� ��Ӱ��������� �� �� �ִ�.

		// - FromType, ToType�� �ƴ� FromType*, ToType* �� ������, �� �ڵ���� ������ Ÿ�� ����
		//   ������� ��ȯ ���� ������ �˷��ִ� ����� �ϴµ�, pt ������� �񱳸� �ؾ� ������ Ÿ��
		//   ���� ������� �񱳰� �����ϱ� �����̶� �Ѵ�. (From ����Ƽ)
#pragma endregion



		MakeTable(Int2Type<i>(), Int2Type<j + 1>());
	}

	// �� ���ȣ�⿡�� Int2Type<j+1> == Int2Type<length>�� �� ȣ��
	template<int32 i>
	static void MakeTable(Int2Type<i>, Int2Type<length>) 
	{
		MakeTable(Int2Type<i + 1>(), Int2Type<0>());
	}

	// �� ���ȣ�⿡�� Int2Type<i+1> == Int2Type<length> �� �� ȣ��
	template<int32 j>
	static void MakeTable(Int2Type<length>, Int2Type<j>)
	{
	}


	static inline bool CanConvert(int32 from, int32 to)
	{
		static TypeConversion conversion;
		return s_convert[from][to];
	}

public:
	static bool s_convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];



template<typename To, typename From>
To TypeCast(From* ptr)
{
	if (!ptr)
	{
		return nullptr;
	}

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
	{
		return static_cast<To>(ptr);
	}

	return nullptr;
}


template<typename To, typename From>
bool CanCast(From* ptr)
{
	if (ptr == nullptr)
	{
		return false;
	}

	using TL = typename From::TL;

	return TypeConversion<TL>::CanConvert(
		ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);
}


// shared_ptr ���� 2 �߰�

#include "Memory.h"

template<typename To, typename From>
std::shared_ptr<To> TypeCast(std::shared_ptr<From> ptr)
{
	if (!ptr)
	{
		return nullptr;
	}

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
	{
		return std::static_pointer_cast<To>(ptr);
	}

	return nullptr;
}

template<typename To, typename From>
bool CanCast(std::shared_ptr<From> ptr)
{
	if (ptr == nullptr)
	{
		return false;
	}

	using TL = typename From::TL;

	return TypeConversion<TL>::CanConvert(
		ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);
}

#pragma endregion


#define DECLARE_TL		using TL = TL; int32 _typeId
#define INIT_TL(Type)	_typeId = IndexOf<TL, Type>::value