#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <random>

template <typename T>
using Matrix = std::vector<std::vector<T>>;

struct Point
{
    int x;
    int y;

    bool operator==(const Point& point) const {
        return x == point.x && y == point.y;
    }

    operator std::string() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

template <typename T>
class PrefixSum2D
{
    std::vector<std::vector<T>> sums;

public:

    PrefixSum2D(const std::vector<std::vector<T>>& array)
            : sums(array.size() + 1, std::vector<T>(array[0].size() + 1))
    {
        for (int i = 0; i <= array.size(); i++) {
            sums[i][0] = 0;
        }

        for (int j = 0; j <= array[0].size(); j++) {
            sums[0][j] = 0;
        }

        for (int i = 0; i < array.size(); i++) {
            for (int j = 0; j < array[0].size(); j++) {
                sums[i + 1][j + 1] = sums[i + 1][j] + array[i][j];
            }
        }

        for (int i = 2; i <= array.size(); i++) {
            for (int j = 1; j <= array[0].size(); j++) {
                sums[i][j] += sums[i-1][j];
            }
        }
    }

    T query(Point p1, Point p2)
    {
        if (p1.x > p2.x)
            std::swap(p1.x, p2.x);
        if (p1.y > p2.y)
            std::swap(p1.y, p2.y);

        return   sums[p2.x + 1][p2.y + 1]
                 - (sums[p1.x][p2.y + 1] + sums[p2.x + 1][p1.y])
                 +  sums[p1.x][p1.y];
    }
};

template <typename T>
class MaxSumSubmatrix
{

public:

    struct Range
    {
        Point from;
        Point to;

        int height() const { return to.x - from.x + 1; }
        int width() const { return to.y - from.y + 1; }

        std::pair<Range, Range> split() const
        {
            Range r1;
            Range r2;

            // The splitting here is a bit more complicated
            // than the maximum sum subarray problem.

            if (height() > width()) {
                int x_mid = (from.x + to.x) / 2;
                r1 = {from, {x_mid, to.y}};
                r2 = {{x_mid + 1, from.y}, to};
            } else {
                int y_mid = (from.y + to.y) / 2;
                r1 = {from, {to.x, y_mid}};
                r2 = {{from.x, y_mid + 1}, to};
            }

            return {r1, r2};
        }

        int size() const { return height() * width(); }

        Range() = default;

        Range(const Point& p1, const Point& p2) : from(p1), to(p2)
        {
            if (from.x > to.x) std::swap(from.x, to.x);
            if (from.y > to.y) std::swap(from.y, to.y);
        }

        operator std::string() const {
            return "{" + (std::string)from + ", " + (std::string)to + "}";
        }
    };

private:

    struct Node
    {
        Range start;
        Range end;

        int max_sum;

        bool can_branch() const {
            // If the whole node represents a single range, then
            //  it can't branch anymore.
            return !(start.from.x == start.to.x &&
                     start.from.y == start.to.y &&
                     end.from.x == end.to.x &&
                     end.from.y == end.to.y);
        }

        bool is_valid() const { return start.from.x <= end.to.x && start.from.y <= end.to.y; }

        bool operator <(const Node& node) const { return max_sum < node.max_sum; }

        operator std::string() const {
            return "{" + (std::string)start + ", " + (std::string)end +
                   ", max_sum = " + std::to_string(max_sum) + "}";
        }
    };

    int n, m;
    PrefixSum2D<T> prefix_sum;
    // Prefix sum of the array where all
    // negative numbers are set to 0.
    PrefixSum2D<T> positive_prefix_sum;

    std::priority_queue<Node> queue;

    Matrix<T> get_positive_matrix(const Matrix<T>& matrix)
    {
        Matrix<T> result(matrix);
        for (auto& row : result)
            for (auto& number : row)
                if (number < 0) number = 0;
        return result;
    }

    void add_to_queue(const Node& node)
    {
        if (node.is_valid())
            queue.push(node);
    }

    bool is_intersecting(const Range& r1, const Range& r2) {
        return r1.to.x > r2.from.x || r1.to.y > r2.from.y;
    }

    int compute_max_sum(const Node& node)
    {
        // The code and the idea here is exactly the
        // same as in the maximum subarray problem.

        int sum = positive_prefix_sum.query(node.start.from, node.end.to);
        if (!is_intersecting(node.start, node.end)) {
            sum -= positive_prefix_sum.query(node.start.to, node.end.from);
            sum += prefix_sum.query(node.start.to, node.end.from);
        }
        return sum;
    }

    std::pair<Node, Node> branch_node(const Node& node)
    {
        // TODO is there a better way of doing this?

        // The code and the idea here is exactly the
        // same as in the maximum subarray problem.

        auto& start = node.start;
        auto& end = node.end;

        std::pair<Node, Node> result;

        if (start.size() > end.size()) {
            auto starts = start.split();
            result = {{starts.first, end}, {starts.second, end}};
        } else {
            auto ends = end.split();
            result = {{start, ends.first}, {start, ends.second}};
        }

        result.first.max_sum = compute_max_sum(result.first);
        result.second.max_sum = compute_max_sum(result.second);

//        std::cout << "Branched " << (std::string)node << " to " << std::endl;
//        std::cout << "         " << (std::string)result.first << " and " << std::endl;
//        std::cout << "         " << (std::string)result.second << std::endl;

        return result;
    }


public:

    MaxSumSubmatrix(const Matrix<T>& array)
        : prefix_sum(array), positive_prefix_sum(get_positive_matrix(array))
        , n(array.size()), m(array[0].size()) {}

    struct Result
    {
        int sum;
        Range range;
    };

    Result compute()
    {
        // The code and the idea here is exactly the
        // same as in the maximum subarray problem.

        Range whole_matrix = {{0, 0}, {n - 1, m - 1}};
        Node root = {whole_matrix, whole_matrix};
        root.max_sum = compute_max_sum(root);

        queue.push(root);

        while (!queue.empty())
        {
            Node node = queue.top();
            queue.pop();

            if (!node.can_branch()) {
                return {node.max_sum, {node.start.from, node.end.to}};
            }

            auto split = branch_node(node);
            add_to_queue(split.first);
            add_to_queue(split.second);
        }

        // Shouldn't reach this case.
        return {-1, {{-1, -1}, {-1, -1}}};
    }
};

template <typename T>
class SlowerMaxSumSubmatrix
{

    const Matrix<T>& matrix;
    PrefixSum2D<T> prefix_sum;

public:

    struct Range
    {
        Point from;
        Point to;
        T sum;
    };

    Range compute()
    {
        Range result = {{0, 0}, {0, 0}, matrix[0][0]};

        for (int x1 = 0; x1 < matrix.size(); x1++) {
            for (int y1 = 0; y1 < matrix[0].size(); y1++) {
                for (int x2 = x1; x2 < matrix.size(); x2++) {
                    for (int y2 = y1; y2 < matrix[0].size(); y2++)
                    {
                        Point p1 = {x1, y1};
                        Point p2 = {x2, y2};
                        T sum = prefix_sum.query(p1, p2);

                        if (sum > result.sum) {
                            result = {p1, p2, sum};
                        }
                    }
                }
            }
        }

        return result;
    }

    SlowerMaxSumSubmatrix(const Matrix<T>& matrix)
        : matrix(matrix), prefix_sum(matrix) {}
};

template <typename T>
Matrix<T> get_random_matrix(int height, int width)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 200 + 1);

    Matrix<int> matrix(height, std::vector<int>(width));

    for (auto& row : matrix)
        for (int &number : row)
            number = (T)distrib(gen) - 100;

    return matrix;
}

template <typename T>
void print_matrix(const Matrix<T>& matrix)
{
    for (auto& row : matrix)
    {
        for (int number : row)
            std::cout << std::setw(4) << number << ' ';
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void test(int height, int width)
{
    auto matrix = get_random_matrix<int>(height, width);

    auto result = MaxSumSubmatrix<int>(matrix).compute();
    auto correct = SlowerMaxSumSubmatrix<int>(matrix).compute();

//    std::cout << "Matrix: " << std::endl;
//    print_matrix(matrix);

    std::cout << "Max sum = " << result.sum << ", from ";
    std::cout << (std::string)result.range.from << " to ";
    std::cout << (std::string)result.range.to << std::endl;

    if (result.sum != correct.sum) {
        std::cout << "Fail..." << std::endl;
    }
}

int main()
{
    for (int i = 1; i <= 50; i++)
        for (int j = 1; j <= 50; j++)
            test(i, j);
}