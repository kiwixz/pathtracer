#include "app/app.h"

int main(int argc, char** argv)
{
    return app::App{}.main(argc, const_cast<const char**>(argv));
}
