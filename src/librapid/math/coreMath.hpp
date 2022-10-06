#ifndef LIBRAPID_CORE_MATH
#define LIBRAPID_CORE_MATH

namespace librapid {
	template<typename T>
	LR_INLINE T product(const std::vector<T> &vals) {
		T res = 1;
		for (const auto &val : vals) res *= val;
		return res;
	}

	template<typename T>
	LR_INLINE T min(const std::vector<T> &vals) {
		T min_found = 0;
		for (const auto &val : vals)
			if (val < min_found) min_found = val;
		return min_found;
	}

	template<typename T>
	LR_INLINE T &&min(T &&val) {
		return std::forward<T>(val);
	}

	template<typename T0, typename T1, typename... Ts>
	LR_INLINE auto min(T0 &&val1, T1 &&val2, Ts &&...vs) {
		return (val1 < val2) ? min(val1, std::forward<Ts>(vs)...)
							 : min(val2, std::forward<Ts>(vs)...);
	}

	template<typename T>
	LR_INLINE T max(const std::vector<T> &vals) {
		T min_found = 0;
		for (const auto &val : vals)
			if (val > min_found) min_found = val;
		return min_found;
	}

	template<typename T>
	LR_INLINE T &&max(T &&val) {
		return std::forward<T>(val);
	}

	template<typename T0, typename T1, typename... Ts>
	LR_INLINE auto max(T0 &&val1, T1 &&val2, Ts &&...vs) {
		return (val1 > val2) ? max(val1, std::forward<Ts>(vs)...)
							 : max(val2, std::forward<Ts>(vs)...);
	}

#define LR_UNARY_MATH_OP(NAME_)                                                                    \
	template<typename T, typename std::enable_if_t<internal::traits<T>::IsScalar, int> = 0>        \
	LR_INLINE auto NAME_(const T &a) {                                                             \
		using Scalar = typename std::conditional_t<isMultiprecision<T>(), mpfr, f64>;              \
		if constexpr (Vc::is_simd_vector<T>::value)                                                \
			return std::NAME_(a);                                                                  \
		else                                                                                       \
			return std::NAME_(static_cast<Scalar>(a));                                             \
	}

#define LR_UNARY_MATH_OP_RECIP(NAME_, OP_)                                                         \
	template<typename T, typename std::enable_if_t<internal::traits<T>::IsScalar, int> = 0>        \
	LR_INLINE typename std::conditional_t<isMultiprecision<T>(), mpfr, f64> NAME_(const T &a) {    \
		using Scalar = typename std::conditional_t<isMultiprecision<T>(), mpfr, f64>;              \
		return Scalar(1) / std::OP_(static_cast<Scalar>(a));                                       \
	}

	LR_UNARY_MATH_OP(abs)
	LR_UNARY_MATH_OP(floor)
	LR_UNARY_MATH_OP(ceil)

	LR_UNARY_MATH_OP(sqrt)
	LR_UNARY_MATH_OP(exp)
	LR_UNARY_MATH_OP(exp2)

	LR_UNARY_MATH_OP(log)
	LR_UNARY_MATH_OP(log2)
	LR_UNARY_MATH_OP(log10)

	LR_UNARY_MATH_OP(sin)
	LR_UNARY_MATH_OP(cos)
	LR_UNARY_MATH_OP(tan)

	LR_UNARY_MATH_OP(asin)
	LR_UNARY_MATH_OP(acos)
	LR_UNARY_MATH_OP(atan)

	LR_UNARY_MATH_OP_RECIP(csc, sin)
	LR_UNARY_MATH_OP_RECIP(sec, cos)
	LR_UNARY_MATH_OP_RECIP(cot, tan)

	LR_UNARY_MATH_OP_RECIP(acsc, asin)
	LR_UNARY_MATH_OP_RECIP(asec, acos)
	LR_UNARY_MATH_OP_RECIP(acot, atan)

	LR_UNARY_MATH_OP(sinh)
	LR_UNARY_MATH_OP(cosh)
	LR_UNARY_MATH_OP(tanh)

	LR_UNARY_MATH_OP(asinh)
	LR_UNARY_MATH_OP(acosh)
	LR_UNARY_MATH_OP(atanh)

	// Functions which could not be auto generated by a simple macro

	template<typename A, typename B>
	LR_INLINE auto pow(const A &a, const B &b) {
		return std::pow(a, b);
	}

	template<typename T>
	LR_INLINE T exp10(const T &a) {
		return pow(T(10), a);
	}

	// Return a * 2 ^ exponent
	template<typename T>
	LR_INLINE T ldexp(const T &a, int exponent) {
		// Use static_cast to ensure we get the most precision possible, even for integer types
		return static_cast<T>(std::ldexp(static_cast<f64>(a), exponent));
	}

	template<typename T, typename B>
	LR_INLINE auto log(const T &a, const B &base) {
		using Scalar =
		  typename std::conditional_t<isMultiprecision<T>() || isMultiprecision<B>(), mpfr, f64>;
		if constexpr (internal::traits<T>::IsScalar && internal::traits<B>::IsScalar) {
			return log(static_cast<Scalar>(a)) / log(static_cast<Scalar>(base));
		} else {
			return log(a) / log(base);
		}
	}

	template<typename T>
	LR_INLINE T atan2(const T &a, const T &b) {
		return std::atan2(a, b);
	}

#if defined(LIBRAPID_USE_VC)
	// SIMD vector tan
	template<typename T, typename Set>
	LR_INLINE auto tan(const Vc::Vector<T, Set> &vec) {
		return sin(vec) / cos(vec);
	}

	// SIMD vector asin
	template<typename T, typename Set>
	LR_INLINE auto asin(const Vc::Vector<T, Set> &vec) {
		return atan2(vec, sqrt(Vc::Vector<T, Set>(1) - vec * vec));
	}

	// SIMD vector acos
	template<typename T, typename Set>
	LR_INLINE auto acos(const Vc::Vector<T, Set> &vec) {
		return atan2(sqrt(Vc::Vector<T, Set>(1) - vec * vec), vec);
	}

	// SIMD vector atan
	template<typename T, typename Set>
	LR_INLINE auto atan(const Vc::Vector<T, Set> &vec) {
		return atan2(vec, Vc::Vector<T, Set>(1));
	}

	// SIMD vector sinh
	template<typename T, typename Set>
	LR_INLINE auto sinh(const Vc::Vector<T, Set> &vec) {
		return (exp(vec) - exp(-vec)) / T(2);
	}

	// SIMD vector cosh
	template<typename T, typename Set>
	LR_INLINE auto cosh(const Vc::Vector<T, Set> &vec) {
		return (exp(vec) + exp(-vec)) / T(2);
	}

	// SIMD vector tanh
	template<typename T, typename Set>
	LR_INLINE auto tanh(const Vc::Vector<T, Set> &vec) {
		return sinh(vec) / cosh(vec);
	}

	// SIMD vector asinh
	template<typename T, typename Set>
	LR_INLINE auto asinh(const Vc::Vector<T, Set> &vec) {
		return log(vec + sqrt(vec * vec + Vc::Vector<T, Set>(1)));
	}

	// SIMD vector acosh
	template<typename T, typename Set>
	LR_INLINE auto acosh(const Vc::Vector<T, Set> &vec) {
		return log(vec + sqrt(vec * vec - Vc::Vector<T, Set>(1)));
	}

	// SIMD vector atanh
	template<typename T, typename Set>
	LR_INLINE auto atanh(const Vc::Vector<T, Set> &vec) {
		return log((Vc::Vector<T, Set>(1) + vec) / (Vc::Vector<T, Set>(1) - vec)) / T(2);
	}
#endif

	template<typename T>
	LR_INLINE T hypot(const T &a, const T &b) {
		return std::hypot(a, b);
	}

	template<typename T>
	LR_INLINE T map(T val, T start1, T stop1, T start2, T stop2) {
		return start2 + (stop2 - start2) * ((val - start1) / (stop1 - start1));
	}

	LR_INLINE f64 trueRandomEntropy() {
		static std::random_device rd;
		return rd.entropy();
	}

	template<typename T = f64>
	LR_INLINE f64 trueRandom(T lower = 0, T upper = 1) {
		// Truly random value in range [lower, upper)
		static std::random_device rd;
		std::uniform_real_distribution<f64> dist((f64)lower, (f64)upper);
		return dist(rd);
	}

	LR_INLINE f64 random(f64 lower = 0, f64 upper = 1, ui64 seed = -1) {
		// Random floating point value in range [lower, upper)

		// Seed generation
		static auto tmpSeed = (ui64)now<time::microsecond>();
		if (seed != -1) tmpSeed = seed;

		static std::uniform_real_distribution<f64> distribution(0., 1.);
		static std::mt19937 generator(tmpSeed);
		return lower + (upper - lower) * distribution(generator);
	}

	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, int> = 0>
	LR_INLINE T randint(T lower, T upper, ui64 seed = -1) {
		// Random integral value in range [lower, upper]
		return (i64)random((f64)(lower - (lower < 0 ? 1 : 0)), (f64)upper + 1, seed);
	}

	LR_INLINE i64 trueRandint(i64 lower = 0, i64 upper = 1) {
		// Truly random value in range [lower, upper)
		return (i64)trueRandom((f64)(lower - (lower < 0 ? 1 : 0)), (f64)upper + 1);
	}

	/**
	 * Adapted from
	 * https://docs.oracle.com/javase/6/docs/api/java/util/Random.html#nextGaussian()
	 */
	LR_INLINE f64 randomGaussian() {
		static f64 nextGaussian;
		static bool hasNextGaussian = false;

		f64 res;
		if (hasNextGaussian) {
			hasNextGaussian = false;
			res				= nextGaussian;
		} else {
			f64 v1, v2, s;
			do {
				v1 = 2 * random() - 1; // between -1.0 and 1.0
				v2 = 2 * random() - 1; // between -1.0 and 1.0
				s  = v1 * v1 + v2 * v2;
			} while (s >= 1 || s == 0);
			f64 multiplier	= sqrt(-2 * log(s) / s);
			nextGaussian	= v2 * multiplier;
			hasNextGaussian = true;
			res				= v1 * multiplier;
		}

		return res;
	}

	template<typename T = f64>
	LR_INLINE auto pow10(i64 exponent) {
		using Scalar = typename internal::traits<T>::Scalar;

		const static Scalar pows[] = {
		  0.00001, 0.0001, 0.001, 0.01, 0.1, 1, 10, 100, 1000, 10000, 100000};
		if (exponent >= -5 && exponent <= 5) return pows[exponent + 5];

		Scalar res = 1;

		if (exponent > 0)
			for (i64 i = 0; i < exponent; i++) res *= T(10);
		else
			for (i64 i = 0; i > exponent; i--) res *= T(0.1);

		return res;
	}

	template<typename T1, typename T2,
			 typename std::enable_if_t<std::is_integral_v<T1> && std::is_integral_v<T2>, int> = 0>
	LR_INLINE auto mod(T1 val, T2 divisor) {
		return val % divisor;
	}

	template<
	  typename T1, typename T2,
	  typename std::enable_if_t<(std::is_fundamental_v<T1> && std::is_fundamental_v<T2>)&&(
								  std::is_floating_point_v<T1> || std::is_floating_point_v<T2>),
								int> = 0>
	LR_INLINE auto mod(T1 val, T2 divisor) {
		return std::fmod(val, divisor);
	}

	template<typename A, typename B>
	LR_INLINE auto mod(const Complex<A> &a, const Complex<B> &b) {
		return Complex<decltype(::librapid::mod(a.real(), b.real()))>(
		  ::librapid::mod(a.real(), b.real()), ::librapid::mod(a.imag(), b.imag()));
	}

	template<typename A, typename B>
	LR_INLINE auto fmod(const Complex<A> &a, const Complex<B> &b) {
		return mod(a, b);
	}

	namespace roundMode {
		// Rounding Mode Information:
		// Bit mask:
		// [0] -> Round up if difference >= 0.5
		// [1] -> Round up if difference < 0.5
		// [2] -> Round to nearest even
		// [3] -> Round to nearest odd
		// [4] -> Round only if difference == 0.5

		static constexpr int8_t UP		  = 0b00000011;
		static constexpr int8_t DOWN	  = 0b00000000;
		static constexpr int8_t TRUNC	  = 0b00000000;
		static constexpr int8_t HALF_EVEN = 0b00010100;
		static constexpr int8_t MATH	  = 0b00000001;
	} // namespace roundMode

	template<typename T = f64>
	LR_INLINE auto round(T num, i64 dp = 0, int8_t mode = roundMode::MATH) {
		using Scalar = typename internal::traits<T>::Scalar;

		const f64 alpha	 = ::librapid::pow10(dp);
		const f64 beta	 = ::librapid::pow10(-dp);
		const f64 absNum = ::librapid::abs(static_cast<f64>(num) * alpha);
		f64 y			 = ::librapid::floor(absNum);
		f64 diff		 = absNum - y;
		if (mode & (1 << 0) && diff >= 0.5) y += 1;
		if (mode & (1 << 2)) {
			auto integer	 = (ui64)y;
			auto nearestEven = (integer & 1) ? (y + 1) : (f64)integer;
			if (mode & (1 << 4) && diff == 0.5) y = nearestEven;
		}

		return static_cast<T>(internal::copySign(y * beta, num));
	}

	template<typename T>
	LR_INLINE auto round(const Complex<T> &num, i64 dp = 0, int8_t mode = roundMode::MATH) {
		return Complex<T>(round(real(num), dp, mode), round(imag(num), dp, mode));
	}

#if defined(LIBRAPID_USE_MULTIPREC)
	template<>
	LR_INLINE auto round(const mpfr &num, i64 dp, int8_t mode) {
		using Scalar = mpfr;

		const Scalar alpha	= ::librapid::exp10(mpfr(dp));
		const Scalar beta	= ::librapid::exp10(mpfr(-dp));
		const Scalar absNum = ::librapid::abs(num * alpha);
		Scalar y			= ::librapid::floor(absNum);
		Scalar diff			= absNum - y;
		if (mode & (1 << 0) && diff >= 0.5) y += 1;
		if (mode & (1 << 2)) {
			auto integer	 = (ui64)y;
			auto nearestEven = (integer & 1) ? (y + 1) : (Scalar)integer;
			if (mode & (1 << 4) && diff == 0.5) y = nearestEven;
		}
		return (num >= 0 ? y : -y) * beta;
	}
#endif

	template<typename T1 = f64, typename T2 = f64>
	LR_INLINE T2 roundTo(T1 num, T2 val) {
		if (num == static_cast<T1>(0)) return 0;
		T2 rem = ::librapid::mod(::librapid::abs(static_cast<T2>(num)), val);
		if (rem >= val / static_cast<T2>(2))
			return internal::copySign((::librapid::abs(static_cast<T2>(num)) + val) - rem, num);
		return internal::copySign(static_cast<T2>(num) - rem, num);
	}

	template<typename T1, typename T2>
	LR_INLINE Complex<T2> roundTo(const Complex<T1> &num, T2 val) {
		return Complex<T2>(roundTo(real(num), val), roundTo(imag(num), val));
	}

	template<typename T1 = f64, typename T2 = f64>
	LR_INLINE Complex<T2> roundTo(const Complex<T1> &num, const Complex<T2> &val) {
		return Complex<T2>(roundTo(real(num), real(val)), roundTo(imag(num), imag(val)));
	}

	template<typename T1 = f64, typename T2 = f64>
	LR_INLINE T2 roundUpTo(T1 num, T2 val) {
		T2 rem = ::librapid::mod(T2(num), val);
		if (rem == T2(0)) return static_cast<T2>(num);
		return (static_cast<T2>(num) + val) - rem;
	}

	template<typename T1 = f64, typename T2 = f64>
	LR_INLINE Complex<T1> roundUpTo(const Complex<T1> &num, T2 val) {
		return Complex<T1>(roundUpTo(real(num), val), roundUpTo(imag(num), val));
	}

	template<typename T1 = f64, typename T2 = f64>
	LR_INLINE Complex<T1> roundUpTo(const Complex<T1> &num, const Complex<T2> &val) {
		return Complex<T1>(roundUpTo(real(num), real(val)), roundUpTo(imag(num), imag(val)));
	}

	template<typename T>
	LR_INLINE T roundSigFig(T num, i64 figs = 3) {
		LR_ASSERT(figs > 0,
				  "Cannot round to {} significant figures. Value must be greater than zero",
				  figs);

		using Scalar = std::conditional_t<std::is_floating_point_v<T>, f64, T>;

		if (num == static_cast<T>(0)) return static_cast<T>(0);

		auto tmp = ::librapid::abs(static_cast<Scalar>(num));
		i64 n	 = 0;

		const auto ten = static_cast<Scalar>(10);
		const auto one = static_cast<Scalar>(1);
		while (tmp > ten) {
			tmp /= ten;
			++n;
		}

		while (tmp < one) {
			tmp *= ten;
			--n;
		}

		return internal::copySign(static_cast<T>(round(tmp, figs - 1) * pow10<Scalar>(n)), num);
	}

	template<typename T>
	LR_INLINE Complex<T> roundSigFig(const Complex<T> &num, i64 figs = 3) {
		return Complex<T>(roundSigFig(real(num), figs), roundSigFig(imag(num), figs));
	}

	template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	LR_INLINE T lerp(T _a, T _b, T _t) {
		if (std::isnan(_a) || std::isnan(_b) || std::isnan(_t))
			return std::numeric_limits<T>::quiet_NaN();
		else if ((_a <= T {0} && _b >= T {0}) || (_a >= T {0} && _b <= T {0}))
		// ab <= 0 but product could overflow.
#ifndef FMA
			return _t * _b + (T {1} - _t) * _a;
#else
			return std::fma(_t, _b, (_Float {1} - _t) * _a);
#endif
		else if (_t == T {1})
			return _b;
		else { // monotonic near t == 1.
#ifndef FMA
			const auto _x = _a + _t * (_b - _a);
#else
			const auto _x = std::fma(_t, _b - _a, _a);
#endif
			return (_t > T {1}) == (_b > _a) ? max(_b, _x) : min(_b, _x);
		}
	}

	template<
	  typename A, typename B, typename C,
	  typename std::enable_if_t<!std::is_floating_point_v<A> || !std::is_floating_point_v<B> ||
								  !std::is_floating_point_v<C>,
								int> = 0>
	LR_INLINE auto lerp(A _a, B _b, C _t) {
		return _a + _t * (_b - _a);
	}

	template<typename T>
	T clamp(T x, T lower, T upper) {
		if (x < lower) return lower;
		if (x > upper) return upper;
		return x;
	}

	template<typename T>
	Complex<T> clamp(Complex<T> x, Complex<T> lower, Complex<T> upper) {
		return Complex<T>(clamp(real(x), real(lower), real(upper)),
						  clamp(imag(x), imag(lower), imag(upper)));
	}

	template<typename T>
	T smoothStep(T edge0, T edge1, T x) {
		// Scale, and clamp x to 0..1 range
		x = clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
		// Evaluate polynomial
		return x * x * x * (x * (x * 6 - 15) + 10);
	}

	template<typename T>
	Complex<T> smoothStep(Complex<T> edge0, Complex<T> edge1, Complex<T> x) {
		return Complex<T>(smoothStep(real(edge0), real(edge1), real(x)),
						  smoothStep(imag(edge0), imag(edge1), imag(x)));
	}
} // namespace librapid

#endif // LIBRAPID_CORE_MATH