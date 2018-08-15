#include "pathtrace/shape.h"

namespace pathtrace {
    Material::Material(Reflection reflection, const Color& emission, const Color& color) :
        reflection{reflection}, emission{emission}, color{color}
    {}
}  // namespace pathtrace
