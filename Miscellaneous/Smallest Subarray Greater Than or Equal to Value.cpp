#include <iostream>
#include <deque>
#include <vector>

template <typename T>
class Deque
{
    // A modified deque that allows incrementing
    //  (or decrementing) all values in it in O(1).

    T delta;
    std::deque<T> deque;

public:

    T front() const { return deque.front() + delta; }
    T back() const { return deque.back() + delta; }
    void pop_front() { return deque.pop_front(); }
    void pop_back() { return deque.pop_back(); }
    void push_front(const T& value) { return deque.push_front(value - delta); }
    void push_back(const T& value) { return deque.push_back(value - delta); }
    void increment_all_elements(const T& value) { delta += value; }
    bool empty() const { return deque.empty(); }

};

template <typename T>
struct Result
{
    int from;
    int to;
    T sum;
};

template <typename T>
Result<T> get_subarray(const std::vector<T>& array, const T& value)
{
    struct Node
    {
        // TODO get rid of this struct.

        int from;
        T sum = 0;

        Node() = default;
        Node(const T& sum) : sum(sum) {}
        Node(int from, const T& sum) : from(from), sum(sum) {}
        Node operator+(const T& value) const { return {from, sum + value}; }
        Node operator-(const T& value) const { return *this + (-value); }
        Node& operator+=(const T& value) { sum += value; return *this; }
        operator T() const { return sum; }
    };

    Result<T> result {-1, (int)array.size(), -1};

    // This deque has an invariant:
    //  for i < j => deque[i].sum > deque[j].sum.
    //  In other words, as you go towards the front
    //  of the deque, the sums increase.
    Deque<Node> deque;

    for (int i = 0; i < array.size(); i++)
    {
        // One each iteration, a node n in the deque
        //  represents the sum of the range [n.from, i].

        deque.increment_all_elements(array[i]);

        while (!deque.empty() && deque.back().sum <= array[i])
            deque.pop_back();

        deque.push_back({i, array[i]});

        while (!deque.empty() && deque.front().sum >= value) {
            auto node = deque.front();
            deque.pop_front();
            if ((i - node.from) < (result.to - result.from)) {
                result.from = node.from;
                result.to = i;
                result.sum = node.sum;
            }
        }
    }

    return result;
}

void test(const std::vector<int>& array, int value)
{
    auto result = get_subarray(array, value);

    std::cout << "Array: ";
    for (int i : array)
        std::cout << i << ' ';
    std::cout << std::endl;

    if (result.from == -1) {
        std::cout << "There is no subarray with sum >= " << value;
        std::cout << std::endl << std::endl;
        return;
    }

    std::cout << "A smallest subarray that has a sum >= ";
    std::cout << value << " is from " << result.from;
    std::cout << " to " << result.to << " with a sum of ";
    std::cout << result.sum << std::endl << std::endl;
}

int main()
{
    test({12, 5, -10, 20, 1, 3, -1, -3, -2, 1, 3, 5, -20}, 22);
    test({1, 1, -8, 1, 1, -8, 1, 1, -8}, 5);
}