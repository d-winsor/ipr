#include "doctest/doctest.h"

#include <ipr/impl>

TEST_CASE("C++ Standard Conversions") {
  using namespace ipr;
  impl::Lexicon lexicon{};
  impl::Module module{lexicon};
  impl::Interface_unit unit{lexicon, module};

  // static_cast<long long>(4);
  //    Integral Promotion            (Promote)
  lexicon.make_promote(
    *lexicon.make_literal(lexicon.int_type(), "4"),
    &lexicon.long_long_type()
  );

  // (float)2.2;
  //    Floating-Point Conversion     (Demote)
  lexicon.make_demote(
    *lexicon.make_literal(lexicon.double_type(), "2.2"),
    &lexicon.float_type()
  );

  // (volatile int)7;
  //    CV-Qualifcation               (Qualification)
  lexicon.make_qualification(
    lexicon.get_qualified(Type_qualifier::Volatile, lexicon.int_type()),
    *lexicon.make_literal(lexicon.double_type(), "2.2"),
    &lexicon.int_type() // Type of the expr can differ from target type (see 7.2.2/2)
  );

  // int* const ptr = 0;
  //    Pointer Conversion            (Coersion)
  const auto& ptr_type = lexicon.get_qualified(
    Type_qualifier::Const, lexicon.get_pointer(lexicon.int_type()));
  auto& ptr = *unit.global_region()->declare_var(*lexicon.make_identifier("ptr"), ptr_type);
  ptr.init = lexicon.make_coerce(
    ptr_type,
    *lexicon.make_literal(lexicon.int_type(), "0"),
    &ptr_type
  );

  // if (ptr) double(6);
  //    Boolean Conversion            (Coersion)
  //    Lvalue-to-Rvalue              (Read)
  //    Integral-Floating Conversion  (Coersion)
  const auto& condition = *lexicon.make_coerce(
    lexicon.bool_type(),
    *lexicon.make_read(
      *lexicon.make_id_expr(ptr),
      &lexicon.get_pointer(lexicon.int_type()) // != ptr.type() (see 7.2.2/2)
    ),
    &lexicon.bool_type()
  );
  const auto& then_expr = *lexicon.make_coerce(
      lexicon.double_type(),
      *lexicon.make_literal(lexicon.int_type(), "4"),
      &lexicon.double_type()
  );
  lexicon.make_if_then(condition, *lexicon.make_expr_stmt(then_expr));
}
