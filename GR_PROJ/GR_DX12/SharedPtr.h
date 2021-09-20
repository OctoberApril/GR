#pragma once

template<typename T>
class SharedPtr
{
public:
	SharedPtr() :m_Ptr(nullptr) {}
	explicit SharedPtr(T* ptr) :m_Ptr(ptr) {}

	SharedPtr(const SharedPtr<T>& other) :m_Ptr(other.m_Ptr)
	{
		AddRef();
	}
	template<typename U>
	SharedPtr(const SharedPtr<U>& other) : m_Ptr(other.m_Ptr)
	{
		AddRef();
	}

	SharedPtr(SharedPtr<T>&& other) :m_Ptr(other.m_Ptr)
	{
		other.Release();
		AddRef();
	}

	template<typename U>
	SharedPtr(SharedPtr<U>&& other) :m_Ptr(other.m_Ptr)
	{
		other.Release();
		AddRef();
	}

	~SharedPtr()
	{
		Release();
	}

	SharedPtr<T>& operator=(const SharedPtr<T>& other)
	{
		if (m_Ptr == other.m_Ptr) return *this;

		Release();
		m_Ptr = other.m_Ptr;
		AddRef();

		return *this;
	}

	template<typename U>
	SharedPtr<T>& operator=(const SharedPtr<U>& other)
	{
		if (m_Ptr == other.m_Ptr) return *this;

		Release();
		m_Ptr = other.m_Ptr;
		AddRef();

		return *this;
	}

	SharedPtr<T>& operator=(T* rawPtr)
	{
		if (m_Ptr == rawPtr) return *this;

		Release();
		m_Ptr = rawPtr;
		AddRef();

		return *this;
	}

	bool operator==(T* rawPtr) const
	{
		return m_Ptr == rawPtr;
	}

	T* operator->() const
	{
		return m_Ptr;
	}

	T& operator*() const
	{
		return *m_Ptr;
	}

	operator T* ()
	{
		return m_Ptr;
	}

	void AddRef()
	{
		if (m_Ptr)
		{
			m_Ptr->AddRef();
		}
	}

	void Release()
	{
		if (m_Ptr)
		{
			m_Ptr->Release();
			m_Ptr = nullptr;
		}
	}

	void Reset()
	{
		Release();
	}

	T* Get()
	{
		return m_Ptr;
	}

	T** GetAddress() const
	{
		return &m_Ptr;
	}
	
	int  GetRefCount() const
	{
		return m_Ptr->GetRefCount();
	}

private:
	T* m_Ptr;
};

