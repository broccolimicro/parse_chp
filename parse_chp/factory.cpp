#include "factory.h"

#include "composition.h"

namespace parse_chp {

parse::syntax *produce(tokenizer &tokens, void *data) {
	return new composition(tokens, 0, data);
}

void expect(tokenizer &tokens) {
	tokens.expect<composition>();
}

void register_syntax(tokenizer &tokens) {
	composition::register_syntax(tokens);
}

}

