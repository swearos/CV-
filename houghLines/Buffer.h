#pragma once

template<typename T,size_t fixed_size = 1024/sizeof(T) +8>
class Buffer
{
public:
	typedef T value_type;

	Buffer();
	Buffer(size_t size);
	Buffer(const Buffer<T, fixed_size>& buf);
	Buffer<T, fixed_size>& operator=(const Buffer<T, fixed_size>& buf);

	~Buffer();

	void allocate(size_t size);
	void deallocate();

	void resize(size_t size);
	size_t size() const;

	operator T*();
	operator const T*() const;
protected:
	T* ptr;
	size_t sz;
	T buf[(fixed_size > 0) ? fixed_size : 1]; 
};


template<typename T, size_t fixed_size> inline
Buffer<T, fixed_size>::Buffer()
{
	ptr = buf;
	sz = fixed_size;
}

template<typename T, size_t fixed_size> inline
Buffer<T, fixed_size>::Buffer(size_t _size)
{
	ptr = buf;
	sz = fixed_size;
	allocate(_size);
}

template<typename T, size_t fixed_size> inline
Buffer<T, fixed_size>::Buffer(const Buffer<T, fixed_size>& abuf)
{
	ptr = buf;
	sz = fixed_size;
	allocate(abuf.size());
	for (size_t i = 0; i < sz; i++)
		ptr[i] = abuf.ptr[i];
}

template<typename T, size_t fixed_size> inline Buffer<T, fixed_size>&
Buffer<T, fixed_size>::operator = (const Buffer<T, fixed_size>& abuf)
{
	if (this != &abuf)
	{
		deallocate();
		allocate(abuf.size());
		for (size_t i = 0; i < sz; i++)
			ptr[i] = abuf.ptr[i];
	}
	return *this;
}

template<typename T, size_t fixed_size> inline
Buffer<T, fixed_size>::~Buffer()
{
	deallocate();
}

template<typename T, size_t fixed_size> inline void
Buffer<T, fixed_size>::allocate(size_t _size)
{
	if (_size <= sz)
	{
		sz = _size;
		return;
	}
	deallocate();
	if (_size > fixed_size)
	{
		ptr = new T[_size];
		sz = _size;
	}
}

template<typename T, size_t fixed_size> inline void
Buffer<T, fixed_size>::deallocate()
{
	if (ptr != buf)
	{
		delete[] ptr;
		ptr = buf;
		sz = fixed_size;
	}
}

template<typename T, size_t fixed_size> inline void
Buffer<T, fixed_size>::resize(size_t _size)
{
	if (_size <= sz)
	{
		sz = _size;
		return;
	}
	size_t i, prevsize = sz, minsize = MIN(prevsize, _size);
	T* prevptr = ptr;

	ptr = _size > fixed_size ? new T[_size] : buf;
	sz = _size;

	if (ptr != prevptr)
		for (i = 0; i < minsize; i++)
			ptr[i] = prevptr[i];
	for (i = prevsize; i < _size; i++)
		ptr[i] = T();

	if (prevptr != buf)
		delete[] prevptr;
}

template<typename T, size_t fixed_size> inline size_t
Buffer<T, fixed_size>::size() const
{
	return sz;
}

template<typename T, size_t fixed_size> inline
Buffer<T, fixed_size>::operator T* ()
{
	return ptr;
}

template<typename T, size_t fixed_size> inline
Buffer<T, fixed_size>::operator const T* () const
{
	return ptr;
}