#include "factory.h"

#include "composition.h"

namespace parse_chp {

const parse::factory factory(parse::schema::from<composition>());

}

