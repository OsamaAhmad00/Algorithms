#include <iostream>
#include <complex>
#include <vector>
#include <cmath>
#include <algorithm>

template <typename T>
class OddEvenArrayView
{
    // This is a convenience class for splitting an array into
    int offset;
    int step_size;
    const std::vector<T>& array;

    OddEvenArrayView(const std::vector<T>& array, int offset, int step_size) :
        array(array), offset(offset), step_size(step_size) {}

    OddEvenArrayView double_step(int new_offset)  const { return OddEvenArrayView(array, new_offset, step_size*2); }

public:

    OddEvenArrayView(const std::vector<T>& array) : OddEvenArrayView(array, 0, 1) {}

    int size() const { return (array.size() - offset + step_size - 1) / step_size; }
    const T& operator[](int index) const { return array[offset + index * step_size]; }
    OddEvenArrayView split_odd ()  const { return double_step(offset + step_size); }
    OddEvenArrayView split_even()  const { return double_step(offset); }
};

template <typename T>
class FastFourierTransform
{
    using complex_type = double;
    using complex = std::complex<complex_type>;

    std::vector<complex> roots_of_unity;

    void compute_roots_of_unity(int n)
    {
        // TODO optimize this to not get computed again
        //  in case it was already computed.

        // root[k] = r * e^((2 * pi * i * k) / n). Here,
        //  r is the magnitude of the number, which is equal
        //  to 1 in our case, and (2 * pi * i * k) / n is the
        //  angle the root makes in radians. We can use the
        //  "std::polar" function to construct the complex
        //  number from its magnitude and angle.

        // These roots (roots of unity) has a special property:
        //  each time they get squared, they get eliminated by
        //  a half. This is because they always come in the form
        //  of positive/negative pairs, and when squared, both
        //  the positive and the negative roots will yield the
        //  same result.
        // We make use of this property to get the algorithm to
        //  run in O(n * log(n)).

        auto tau = M_PI * 2;
        auto base = std::polar<complex_type>(1, tau / n);
        roots_of_unity.resize(n);
        roots_of_unity[0] = 1;
        for (int i = 1; i < n; i++) {
            roots_of_unity[i] = roots_of_unity[i - 1] * base;
        }
    }

    static int biggest_power_of_two(int n)
    {
        int bits = 1;
        while ((1 << bits) < n) bits++;
        return 1 << bits;
    }

    template <typename U>
    std::vector<complex> compute(const OddEvenArrayView<U>& coefficients, int step = 1) const
    {
        // https://www.youtube.com/watch?v=h7apO7q16V0

        if (coefficients.size() == 1) return { (complex)coefficients[0] };

        // Note that the nth roots of unity are in the indices: 0, 1, 2, 3,  ... (x^1)
        //  and the  (n/2)th roots of unity are in the indices: 0, 2, 4, 8,  ... (x^2)
        //  and the  (n/4)th roots of unity are in the indices: 0, 4, 8, 16, ... (x^4)
        //  We make use of this fact to avoid making additional computations for
        //  each level, instead, we just use a "step" variable.

        // A polynomial A(x) = a0 + a1 * x + a2 * x^2 + a3 * x^3 + ...
        //  can be expressed in the form A(x) = E(x^2) + x * O(x^2)
        //  where E(x) = a0 + a2 * x + a4 * x^2 + a6 * x^3 + ...
        //  and   O(x) = a1 + a3 * x + a5 * x^2 + a7 * x^3 + ...
        // E(x) is a polynomial with the coefficients of the terms with
        //  an even degree in A(x) and O(x) is a polynomial with the
        //  coefficients of the terms with an odd degree in A(x).
        // Since we're using roots of unity, squaring eliminates half
        //  of the inputs, thus, we'll evaluate both E(x^2) and O(x^2)
        //  separately, and combine the results back. Here, "step*2"
        //  accounts for the squaring.
        auto result_odd  = compute(coefficients.split_odd(), step*2);
        auto result_even = compute(coefficients.split_even(), step*2);

        // This is the number of different arguments we need to evaluate at.
        int n = roots_of_unity.size() / step;
        std::vector<complex> result(n);

        for (int i = 0, root_index = 0; i < n/2; i++, root_index += step) {
            auto even = result_even[i];
            auto odd = roots_of_unity[root_index] * result_odd[i];
            result[i]     = even + odd;
            result[i+n/2] = even - odd;
        }

        return result;
    }

public:

    std::vector<complex> FFT(std::vector<T> coefficients, int arguments)
    {
        // To avoid complicating the code, we round both the number of
        //  coefficients and the number arguments to the nearest power of 2
        //  of the arguments.
        int n = biggest_power_of_two(arguments);
        compute_roots_of_unity(n);
        coefficients.resize(n, 0);
        auto view = OddEvenArrayView<T>(coefficients);
        return compute(view);
    }

    std::vector<T> inverse_FFT(const std::vector<complex>& result)
    {
        int n = result.size();
        compute_roots_of_unity(n);

        // When taking the inverse, the roots array should be in the form
        //  roots[k] = e^(-(i * tau * k) / n). Notice the negative in the
        //  exponent. This is the same as going through the computed roots
        //  backwards, but keep in mind that the first root is always 1.
        //  We'll keep the first item in the array as it is, and reverse
        //  the rest of the array.
        std::reverse(roots_of_unity.begin()+1, roots_of_unity.end());

        auto view = OddEvenArrayView<complex>(result);
        auto coefficients_complex = compute(view);

        // After the computation, the result array needs to get divided by n.
        // The imaginary component of the results will be zeros.
        //  Here, we're just constructing an array of type T
        //  instead of returning an array of complex numbers.
        // Note that we're rounding the number by adding 0.5.
        std::vector<T> coefficients(n);
        for (int i = 0; i < n; i++)
            coefficients[i] = (coefficients_complex[i].real() + 0.5) / n;
        return coefficients;
    }

    static std::vector<T> convolve(const std::vector<T> &a, const std::vector<T>& b)
    {
        int n = a.size() + b.size() - 1;
        int rounded_n = biggest_power_of_two(n);

        // Fact: a polynomial P with degree n can be uniquely identified
        //  by n+1 points in the form (x, P(x)), in which all x's are
        //  unique. This should feel intuitive. A polynomial is uniquely
        //  identified by its coefficients. In the case of the points,
        //  the coefficients are unknown (there are n+1 of them), we
        //  have n+1 linear equations (the n+1 points), thus, we have
        //  a single solution to the system of equations. The x's are
        //  required to be unique to make sure that we don't have coincident
        //  lines, thus having a single unique solution.

        // Converting the polynomials from the "Coefficients" form to
        //  a "Point-Value" form. Multiplication in this form takes
        //  O(n) instead of O(n^2). The conversion takes O(n * log(n)).
        FastFourierTransform<T> fft;
        auto a_fft = fft.FFT(a, rounded_n);
        auto b_fft = fft.FFT(b, rounded_n);

        // Multiplying the points.
        // For polynomials A and B, if A(x) = y1, B(x) = y2,
        //  C(x) = y1 * y2 for C = A * B.
        for (int i = 0; i < rounded_n; i++)
            a_fft[i] *= b_fft[i];

        // Converting back to the coefficients form.
        auto result = fft.inverse_FFT(a_fft);

        // Popping back the unnecessary padding zeros at the end.
        result.resize(n);

        return result;
    }
};

void test(const std::vector<int>& coefficients_a, const std::vector<int>& coefficients_b)
{
    auto result = FastFourierTransform<int>::convolve(coefficients_a, coefficients_b);
    for (auto& i : result)
        std::cout << i << ' ';
    std::cout << std::endl;
}

int main()
{
    test({1, 2}, {3, 4});
    test({1, 2, 3, 4}, {5, 6, 7, 8});
    test({1, 2, 3, 4}, {5, 6});
}