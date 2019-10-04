#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma once

#include "meta.hpp"
#include <atomic>
#include <new>
#include <functional>

template<typename T>
class my_smart_ptr final {
	static_assert(
	        !std::is_reference<T>::value,
	        "template argument T of class my_smart_ptr<T> can not be of reference type\n"
            "e.g. use my_smart_ptr<T> for T* and my_smart_ptr<T*> for T**"
    );

	template<typename>
	friend class my_smart_ptr;

private:
	T* raw_ptr = nullptr;
	std::atomic_uint* ref_count = nullptr;

	void inc_count() noexcept {
		if(ref_count)
			*ref_count += 1;
	}

	void dec_count() noexcept(std::is_nothrow_destructible<T>::value) {
		if (ref_count) {
			*ref_count -= 1;

			if (*ref_count == 0) {
				delete raw_ptr;
				delete ref_count;
			}
		}
	}

public:
	using type = T;
	using ptr_type = T*;

	my_smart_ptr() = default;

	explicit my_smart_ptr(std::nullptr_t) :
			my_smart_ptr{}
	{
	    //noop
	}

	explicit my_smart_ptr(T* ptr) :
	        raw_ptr{ptr},
            ref_count{new(std::nothrow) std::atomic_uint{1}}
	{
		if (ref_count == nullptr) {
		    delete raw_ptr;
		    throw std::bad_alloc{};
		}
	}

	template<typename S, typename std::enable_if<meta::is_subtype<T, S>::value, int>::type = 0>
	explicit my_smart_ptr(S* ptr) try :
			my_smart_ptr{dynamic_cast<T>(ptr)}
	{
		// noop
	}
	catch (const std::bad_cast& e) {
		delete ptr;
		throw e;
	}

	my_smart_ptr(const my_smart_ptr<T>& other) :
			raw_ptr{other.raw_ptr},
			ref_count{other.ref_count}
	{
		inc_count();
	}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-explicit-conversions"
	template<typename S, typename std::enable_if<meta::is_subtype<T, S>::value, int>::type = 0>
	my_smart_ptr(const my_smart_ptr<S>& other) :
			raw_ptr{dynamic_cast<T*>(other.raw_ptr)},
			ref_count{other.ref_count}
	{
		inc_count();
	}
#pragma clang diagnostic pop

	my_smart_ptr(my_smart_ptr<T>&& other) noexcept :
			my_smart_ptr{}
	{
		swap(*this, other);
		std::cout << "move";
	}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-explicit-conversions"
	template<typename S, typename std::enable_if<meta::is_subtype<T, S>::value, int>::type = 0>
	my_smart_ptr(my_smart_ptr<S>&& other) :
			my_smart_ptr{}
	{
		swap(*this, other);
	}
#pragma clang diagnostic pop

	~my_smart_ptr() noexcept(std::is_nothrow_destructible<T>::value) {
		dec_count();
	}

	my_smart_ptr& operator=(std::nullptr_t) {
		swap(*this, my_smart_ptr{nullptr});
		return *this;
	}

	my_smart_ptr& operator=(T* ptr) {
		swap(*this, my_smart_ptr{ptr});
		return *this;
	}

	template<typename S, typename std::enable_if<meta::is_subtype<T, S>::value, int>::type = 0>
	my_smart_ptr& operator=(S* ptr) {
		swap(*this, my_smart_ptr{dynamic_cast<T>(ptr)});
		return *this;
	}

	my_smart_ptr& operator=(my_smart_ptr<T> other) {
		swap(*this, other);
		return *this;
	}

	my_smart_ptr& operator=(my_smart_ptr<T>&& other) noexcept {
		swap(*this, other);
		return *this;
	}

	//inplace object construction
	template<typename... arg_types>
	[[nodiscard]]
	static my_smart_ptr<T> make_shared(arg_types&&... args) {
		return my_smart_ptr<T>(new T(args...));
	}

	friend void swap(my_smart_ptr& first, my_smart_ptr& second) noexcept {
		using std::swap;
		swap(first.raw_ptr, second.raw_ptr);
		swap(first.ref_count, second.ref_count);
	}

	[[nodiscard]] bool isUnique() const noexcept {
		return ref_count ? *ref_count == 1 : false;
	}

	[[nodiscard]] bool operator==(const T* other) const noexcept {
		return raw_ptr == other;
	}

	[[nodiscard]] bool operator==(const my_smart_ptr<T>& other) const noexcept {
		return raw_ptr == other.raw_ptr;
	}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-explicit-conversions"
	explicit operator bool() const noexcept {
		return static_cast<bool>(raw_ptr);
	}

	explicit operator T*() const noexcept {
		return raw_ptr;
	}
#pragma clang diagnostic pop

	T* operator->() const noexcept {
		return raw_ptr;
	}

	T& operator*() const noexcept {
		return *raw_ptr;
	}
};

#pragma clang diagnostic pop