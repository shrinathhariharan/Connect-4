#include <iostream>
#include <array>
#include <string>
#include <string_view>
#include <limits>
#include <utility>
#include <optional>

namespace Color
{
    constexpr std::string_view RED{"\033[31m"};
    constexpr std::string_view YELLOW{"\033[93m"};
    constexpr std::string_view GRAY{"\033[90m"};
    constexpr std::string_view WHITE{"\033[0m"};
}

namespace Settings
{
    constexpr int boardLength{7};
    constexpr int boardHeight{6};
    
    constexpr char character{'O'};

    constexpr int consoleNewline{23};
}

struct Point
{
    int x{};
    int y{};
};

class Piece
{
public:
    enum Player
    {
        p1,
        p2,
        empty,
    };
private:
    Player m_player{empty};

public:
    Piece(Player p) : m_player{p}
    {}

    Piece(){}

    std::string_view playerColor() const 
    {
        switch (m_player)
        {
        case p1: return Color::RED;
        case p2: return Color::YELLOW;
        default: return Color::WHITE;
        }
    }
   
    Player getPlayer() const {return m_player;}
};

bool operator==(Piece a, Piece::Player b)
{
    return a.getPlayer() == b;
}

bool operator==(Piece a, Piece b)
{
    return a.getPlayer() == b.getPlayer();
}

bool operator==(Piece::Player a, Piece b)
{
    return b.getPlayer() == a;
}

bool operator!=(Piece a, Piece::Player b)
{
    return !(a == b);
}

bool operator!=(Piece::Player a, Piece b)
{
    return !(a == b);
}

bool operator!=(Piece a, Piece b)
{
    return !(a == b);
}

std::ostream& operator<<(std::ostream& out, const Piece& piece)
{
    out << piece.playerColor() << Settings::character << Color::WHITE;
    return out;
}

class Board
{
    std::array<std::array<Piece, Settings::boardLength>, Settings::boardHeight> m_board{};
    int m_move{1};
public:
    Board()
    {
        for (auto& row : m_board)
            for (auto& item : row)
                item = Piece{Piece::empty}; 
    }

    Piece operator()(int x, int y) const {return m_board[y][x];}
    Piece& operator()(int x, int y) {return m_board[y][x];}

    int getMove() const {return m_move;}
    void nextTurn() {++m_move;}

    bool fullBoard() const
    {
        for (const auto& row : m_board)
            for (auto piece : row)
                if (piece == Piece::empty)
                    return false;
        return true;
    }

};

std::ostream& operator<<(std::ostream& out, const Board& board)
{
    for (int i{0}; i < Settings::consoleNewline; ++i)
        out << '\n';

    out << "  ";
    for (int i{0}; i < Settings::boardLength; ++i)
        out << Color::GRAY << i << Color::WHITE << ' ';
    out << '\n';

    for (int i{0}; i < Settings::boardHeight; ++i)
    {
        out << Color::GRAY << i << Color::WHITE << ' ';
        for (int j{0}; j < Settings::boardLength; ++j)
            out << board(j, i) << ' ';
        out << '\n';
    }

    return out;
}

enum Stats
{
    player1,
    player2,
    draw,
};

template <typename T>
T getInput(std::string_view statement)
{
    std::cout << statement << '\n';

    T input{};
    while (!(std::cin >> input))
    {
        std::cin.clear();
        std::cout << "Invalid Input!\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return input;
}

std::optional<Point> addPiece(Board& board)
{
    while (true)
    {
        int columnChoice{};
        while (true)
        {
            columnChoice = getInput<int>("Choose a column to place your piece in:");
            
            if (columnChoice >= 0 && columnChoice < Settings::boardLength)
                break;
            std::cout << "Invalid Column!\n";
        }

        bool validPlacement{false};
        for (int i{Settings::boardHeight - 1}; i >= 0; --i)
        {
            Piece& playerSpot{board(columnChoice, i)};
            if (playerSpot == Piece::empty)
            {
                playerSpot = static_cast<Piece::Player>((board.getMove() + 1) % 2);
                board.nextTurn();

                validPlacement = true;
                return Point{columnChoice, i};
            }
        }

        if (board.fullBoard())
            return std::nullopt;

        if (!validPlacement)
        {
            std::cout << "That row is already full!\n";
            continue;
        }
    }
}

std::pair<bool, Piece::Player> checkWin(const Board& board, Point placedPiece)
{
    Piece placedSpot{board(placedPiece.x, placedPiece.y)};


    int horizontalCount{1};
    
    for (int i{1}; i < 4; ++i) //right
    {
        int nextSpot{placedPiece.x + i};
        if (nextSpot >= Settings::boardLength || board(nextSpot, placedPiece.y) != placedSpot)
            break;
        ++horizontalCount;
    }

    for (int i{1}; i < 4; ++i) //left
    {
        int nextSpot{placedPiece.x - i};
        if (nextSpot < 0 || board(nextSpot, placedPiece.y) != placedSpot)
            break;
        ++horizontalCount;
    }

    if (horizontalCount >= 4)
        return {true, placedSpot.getPlayer()};


    int verticalCount{1};
    
    for (int i{1}; i < 4; ++i) //up
    {
        int nextSpot{placedPiece.y - i};
        if (nextSpot < 0 || board(placedPiece.x, nextSpot) != placedSpot)
            break;
        ++verticalCount;
    }

    for (int i{1}; i < 4; ++i) //down
    {
        int nextSpot{placedPiece.y + i};
        if (nextSpot >= Settings::boardHeight || board(placedPiece.x, nextSpot) != placedSpot)
            break;
        ++verticalCount;
    }

    if (verticalCount >= 4)
        return {true, placedSpot.getPlayer()};

    int diagonalCountRight{1};

    for (int i{1}; i < 4; ++i) //right up
    {
        Point nextSpot{placedPiece.x + i, placedPiece.y + i};
        if (nextSpot.x >= Settings::boardLength || nextSpot.y >= Settings::boardHeight || board(nextSpot.x, nextSpot.y) != placedSpot)
            break;
        ++diagonalCountRight;
    }

    for (int i{1}; i < 4; ++i) //right down
    {
        Point nextSpot{placedPiece.x - i, placedPiece.y - i};
        if (nextSpot.x < 0 || nextSpot.y < 0 || board(nextSpot.x, nextSpot.y) != placedSpot)
            break;
        ++diagonalCountRight;
    }

    if (diagonalCountRight >= 4)
        return {true, placedSpot.getPlayer()};


    int diagonalCountLeft{1};

    for (int i{1}; i < 4; ++i) //left up
    {
        Point nextSpot{placedPiece.x - i, placedPiece.y + i};
        if (nextSpot.x < 0 || nextSpot.y >= Settings::boardHeight || board(nextSpot.x, nextSpot.y) != placedSpot)
            break;
        ++diagonalCountLeft;
    }

    for (int i{1}; i < 4; ++i) //left down
    {
        Point nextSpot{placedPiece.x + i, placedPiece.y - i};
        if (nextSpot.x >= Settings::boardLength || nextSpot.y < 0 || board(nextSpot.x, nextSpot.y) != placedSpot)
            break;
        ++diagonalCountLeft;
    }

    if (diagonalCountLeft >= 4)
        return {true, placedSpot.getPlayer()};

    return {false, Piece::empty};
}

void printStats(const std::array<int, 3>& stats)
{
    std::cout << "Player 1 has won " << stats[player1] << ' ' << (stats[player1] == 1 ? "game" : "games") << '\n';
    std::cout << "Player 2 has won " << stats[player2] << ' ' << (stats[player2] == 1 ? "game" : "games") << '\n';
    std::cout << stats[draw] << ' ' << (stats[draw] == 1 ? "game" : "games") << " have resulted in a draw!\n";
}

int main()
{
    std::cout << "This is a game of " << Color::YELLOW << "CONNECT" << Color::RED << '4' 
              << Color::WHITE << "! The goal of the game is to line up four pieces vertically, horizontially, or diagionally consecutively!\n";

    bool keepPlaying{true};
    std::array<int, 3> playerStats{0, 0, 0};

    do
    {
        std::cout << Piece{Piece::p1} << " is player 1, and " << Piece{Piece::p2} << " is player 2.\n";
        std::cout << "Press ENTER to start!\n";
        std::cin.get();

        std::pair<bool, Piece::Player> playerWon{false, Piece::empty};

        Board board{};

        while (!playerWon.first)
        {
            std::cout << board;
            std::cout << "Turn: " << board.getMove() << "\t\t" << (board.getMove() % 2 ? "Player One's Turn" : "Player Two's Turn") << '\n';

            std::optional<Point> addedPiece{addPiece(board)};
        
            if (!addedPiece)
            {
                std::cout << "The the board is full!\n";
                break;
            }

            playerWon = checkWin(board, *addedPiece);
        }

        std::cout << board;

        if (playerWon.first)
        {
            std::cout << "Congratualations, " << (playerWon.second == Piece::p1 ? "Player One" : "Player Two") << " won!\n";
            playerStats[playerWon.second] += 1;
        }
        else
        {
            std::cout << "The game was a draw.\n";
            playerStats[draw] += 1;
        }

        bool quitLoop{false};
        while (!quitLoop)
        {
            const char playInput{getInput<char>("Do you want to play again (y / n), or see player statistics (s)?")};
            switch (playInput)
            {
            case 'n':
            case 'N': 
                keepPlaying = false;
                quitLoop = true;
                break;
            case 'y':
            case 'Y':
                quitLoop = true;
                break;
            case 's':
            case 'S':
                printStats(playerStats);
                break;
            default: std::cout << "Invalid Input!\n";
            }
        }
    } while (keepPlaying);

    std::cout << "Thank you for playing " << Color::YELLOW << "CONNECT" << Color::RED << '4' << Color::WHITE << '\n';

    return 0;
}

