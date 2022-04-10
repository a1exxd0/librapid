#pragma once

#include "config.hpp"

namespace librapid {
	namespace memory {
		template<typename T = u_char, typename d = device::CPU>
		class DenseStorage;
	}

	namespace packet {
		template<typename T>
		struct traits;
	}

	template<typename T = int64_t, int64_t maxDims = 32>
	class Extent;

	template<typename Derived, typename device>
	class ArrayBase;
} // namespace librapid
