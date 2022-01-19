#include <iostream>
#include <vector>
#include <cmath>
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
T euclidean_distance(const Point<T>& p1, const Point<T>& p2)
{
    T x = p2.x - p1.x;
    T y = p2.y - p1.y;
    return std::sqrt(x * x + y * y);
}

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
bool is_further(const Point<T>& p1, const Point<T>& p2, const Point<T>& common)
{
    return euclidean_distance(p1, common) > euclidean_distance(p2, common);
}

template <typename T>
std::vector<Point<T>> convex_hull(const std::vector<Point<T>>& points)
{
    // https://www.youtube.com/watch?v=B2AJoQSZf4M
    // https://www.youtube.com/watch?v=nBvCZi34F_o

    // This takes O(n * h) where n is the number of points,
    //  and h is the number of the vertices of the convex hull.
    //  If all points are vertices, this will be O(n^2) which
    //  is quite slow.
    // The algorithm here is simple.
    // First, we need to get one point that is a vertex to
    //  start with, but, how are we going to find it?
    // We can add left most (smallest x) point to the vertices.
    //  If there is a tie, we choose the lowest one (smallest y).
    //  It's clear that such a point will be a vertex.
    // For each point we add to the vertices, we iterate over all
    //  points, and add the point with the smallest counterclockwise
    //  angle in reference to the lastly added vertex.
    // If we reach the first vertex, then we're done.
    // Note that it doesn't matter whether we search for the point with the
    //  smallest clockwise or counterclockwise angle, this will just change
    //  the direction of the search.

    std::vector<Point<T>> vertices;
    vertices.push_back(*std::min_element(points.begin(), points.end()));

    while (true)
    {
        // TODO this can be optimized by not considering
        //  points that are already considered to be vertices.

        Point<T> candidate = points.front();
        Point<T>& last_vertex = vertices.back();

        // Starting from the second point since
        //  the first is the current candidate.
        for (int i = 1; i < points.size(); i++)
        {
            // We can check if two vectors form a clockwise or a counterclockwise
            //  angle using the cross product.
            // Instead of calculating the angle, and minimizing (which is more
            //  complicated), we can just pick any point that forms a clockwise
            //  angle with the current candidate point, then pick any point that
            //  forms a clockwise angle with it... and so on. This will result in
            //  the point with the maximum clockwise angle from the current node,
            //  in other words, the minimum counterclockwise angle.
            // In the case the points were collinear, choose the one that is
            //  the furthest away from the last vertex.
            T cp = cross_product(candidate, points[i], last_vertex);
            if (cp < 0 || (cp == 0 && is_further(points[i], candidate, last_vertex)))
                candidate = points[i];
        }

        if (candidate == vertices.front())
            break;

        vertices.push_back(candidate);
    }

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