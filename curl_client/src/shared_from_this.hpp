//
// Created by Andrey Solovyev on 01.05.2024.
//

#pragma once

#include <memory>
#include <type_traits>
#include <concepts>

namespace utility {

	template <typename T>
	requires requires {std::is_object_v<T>;}
	struct shared_from_this : public std::enable_shared_from_this<T> {

		virtual ~shared_from_this() = default;

		template<typename... Args>
		requires std::constructible_from<T, Args...>
		static std::shared_ptr<T> create(Args&&... args) {
			return std::make_shared<T>(std::forward<Args>(args)...);
		}

		std::shared_ptr<T> get_shared() {
			return std::enable_shared_from_this<T>::shared_from_this();
		}

	};

}//!namespace
