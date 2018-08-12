#include "pathtrace/shape.h"

namespace pathtracer {
    Material::Material(Reflection reflection, const Color& emission, const Color& color) :
        reflection{reflection}, emission{emission}, color{color}
    {}
}  // namespace pathtracer
