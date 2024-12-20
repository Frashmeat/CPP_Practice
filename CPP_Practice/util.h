#pragma once
#include <atomic>


namespace util {

	template<typename To, typename From>
	struct same_constness {
		using type = To;
	};

	template<typename To, typename From>
	struct same_constness<To, From const> {
		using type = To const;
	};

	template<typename To, typename From>
	using same_constness_t = same_constness<To, From>::type;


#ifdef LONG_ID_TYPE
	using IDType = uint64_t;
#else
	using IDType = uint32_t;
#endif	


	class non_type {
	public:
		template <typename Ty, auto Param>
		[[nodiscard]] static IDType getId() {
			static IDType typeId = non_type::typeId++;
			return typeId;
		}

	private:
		static inline std::atomic<IDType> typeId;
	};

	template<auto>
	struct Spreader{
		explicit Spreader() = default;
	};

	template<auto Candidate>
	constexpr Spreader<Candidate> spreadArg{};
};
