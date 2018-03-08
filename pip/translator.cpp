#include "translator.hpp"
#include "decl.hpp"
#include "type.hpp"
#include "context.hpp"
#include "expr.hpp"
#include "action.hpp"
#include "dumper.hpp"

#include <string>

// for testing only
#include <iostream>
#include <sstream>

namespace pip
{
  class type_error : public cc::diagnosable_error
  {
  public:
    type_error(cc::location loc, const std::string& msg)
      : cc::diagnosable_error(cc::dk_error, "type", loc, msg)
    { }
  };

  class syntax_error : public cc::diagnosable_error
  {
  public:
    syntax_error(cc::location loc, const std::string& msg)
      : cc::diagnosable_error(cc::dk_error, "syntax", loc, msg)
    { }
  };
  
  translator::translator(context& cxt)
    : cxt(cxt), field_decoder(cxt)
  {
  }
  
  /// program ::= (pip <decl-seq>)
  decl*
  translator::trans_program(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      decl_seq decls;
      match_list(list, "pip", &decls);
      
      return new program_decl(std::move(decls));
    }
    sexpr::throw_unexpected_term(e);
  }
  
  /// decl-seq ::= (<decl*>)
  decl_seq
  translator::trans_decls(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      decl_seq decls;

      decl* d = trans_decl(list);
      decls.push_back(d);
      
      return decls;
    }
    sexpr::throw_unexpected_term(e);
  }
  
  /// decl ::= table-decl | meter-decl
  decl*
  translator::trans_decl(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      symbol* sym;
      match_list(list, &sym);
      if (*sym == "table")
	return trans_table(list);
      sexpr::throw_unexpected_id(cast<sexpr::id_expr>(list->exprs[0]));
    }
    sexpr::throw_unexpected_term(e);
  }
  
  /// table-decl ::= (table id <match-kind> <action-seq> <rule-seq>)
  ///
  /// rule-kind ::= exact | prefix | wildcard | range
  decl*
  translator::trans_table(const sexpr::list_expr* e)
  {
    symbol* id;
    symbol* kind;
    action_seq actions;
    rule_seq rules;
    match_list(e, "table", &id, &kind, &actions, &rules);
    
    auto it = match_kinds.find(kind);
    if(it == match_kinds.end()) {
      std::stringstream ss;
      ss << "Invalid table rule: " << *(kind);
      throw syntax_error(cc::get_location(e), ss.str());
    }

    match_kind = it->second;
    
    return new table_decl(id, match_kind, std::move(actions), std::move(rules));
  }
  
  /// rule_seq ::= (<rule*>)
  rule_seq
  translator::trans_rules(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      match_list(list, "rules");
      rule_seq rules;
      
      // Match each element in turn.
      for(const sexpr::expr* el : list->exprs) {
        // We are only interested in the entire sublists contained within list,
        // not its individual expressions.
	if(const sexpr::list_expr* r = as<sexpr::list_expr>(el)) {					
	  rule* r1 = trans_rule(r);
	  rules.push_back(r1);
	}
      }
      return rules;
    }
    sexpr::throw_unexpected_term(e);    
  }
  
  rule*
  translator::trans_rule(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      expr_seq exprs;
      expr* key;
      action_seq actions;
      
      match_list(list, &key, &actions);
      
      auto r = new rule(match_kind, key, std::move(actions));
      return r;
    }
    
    sexpr::throw_unexpected_term(e);
  }
  
  rule*
  translator::trans_rule(const sexpr::list_expr* e)
  {
    expr_seq exprs;
    expr* key;
    action_seq actions;
    
    match_list(e, "rule", &key, &actions);
    
    auto r = new rule(match_kind, key, std::move(actions));

    return r;
  }
  
  
  action_seq
  translator::trans_actions(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      match_list(list, "actions");
      action_seq actions;
      for(const sexpr::expr* el : list->exprs) {
	// We are only interested in the entire sublists contained within list,
	// not its individual expressions.
	if(const sexpr::list_expr* a = as<sexpr::list_expr>(el)) {
	  action* a1 = trans_action(a);
	  actions.push_back(a1);
	}
      }
      
      return actions;
    }
    sexpr::throw_unexpected_term(e);
  }
  
  action*
  translator::trans_action(const sexpr::list_expr* e)
  {
    symbol* action_name;
    expr_seq exprs;
    int i;
    
    match_list(e, &action_name);
    
    if(*action_name == "advance") {
      expr* amount;
      match_list(e, "advance", &amount);
      return cxt.make_advance_action(amount);
    }
    
    if(*action_name == "copy") {
      expr* src;
      expr* dst;
      expr* n;
      match_list(e, "copy", &src, &dst, &n);
      
      if(!dynamic_cast<bitfield_expr*>(src)) {
	std::stringstream ss;
	ss << "Source in copy action does not have location type. Currently of type: "
	   << get_node_name(src->ty) << '\n';
	throw type_error(cc::get_location(e), ss.str());
      }
      
      if(!dynamic_cast<bitfield_expr*>(dst)) {
	std::stringstream ss;
	ss << "Destination of copy must be of type location. Currently of type: "
	   << get_node_name(dst->ty) << '\n';
	throw type_error(cc::get_location(e), ss.str());
      }
      
      if(!(dynamic_cast<int_expr*>(n))) {
	std::stringstream ss;
	ss << "Length of copy must be of type int. Currently of type: "
	   << get_node_name(n->ty) << '\n';
	throw type_error(cc::get_location(e), ss.str());
      }

      return cxt.make_copy_action(src, dst, n);
    }
    
    if(*action_name == "set") {
      expr* f;
      expr* v;
      match_list(e, "set", &f, &v);
      return cxt.make_set_action(f, v);
    }
    
    if(*action_name == "write") {
      action* a;
      match_list(e, "write", &a);
      
      return cxt.make_write_action(a);
    }
    
    if(*action_name == "clear") {
      return cxt.make_clear_action();
    }

    if(*action_name == "drop") {
      return cxt.make_drop_action();
    }

    if(*action_name == "match") {
      return cxt.make_match_action();
    }

    if(*action_name == "goto") {
      expr* dst;
      match_list(e, "goto", &dst);

      return cxt.make_goto_action(dst);
    }
    
    if(*action_name == "output") {
      expr* dst;
      match_list(e, "output", &dst);
      
      return cxt.make_output_action(dst);
    }
    
    sexpr::throw_unexpected_term(e);
  }
  
  expr*
  translator::trans_expr(const sexpr::expr* e)
  {
    // Match phrases.
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      symbol* sym;
      match_list(list, &sym);

      auto it = expression_symbols.find(sym);

      if(it == expression_symbols.end()) {
	std::stringstream ss;
	ss << "Invalid expression kind: " << *(sym);
	throw syntax_error(cc::get_location(e), ss.str());
      }

      switch(it->second) {
      case es_int:
	return trans_int_expr(list);
      
      case es_wildcard:
	return trans_wild_expr(list);
      case es_range:
	return trans_range_expr(list);
      case es_port:
	return trans_port_expr(list);
      case es_reserved_port:
	return trans_reserved_port_expr(list);
      case es_bitfield:
	return trans_bitfield_expr(list);
      case es_miss:
	return trans_miss_expr();
      case es_named_field:
	return trans_named_field_expr(list);
      case es_ref:
	return trans_ref_expr(list);
      }
    }
  }
  
  expr_seq
  translator::trans_exprs(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      expr_seq exprs;
      for (const sexpr::expr* elem : list->exprs) {
	expr* e = trans_expr(elem);
	exprs.push_back(e);
      }
      return exprs;
    }
    sexpr::throw_unexpected_term(e);
  }
  
  expr*
  translator::trans_range_expr(const sexpr::list_expr* e)
  {
    expr* lo;
    expr* hi;
    match_list(e, "range", &lo, &hi);
    
    auto lo_expr = as<int_expr>(lo);    
    if(!lo_expr) {
      std::stringstream ss;
      ss << "Expression 1 in range not of int type. Currently of type: " << get_node_name(lo->ty);
      throw type_error(cc::get_location(e), ss.str());
    }
    
    auto hi_expr = as<int_expr>(hi);
    if(!hi_expr) {
      std::stringstream ss;
      ss << "Expression 2 in range not of int type. Currently of type: " << get_node_name(hi->ty);
      throw type_error(cc::get_location(e), ss.str());
    }
    
    auto lo_val = lo_expr->val;
    auto hi_val = hi_expr->val;
    
    auto lo_ty = static_cast<int_type*>(lo_expr->ty);
    auto hi_ty = static_cast<int_type*>(hi_expr->ty);
    
    if(lo_ty->width == hi_ty->width)
      return cxt.make_range_expr(new range_type(
				   new int_type(lo_ty->width)), lo_val, hi_val);
    std::stringstream ss;
    ss << "Width of range arguments not equal: " << lo_ty->width << ", and "
       << hi_ty->width << "\n";
    throw type_error(cc::get_location(e), ss.str());
  }
  
  expr*
  translator::trans_wild_expr(const sexpr::list_expr* e)
  {
  }
  
  expr*
  translator::trans_miss_expr()
  {
    // FIXME: this should correspond to the match kind of the table.
    return cxt.make_miss_expr(new int_type(64));
  }
  
  expr*
  translator::trans_ref_expr(const sexpr::list_expr* e)
  {
    symbol* ref;
    match_list(e, "ref", &ref);

    return cxt.make_ref_expr(new ref_type, ref);
  }
  
  expr*
  translator::trans_named_field_expr(const sexpr::list_expr* e)
  {
    symbol* field;
    match_list(e, "named_field", &field);

    // TODO: move decoding to resolver
    return field_decoder.decode_named_field(
      static_cast<named_field_expr*>(cxt.make_named_field_expr(new loc_type, field)));
  }
  
  expr*
  translator::trans_port_expr(const sexpr::list_expr* e)
  {
    expr* port;
    match_list(e, "port", &port);

    if(get_kind(port) != ek_int) {
      std::stringstream ss;
      ss << "Port value must be of type int. Currently of type: " << get_node_name(port->ty);
      throw type_error(cc::get_location(e), ss.str());
    }
    
    return cxt.make_port_expr(new port_type, port);
  }

  expr*
  translator::trans_reserved_port_expr(const sexpr::list_expr* e)
  {
    symbol* port_name;
    match_list(e, "reserved_port", &port_name);

    auto find = port_names.find(*port_name);
    if(find == port_names.end()) {
      std::stringstream ss;
      ss << "Invalid reserved port: \"" << *port_name << "\"";
      throw syntax_error(cc::get_location(e), ss.str());
    }

    return cxt.make_port_expr(new port_type, port_name);
  }
  
  expr*
  translator::trans_bitfield_expr(const sexpr::list_expr* e)
  {
    symbol* space;
    expr* pos;
    expr* len;
    
    match_list(e, "bitfield", &space, &pos, &len);

    address_space as;
    auto it = address_spaces.find(space);
    if(it != address_spaces.end())
      as = it->second;
    else {
      std::stringstream ss;
      ss << "Invalid address space: " << *(space);
      throw syntax_error(cc::get_location(e), ss.str());
    }

    int_type* pos_ty = dynamic_cast<int_type*>(pos->ty);
    if(!pos_ty) {
      std::stringstream ss;
      ss << "Position in bitfield expression must be of type int. Currently of type: " <<
	get_node_name(pos->ty);
      throw type_error(cc::get_location(e), ss.str());
    }

    int_type* len_ty = dynamic_cast<int_type*>(len->ty);
    if(!len_ty) {
      std::stringstream ss;
      ss << "Length in bitfield expression must be of type int. Currently of type: " <<
	get_node_name(len->ty);
      throw type_error(cc::get_location(e), ss.str());
    }
    
    return cxt.make_bitfield_expr(as, pos, len);
  }
  
  // When given an integer width specifier, such as i32,
  // return the integer after the i. In this case, 32.
  static int
  deduce_int_type_width(const sexpr::list_expr* e, const symbol* ty)
  {
    std::string spec = *ty;
    
    if(spec.front() == 'i') {
      spec.erase(0, 1);
      
      std::size_t size;
      auto width = std::stoi(spec.c_str(), &size);

      if(width > 64 || width < 1) {
	std::stringstream ss;
	ss << "Integer width specifier must be between 1 and 64."
	   << " Currently of value: " << width;
	throw syntax_error(cc::get_location(e), ss.str());
      }
      
      if(spec.size() == size)
	return width;
    }
    
    std::stringstream ss;
    ss << "Invalid integer width specifier: " << *ty <<
      ". Specifier should be of form i[integer].\n";
    throw syntax_error(cc::get_location(e), ss.str());
  }
  
  expr*
  translator::trans_int_expr(const sexpr::list_expr* e)
  {
    symbol* width_specifier;
    int value;
    match_list(e, "int", &width_specifier, &value);
    
    int w = deduce_int_type_width(e, width_specifier);
    
    return cxt.make_int_expr(new int_type(w), value);
  }
  
  // -------------------------------------------------------------------------- //
  // Matching
  
  void 
  translator::match(const sexpr::list_expr* list, int n, decl_seq* decls)
  {
    *decls = trans_decls(get(list, n));
  }
  
  void 
  translator::match(const sexpr::list_expr* list, int n, rule_seq* rules)
  {
    *rules = trans_rules(get(list, n));
  }
  
  void 
  translator::match(const sexpr::list_expr* list, int n, expr_seq* exprs)
  {
    *exprs = trans_exprs(get(list, n));
  }
  
  void
  translator::match(const sexpr::list_expr* list, int n, expr** out)
  {
    *out = trans_expr(get(list, n));
  }
  
  void 
  translator::match(const sexpr::list_expr* list, int n, action_seq* actions)
  {
    *actions = trans_actions(get(list, n));
  }
  
  void 
  translator::match(const sexpr::list_expr* list, int n, action** a)
  {
    *a = trans_action(list);
  }
} // namespace pip
