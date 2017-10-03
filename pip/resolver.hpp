#pragma once

#include <pip/syntax.hpp>

#include <cc/diagnostics.hpp>

#include <unordered_map>

namespace pip
{
  /// The name resolver is responsible for performing name lookup after
  /// an initial translation. This class is essentially responsible for
  /// attaching meaning tor reference and field expressions.
  class resolver
  {
  public:
    resolver(context& cxt)
      : cxt(cxt)
    { }

    void operator()(decl* d) { resolve_decl(d); }

  private:
    void resolve_decl(decl* d);
    void resolve_decl(program_decl* p);
    void resolve_decl(table_decl* t);
    
    void resolve_actions(action_seq& as);
    void resolve_action(action* a);
    void resolve_action(advance_action* a);
    void resolve_action(copy_action* a);
    void resolve_action(set_action* a);
    void resolve_action(write_action* a);
    void resolve_action(clear_action* a);
    void resolve_action(drop_action* a);
    void resolve_action(match_action* a);
    void resolve_action(goto_action* a);
    void resolve_action(output_action* a);

    void resolve_expr(expr* e);
    void resolve_expr(int_expr* e);
    void resolve_expr(wild_expr* e);
    void resolve_expr(range_expr* e);
    void resolve_expr(ref_expr* e);
    void resolve_expr(field_expr* e);

  private:
    context& cxt;

    /// Allow lookup of declared dataplane entities.
    std::unordered_map<symbol*, decl*> decls;
  };

// -------------------------------------------------------------------------- //
// Exceptions

  /// Represents a name lookup error.
  class lookup_error : public cc::diagnosable_error
  {
  public:
    lookup_error(cc::location loc, const std::string& msg)
      : cc::diagnosable_error(cc::dk_error, "lookup", loc, msg)
    { }
  };

} // namespace pip
