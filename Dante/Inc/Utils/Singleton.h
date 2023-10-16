#pragma once
namespace Dante::Utils
{
	template <typename T> 
	class Singleton
	{
	protected:

		explicit Singleton()
		{
			if (instance)
			{
				throw std::runtime_error
				(std::format(
					"Instance already create for type {}.", typeid(T).name()
				));
			}
			instance = static_cast<T*>(this);
		}

		virtual ~Singleton()
		{
			instance = nullptr;
		}
	public:
		static bool Exist()
		{
			return instance ? 1 : 0;
		}
		static T& Instance()
		{
			if (!instance)
			{
				throw std::runtime_error(std::format("Instance not availabe for {}", typeid(T).name()));
			}
			return *instance;
		}
	private:
		Singleton(const Singleton& other) = delete;
		Singleton& operator=(const Singleton& other) = delete;

		Singleton(Singleton&& other) = delete;
		Singleton& operator=(Singleton&& other) = delete;


	private:
		static inline T* instance{};

	};
}