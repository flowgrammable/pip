#include "type_checker.hpp"

#include "decl.hpp"

namespace pip
{

void type_checker::check()
{
  for(auto d : prog->decls) {
    table_decl* td = static_cast<table_decl*>(d);
    check(td);
  }
}

void type_checker::check(table_decl* d)
{
  // TODO: check prep as well

  for(auto r : d->rules) {
    check(r->key);
    // TODO: check acts
  }
}

void type_checker::check(expr* e)
{  
}

}
