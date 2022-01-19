#include <iostream>
#include <vector>
#include <algorithm>

template <typename T>
struct Point
{
    T x;
    T y;

    bool operator<(const Point<T>& p) const {
        return x < p.x || (x == p.x && y < p.y);
    }

    bool operator==(const Point<T>& p) const {
        return x == p.x && y == p.y;
    }

    operator std::string() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

template <typename T>
T cross_product(Point<T> p1, Point<T> p2, const Point<T>& common = {0, 0})
{
    // let v1 = p1 - common, v2 = p2 - common.
    // This function returns the signed area of the parallelogram
    //  with the vectors v1 and v2 as sides.
    // The returned value is positive if v1 is to the right of v2, forming a counterclockwise angle.
    // The returned value is negative if v1 is to the left of v2, forming a clockwise angle.
    // The returned value is 0 if v1 and v2 are collinear.
    p1.x -= common.x; p1.y -= common.y;
    p2.x -= common.x; p2.y -= common.y;
    return p1.x * p2.y - p1.y * p2.x;
}

template <typename T>
struct AngleComparator
{
    const Point<T> reference;

    bool operator()(const Point<T>& p1, const Point<T>& p2) const
    {
        // You can calculate the angle using the
        // atan2 function, but this should faster.

        // The reference point should be
        // at the beginning of the array.
        if (p1 == reference) return true;
        if (p2 == reference) return false;

        T cp = cross_product(p1, p2, reference);

        // If p1 and p2 are collinear
        if (cp == 0) return p1 < p2;

        return cp > 0;
    }
};

template <typename T>
bool causes_clockwise_angle(const Point<T>& new_point, const std::vector<Point<T>>& vertices)
{
    auto& p1 = *(vertices.end() - 1);
    auto& p2 = *(vertices.end() - 2);
    T cp = cross_product(p1, new_point, p2);
    return cp <= 0;
}

template <typename T>
std::vector<Point<T>> convex_hull(std::vector<Point<T>> points)
{
    // https://www.youtube.com/watch?v=B2AJoQSZf4M
    // https://www.youtube.com/watch?v=SBdWdT_5isI
    // This algorithm takes O(n * log(n)).
    // First, we need to get one point that is a vertex to
    //  start with, but, how are we going to find it?
    // We can add left most (smallest x) point to the vertices.
    //  If there is a tie, we choose the lowest one (smallest y).
    //  It's clear that such a point will be a vertex.
    // This chosen point will be a "reference" point.
    // Next, we'll sort all points by the polar angle they make
    //  with the reference point.
    // Push the first 2 points (first one will be the reference
    //  point) in a stack, then for each point, starting from the
    //  third point, while the current point forms a clockwise
    //  angle if added to the vertices, pop from the stack.
    // After popping out all points that makes a clockwise angle
    //  push the current point, then repeat for the remaining points.
    // After you're done, the points in the stack will be the vertices
    //  of the convex hull.

    // This will be used as a stack.
    std::vector<Point<T>> vertices;

    Point<T> reference = *std::min_element(points.begin(), points.end());
    std::sort(points.begin(), points.end(), AngleComparator<T>{reference});

    vertices.push_back(points[0]);
    vertices.push_back(points[1]);

    for (int i = 2; i < points.size(); i++)
    {
        while (vertices.size() >= 2 && causes_clockwise_angle(points[i], vertices))
            vertices.pop_back();

        vertices.push_back(points[i]);
    }

    // FIXME is this necessary?
    // The last point may be collinear. In such case, pop it out.
    if (causes_clockwise_angle(reference, vertices))
        vertices.pop_back();

    return vertices;
}

void test(const std::vector<Point<int>>& points)
{
    std::cout << "Points: ";
    for (auto& point : points)
        std::cout << (std::string)point << ' ';
    std::cout << std::endl;

    auto result = convex_hull(points);

    std::cout << "Convex Hull Vertices: ";
    for (auto& point : result)
        std::cout << (std::string)point << ' ';
    std::cout << std::endl << std::endl;
}

std::vector<Point<int>> get_sample_points_1()
{
    // (0, 0), (0, 3), (3, 0), (3, 3)
    return {{0, 3}, {2, 2}, {1, 1}, {2, 1},
            {3, 0}, {0, 0}, {3, 3}};
}

std::vector<Point<int>> get_sample_points_2()
{
    // (0, 0), (2, 0), (2, 2), (0, 2)
    return {{0, 0}, {1, 0}, {2, 0}, {0, 1},
            {0, 2}, {0, 2}, {1, 2}, {2, 2}};
}

int main()
{
    test(get_sample_points_1());
    test(get_sample_points_2());
}