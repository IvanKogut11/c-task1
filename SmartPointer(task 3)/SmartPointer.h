template <typename T>
class SmartPointer
{
	size_t* counter_ = 0;
	T* pointer_;

public:

	explicit SmartPointer()
	{
		pointer_ = 0;
		Set(pointer_);
	}

	explicit SmartPointer(T* V) : pointer_(V)
	{
		Set(pointer_);
	}

	~SmartPointer()
	{
		Release();
	}

	SmartPointer& operator->() const
	{
		return pointer_;
	}

	SmartPointer& operator*() const
	{
		return pointer_;
	}

	SmartPointer& operator=(const SmartPointer& Other)
	{
		if (this == &Other)
			return *this;

		Release();
		return *this;
	}

	void Set(T* Ptr)
	{
		counter_ = new size_t();
		(*counter_)++;
	}

	T* Get() const
	{
		return pointer_;
	}

	void Release()
	{
		if (!counter_)
		{
			if (pointer_)
				delete pointer_;
			pointer_ = 0;
			return;
		}

		size_t nxt_counter = --(*counter_);
		if (nxt_counter == 0)
		{
			delete counter_;
			if (pointer_)
				delete pointer_;
		}

		counter_ = 0;
		pointer_ = 0;
	}
};
