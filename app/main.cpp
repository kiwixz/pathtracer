#include "app/app.h"

int main(int argc, char** argv)
{
    return pathtracer::App{}.main(argc, const_cast<const char**>(argv));
}
