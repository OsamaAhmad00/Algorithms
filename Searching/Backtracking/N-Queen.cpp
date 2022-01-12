#include <iostream>
#include <vector>

class NQueen
{

public:

    struct Coord
    {
        int x;
        int y;
    };

private:

    int n;
    std::vector<bool> rows; // ranks
    std::vector<bool> cols; // files

    // Assuming that the board[0][0] is the
    //  top left corner of the board, for the
    //  positive slope diagonal, x + y is
    //  constant, and for the diagonal with
    //  negative slope, x - y is constant.
    // The diagonal constants will be in
    //  the range [-(n-1), (n-1)].
    std::vector<bool> positive_diagonal;
    std::vector<bool> negative_diagonal;

    std::vector<Coord> board;
    std::vector<std::vector<Coord>> result;

    void set_row(int row, bool value) { rows[row] = value; }
    void set_col(int col, bool value) { cols[col] = value; }

    void set_positive_diagonal(const Coord& coord, bool value)
    {
        int constant = coord.x + coord.y;
        // Shifting by n to deal with negative values.
        positive_diagonal[constant + n] = value;
    }

    void set_negative_diagonal(const Coord& coord, bool value)
    {
        int constant = coord.x - coord.y;
        // Shifting by n to deal with negative values.
        negative_diagonal[constant + n] = value;
    }

    void set_square(const Coord& coord, bool value)
    {
        set_row(coord.x, value);
        set_col(coord.y, value);
        set_positive_diagonal(coord, value);
        set_negative_diagonal(coord, value);
    }

    bool is_threatened(const Coord& coord) const
    {
        int x = coord.x;
        int y = coord.y;
        return positive_diagonal[x + y + n] ||
               negative_diagonal[x - y + n] ||
               rows[x] || cols[y];
    }

    void compute(int x = 0)
    {
        if (x == n) {
            result.push_back(board);
            return;
        }

        for (int y = 0; y < n; y++)
        {
            Coord coord = {x, y};

            if (is_threatened(coord))
                continue;

            set_square(coord, true);
            board.push_back(coord);

            compute(x + 1);

            board.pop_back();
            set_square(coord, false);
        }
    }

public:

    NQueen(int n) : n(n), rows(n, false), cols(n, false),
        positive_diagonal(2 * n, false), negative_diagonal(2 * n, false)
    {}

    std::vector<std::vector<Coord>> solve()
    {
        compute();
        return std::move(result);
    }
};

void test(int n)
{
    std::vector<std::vector<bool>> board(n, std::vector<bool>(n));
    auto results = NQueen{n}.solve();

    std::cout << "There are " << results.size() << " possible arrangements." << std::endl;
    std::cout << "Arrangements: " << std::endl << std::endl;

    for (auto& result : results)
    {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                board[i][j] = false;

        for (auto& coord : result)
            board[coord.x][coord.y] = true;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++)
                std::cout << (board[i][j] ? '*' : '.');
            std::cout << std::endl;
        }

        std::cout << std::endl;
    }
}

int main()
{
    test(8);
}