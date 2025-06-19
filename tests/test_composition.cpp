#include <common/standard.h>
#include <gtest/gtest.h>

#include <parse/tokenizer.h>
#include <parse/default/block_comment.h>
#include <parse/default/line_comment.h>
#include <parse_chp/factory.h>
#include <parse_chp/composition.h>

using namespace std;

// Helper function to parse a string into an HSE graph
parse_chp::composition load_chp_string(string input) {
	// Set up tokenizer
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	parse_chp::register_syntax(tokens);

	tokens.insert("string_input", input, nullptr);

	// Parse input
	tokens.increment(true);
	tokens.expect<parse_chp::composition>();
	if (tokens.decrement(__FILE__, __LINE__)) {
		return parse_chp::composition(tokens);
	}

	return parse_chp::composition();
}

TEST(Composition, Sequence) {
	EXPECT_EQ(load_chp_string("a+; b+; c-; d-").to_string(), "a+;b+;c-;d-");
}

TEST(Composition, Parallel) {
	EXPECT_EQ(load_chp_string("(a+, b+); (a-, b-)").to_string(), "(a+,b+);(a-,b-)");
}

TEST(Composition, Selection) {
	EXPECT_EQ(load_chp_string("[c -> a+; a- [] ~c -> b+; b-]").to_string(), "[c->a+;a-[]~c->b+;b-]");
}

TEST(Composition, Loop) {
	EXPECT_EQ(load_chp_string("*[a+; b+; a-; b-]").to_string(), "*[a+;b+;a-;b-]");
}

TEST(Composition, ComplexComposition) {
	EXPECT_EQ(load_chp_string("(a+; b+) || (c+; d+)").to_string(), "(a+;b+)||(c+;d+)");
}

TEST(Composition, NestedControls) {
	EXPECT_EQ(load_chp_string("*[[a -> b+; b- [] ~a -> c+; (d+, e+); c-; (d-, e-)]]").to_string(), "*[[a->b+;b-[]~a->c+;(d+,e+);c-;(d-,e-)]]");
}

TEST(Composition, Counter) {
	auto dut = load_chp_string(R"(
R.i-,R.d-,R0-,R1-,Rz-,L.z-,L.n-,v0-,v1-,vz+; [R.z&~R.n&~L.i&~L.d];
*[[  v1 & (R.z | R.n) & L.i -> R.i+
  [] (v0 | vz) & (R.z | R.n) & L.d -> R.d+
  [] (v0 | vz) & L.i -> R1+
  [] v1 & R.n & L.d -> R0+
  [] v1 & R.z & L.d -> Rz+
  ]; L.z-, L.n-;
  [~L.i & ~L.d];
  [  Rz -> vz+; v0-,v1-
  [] R.i | R0 -> v0+; v1-,vz-
  [] R.d | R1 -> v1+; v0-,vz-
  ];
	(
	  [~v0 & ~v1]; Rz- ||
	  [~v1 & ~vz]; R0- ||
		[~v1 & ~vz & ~R.z & ~R.n]; R.i- ||
		[~v0 & ~vz]; R1- ||
		[~v0 & ~vz & ~R.z & ~R.n]; R.d-
  ); @
  [  ~v0 & ~v1 -> L.z+
  [] ~vz -> L.n+
  ]
 ] ||

(L.i-,L.d-;[~L.z&~L.n]; *[[L.z | L.n]; [1->L.i+:1->L.d+]; [~L.z&~L.n]; L.i-,L.d-] ||
R.z+,R.n-;[~R.i&~R.d]; *[[R.i | R.d]; R.z-,R.n-; [~R.i&~R.d]; [1->R.z+:1->R.n+]])'1
)");

	EXPECT_EQ(dut.to_string(),"R.i-,R.d-,R0-,R1-,Rz-,L.z-,L.n-,v0-,v1-,vz+;[R.z&~R.n&~L.i&~L.d];*[[v1&(R.z|R.n)&L.i->R.i+[](v0|vz)&(R.z|R.n)&L.d->R.d+[](v0|vz)&L.i->R1+[]v1&R.n&L.d->R0+[]v1&R.z&L.d->Rz+];L.z-,L.n-;[~L.i&~L.d];[Rz->vz+;v0-,v1-[]R.i|R0->v0+;v1-,vz-[]R.d|R1->v1+;v0-,vz-];([~v0&~v1];Rz-||[~v1&~vz];R0-||[~v1&~vz&~R.z&~R.n];R.i-||[~v0&~vz];R1-||[~v0&~vz&~R.z&~R.n];R.d-);@[~v0&~v1->L.z+[]~vz->L.n+]]||(L.i-,L.d-;[~L.z&~L.n];*[[L.z|L.n];[1->L.i+:1->L.d+];[~L.z&~L.n];L.i-,L.d-]||R.z+,R.n-;[~R.i&~R.d];*[[R.i|R.d];R.z-,R.n-;[~R.i&~R.d];[1->R.z+:1->R.n+]])'1");
}

TEST(Composition, Buffer) {
	auto dut = load_chp_string(R"(
*[x = L?; R!x]
)");

	EXPECT_EQ(dut.to_string(),"*[x=L?;R!x]");
}

TEST(Composition, Split) {
	auto dut = load_chp_string(R"(
*[c = C?;
  [ c == 0 -> A!L?
  [] c == 1 -> B!L?
	]
 ]
)");

	EXPECT_EQ(dut.to_string(),"*[c=C?;[c==0->A!L?[]c==1->B!L?]]");
}

TEST(Composition, Split2) {
	auto dut = load_chp_string(R"(
*[D[C?]!L?]
)");

	EXPECT_EQ(dut.to_string(),"*[D[C?]!L?]");
}
