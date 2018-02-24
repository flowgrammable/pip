#pragma once

#include <pip/syntax.hpp>
#include <pip/pcap.hpp>
#include <pip/decoder.hpp>

#include <deque>
#include "decode.hpp"

namespace pip
{
  /// A queue of instructions.
  using action_queue = std::deque<const action*>;


  /// Evaluates a pipeline for a single packet.
  ///
  /// \note The evaluator contains all of the information that is typically
  /// associated with the "packet context". Essentially, the packet context
  /// is just part of the evaluation context for a pip program.
  ///
  /// \todo The evaluator should take any packet. Not just a pcap packet.
  /// That implies either that packets have a base class or that this should
  /// be a template. Neither is great. Find a better solution.
  ///
  /// \todo This is really a virtual machine. Perhaps rename this 'machine'.
  class evaluator
  {
  public:
    evaluator(context& cxt, decl* prog, cap::packet& pkt);

    ~evaluator();

    /// Returns true if the program is finished.
    bool done() const { return eval.empty(); }
    
    /// Execute the next action.
    void step();
    
    /// Execute the program.
    void run();

  private:
    /// Fetch the next instruction from the evaluation queue.
    const action* fetch();

    void eval_advance(const advance_action* a);
    void eval_copy(const copy_action* a);
    void eval_set(const set_action* a);
    void eval_write(const write_action* a);
    void eval_clear(const clear_action* a);
    void eval_drop(const drop_action* a);
    void eval_match(const match_action* a);
    void eval_goto(const goto_action* a);
    void eval_output(const output_action* a);

  private:
    /// Various program facilities.
    context& cxt;

    /// Facilities to decode field expressions
    decoder dec;

    /// The pip program to execute.
    decl* prog;

    /// The packet to execute the program on.
    cap::packet& data;

    /// The time at which the packet arrive.
    ///
    /// \todo Openflow 5.1 uses a timeval-like structure that encodes seconds
    /// and nanoseconds. timeval has seconds and microseconds. 
    timeval arrival;

    /// The (possibly logical) port on which the packet arrived.
    std::uint32_t ingress_port;

    /// The physical port on which the packet arrived.
    std::uint32_t physical_port;

    /// The port on which the packet will be outputted after processing.
    std::uint32_t egress_port;

    /// Dynamic metadata. This can be written to by copy actions.
    std::uint64_t metadata;

    /// The key used for table lookup. This can be written to by copy actions.
    std::uint64_t keyreg;

    /// The decoder offset, modified by advance instructions.
    std::uint32_t decode;

    /// The sequence of actions to execute on egress.
    std::vector<const action*> actions;

    /// A copy of the frame to be modified throughout the evaluator.
    unsigned char* modified_buffer;

    /// The sequence of actions being evaluated. Each action is fetched from
    /// the queue in turn. On table lookup, the action list for the matched 
    /// key is added to the end of the queue.
    ///
    /// Note that any time actions are inserted into the queue, it should
    /// be empty. That reinforces the idea that a "goto", "output", "match",
    /// or "drop" action must be a kind of terminator instruction.
    ///
    /// \note We could make this a pair of pointer + index, and point to
    /// the current action-seq being evaluated. However, we could conceivably
    /// end up dropping a rule while it is being executed. In order to avoid
    /// that scenario, we simply copy instructions into the queue.
    action_queue eval;

    /// A list of all tables in the program.
    std::vector<decl*> tables;

    /// The table currently being examined.
    table_decl* current_table = nullptr;
  };


} // namespace pip

