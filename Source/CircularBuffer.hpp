#pragma once

#include <assert.h>

template<class Type, int t_Capacity>
class CircularBuffer final
{
	static_assert(t_Capacity > 0, "Invalid capacity.");

public:
	constexpr CircularBuffer() = default;

	template <typename... Types>
	constexpr CircularBuffer(Types&&... values);

	constexpr auto operator[](const int index)->Type&;
	constexpr auto operator[](const int index) const -> const Type&;

	constexpr bool IsEmpty() const;
	constexpr bool IsFull() const;

	constexpr int GetCapacity() const;
	constexpr int GetCount() const;

	constexpr auto Get(const int index)->Type&;
	constexpr auto Get(const int index) const -> const Type&;

	constexpr auto GetFirst()->Type&;
	constexpr auto GetFirst() const -> const Type&;

	constexpr auto GetLast()->Type&;
	constexpr auto GetLast() const -> const Type&;

	void RemoveAll();
	void Pop();
	void Push(const Type& value);

private:
	Type m_Array[t_Capacity];
	int m_Count = 0;
	int m_Head = 0;
	int m_Tail = -1;
};

#include "CircularBuffer.inl"