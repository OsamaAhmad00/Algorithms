#include <iostream>
#include <vector>

template <typename T>
class PrefixSum2D
{
    std::vector<std::vector<T>> sums;

public:

    struct Point
    {
        int x;
        int y;
    };

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

void test(const std::vector<std::vector<int>>& v)
{
    PrefixSum2D<int> sums(v);
    for (int x1 = 0; x1 < v.size(); x1++) {
        for (int y1 = 0; y1 < v[0].size(); y1++) {
            for (int x2 = x1; x2 < v.size(); x2++) {
                for (int y2 = y1; y2 < v[0].size(); y2++)
                {
                    int sum = 0;
                    for (int x = x1; x <= x2; x++)
                        for (int y = y1; y <= y2; y++)
                            sum += v[x][y];

                    int result = sums.query({x1, y1}, {x2, y2});
                    if (sum != result)
                        std::cout << "Fail..." << std::endl;
                }
            }
        }
    }
}

int main()
{
    test({{32, 23, 1, 56, 12, 6, 1, 5, 55, 90, 23, 11},
          {32, 23, 1, 56, 12, 6, 1, 5, 55, 10, 71, 51},
          {32, 23, 1, 56, 12, 6, 1, 2, 53, 90, 23, 31},
          {32, 21, 1, 56, 12, 6, 7, 5, 52, 90, 22, 21},
          {33, 22, 1, 56, 11, 8, 1, 5, 15, 90, 23, 11},
          {32, 25, 1, 56, 12, 6, 1, 5, 45, 90, 23, 11}});
}