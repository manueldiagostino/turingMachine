#ifndef COLORS_HH
#define COLORS_HH
#include <ostream>
#include <string>

namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_YELLOW   = 33,
        FG_BLUE     = 34,
        FG_MAGENTA	= 35,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_YELLOW   = 43,
        BG_BLUE     = 44,
        BG_MAGENTA	= 45,
        BG_DEFAULT  = 49
    };

    inline
    void
    to_lower(std::string& color) {
        for (auto it = color.begin(); it != color.end(); it++)
            if (*it >= 'A' && *it <= 'Z')
                *it += 32; // to lower conversion
    }

    inline
    Code
    BG_Converter(std::string& color) {    
        to_lower(color);
            
        if (color == "red")
            return BG_RED;
        else if (color == "green")
            return BG_GREEN;
        else if (color == "yellow")
            return BG_YELLOW;
        else if (color == "blue")
            return BG_BLUE;
        else if (color == "green")
            return BG_GREEN;
        else if (color == "magenta")
            return BG_MAGENTA;
        else
            return BG_DEFAULT;
    }

    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };

    const Modifier MD_FG_DEFAULT(FG_DEFAULT);
    const Modifier MD_FG_GREEN(FG_GREEN);
    const Modifier MD_FG_RED(FG_RED);
    const Modifier MD_FG_YELLOW(FG_YELLOW);
}

#endif // ! COLORS_HH