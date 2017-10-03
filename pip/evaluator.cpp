#include "evaluator.hpp"
#include "action.hpp"

namespace pip
{

  evaluator::evaluator(context& cxt, decl* prog, cap::packet& pkt)
    : cxt(cxt), 
      prog(prog), 
      data(pkt), 
      arrival(pkt.timestamp()),
      ingress_port(), 
      physical_port(), 
      metadata(), 
      keyreg(), 
      decode()
  {
    // TOOD: Perform static initialization. We need to evaluate all of
    // the table definitions to load them with their static rules.

    // TODO: Load the instructions from the first table.
  }

  const action*
  evaluator::fetch()
  {
    action* a = eval.front();
    eval.pop_front();
    return a;
  }

  void
  evaluator::step()
  {
    // If the action queue is empty, proceed to egress processing. This
    // will execute the actions in the action list. If the action list
    // is empty, then the program is complete.
    if (eval.empty()) {
      if (actions.empty())
        return;

      // This marks the beginning of egress processing.
      eval.insert(eval.end(), actions.begin(), actions.end());
      actions.clear();
    }
    
    const action* a = fetch();
    switch (get_kind(a)) {
      ak_advance:
        return eval_advance(cast<advance_action>(a));
      ak_copy:
        return eval_copy(cast<copy_action>(a));
      ak_set:
        return eval_set(cast<set_action>(a));
      ak_write:
        return eval_write(cast<write_action>(a));
      ak_clear:
        return eval_clear(cast<clear_action>(a));
      ak_drop:
        return eval_drop(cast<drop_action>(a));
      ak_match:
        return eval_match(cast<match_action>(a));
      ak_goto:
        return eval_goto(cast<goto_action>(a));
      ak_output:
        return eval_output(cast<output_action>(a));
      
      default:
        break;   
    }

    throw std::logic_error("invalid action");
  }

  void
  evaluator::run()
  {
    while (!done())
      step();
  }

  void
  evaluator::eval_advance(const advance_action* a)
  {

  }

  void
  evaluator::eval_copy(const copy_action* a)
  {

  }

  void
  evaluator::eval_set(const set_action* a)
  {

  }

  void
  evaluator::eval_write(const write_action* a)
  {

  }

  void
  evaluator::eval_clear(const clear_action* a)
  {

  }

  void
  evaluator::eval_drop(const drop_action* a)
  {

  }

  void
  evaluator::eval_match(const match_action* a)
  {

  }

  void
  evaluator::eval_goto(const goto_action* a)
  {

  }

  void
  evaluator::eval_output(const output_action* a)
  {

  }



} // namespace pip
