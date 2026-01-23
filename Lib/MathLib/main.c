#include "MathLib.h"

SUNY_API struct Sframe* Smain(struct Sframe* frame, struct Stable *table) {
    Sinitialize_c_api_func(frame, table, 20, 1, "complex", Scomplex);
    Sinitialize_c_api_func(frame, table, 21, 1, "sqrt", Ssqrt);
    Sinitialize_c_api_func(frame, table, 22, 1, "abs", Sabs);
    Sinitialize_c_api_func(frame, table, 23, 1, "log", Slog);
    Sinitialize_c_api_func(frame, table, 24, 1, "log10", Slog10);
    Sinitialize_c_api_func(frame, table, 25, 1, "ln", Sln);
    Sinitialize_c_api_func(frame, table, 26, 1, "sin", Ssin);
    Sinitialize_c_api_func(frame, table, 27, 1, "cos", Scos);
    Sinitialize_c_api_func(frame, table, 28, 1, "tan", Stan);
    Sinitialize_c_api_func(frame, table, 29, 1, "cot", Scot);
    return frame;
}
