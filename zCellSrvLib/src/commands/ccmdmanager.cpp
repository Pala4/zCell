#include "ccmdmanager.h"

#include <sstream>

#include "ccommand.h"
#include "thread/cthreadpool.h"
#include "core/cexception.h"

using namespace zcell_lib;

void CCmdManager::set_thread_pool(CThreadPool *thread_pool)
{
    m_thread_pool = thread_pool;
}

command_ptr_t CCmdManager::create_command(const std::string &cmd_name, const bool &multy_thread,
                                 const CCommand::function_t &func)
{
    if (cmd_name.empty() || (m_commands.find(cmd_name) != m_commands.end()))
        return command_ptr_t();
    command_ptr_t cmd = std::make_shared<CCommand>(cmd_name, multy_thread, func);
    m_commands[cmd_name] = cmd;
    return cmd;
}

bool CCmdManager::add_command(const command_ptr_t &command)
{
    if (!command || command->name().empty() ||
            (m_commands.find(command->name()) != m_commands.end())) {
        return false;
    }
    m_commands[command->name()] = command;
    return true;
}

const CCmdManager::commands_map_t &CCmdManager::commands() const
{
    return m_commands;
}

void CCmdManager::execute(const std::string &cmd_line, const CCommand::args_map_t &ext_args)
{
    command_ptr_t cmd = pars(cmd_line, ext_args);
    if (!cmd)
        return;

    if (cmd->is_async()) {
        if (m_thread_pool != nullptr) {
            auto cmd_exe = cmd->get_cmd_execution_instace();
            m_thread_pool->add_job(cmd_exe);
        }
    } else {
        cmd->execute();
    }
}

std::vector<std::string> CCmdManager::split(const std::string &s, char delim)
{
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
      if (!item.empty())
          elems.push_back(std::move(item));
  }
  return elems;
}

command_ptr_t CCmdManager::pars(const std::string &cmd_line, const CCommand::args_map_t &ext_args)
{
    std::vector<std::string> cmd_list = split(cmd_line, ' ');
    if (cmd_list.empty())
        throw CException("Command pars error: command line is empty");
    std::string cmd_name = cmd_list.at(0);
    if (cmd_name.empty() || (m_commands.find(cmd_name) == m_commands.end())) {
        throw CException("Command pars error: unknown command [" + cmd_name +
                         "]. Use -h/help for more information");
    }
    command_ptr_t cmd = m_commands[cmd_name];
    if (!cmd)
        throw CException("Command pars error: command not registered [" + cmd_name + "]");
    cmd_list.erase(cmd_list.begin());

    int idx = 0;
    std::vector<std::pair<std::string, std::string>> arg_pairs;
    while (idx < cmd_list.size()) {
        auto cmd_part = cmd_list.at(idx);
        auto it = std::find_if(cmd->argument_defs().begin(), cmd->argument_defs().end(),
                               [&cmd_part](const auto &arg_def){return (arg_def.name == cmd_part);});
        std::pair<std::string, std::string> arg_pair;
        if (it == cmd->argument_defs().end()) {
            arg_pair = std::make_pair(std::string(), cmd_part);
            ++idx;
        } else {
            std::string cmd_val_str;
            if (++idx < cmd_list.size())
                cmd_val_str = cmd_list.at(idx);
            arg_pair = std::make_pair(cmd_part, cmd_val_str);
            ++idx;
        }
        arg_pairs.push_back(arg_pair);
    }

    CJobBase::args_map_t args;
    auto num_requireds = std::count_if(cmd->argument_defs().begin(), cmd->argument_defs().end(),
                                       [](const auto &arg_def_pair) {
        return arg_def_pair.required;
    });
    if ((arg_pairs.size() < num_requireds) || (arg_pairs.size() > cmd->argument_defs().size())) {
        throw CException(std::string() + "Command pars error: the number of arguments does not " +
                         "match the function call [" + cmd->name() + "]. " +
                         "Use -h/help for more information");
    }

    for (int idx = 0; idx < arg_pairs.size(); ++idx) {
        auto arg_name = arg_pairs.at(idx).first;
        auto arg_val = arg_pairs.at(idx).second;
        std::any any_val;
        const argument_def *arg_def = nullptr;
        if (!arg_name.empty()) {
            auto arg_def_it = std::find_if(cmd->argument_defs().begin(), cmd->argument_defs().end(),
                                           [&arg_name](const auto &arg_def_pair) {
                return (arg_def_pair.name == arg_name);
            });
            if (arg_def_it == cmd->argument_defs().end()) {
                throw CException("Command pars error: unknown argument [" + arg_name +
                                 "] in command [" + cmd->name() + "]. " +
                                 "Use -h/help for more information");
            }
            arg_def = arg_def_it.base();
        } else {
            assert(idx < cmd->argument_defs().size());
            arg_def = &cmd->argument_defs().at(idx);
            arg_name = arg_def->name;
        }
        try {
            any_val = arg_def->convertor->convert(arg_val);
        } catch (CException &ex) {
            throw CException("Command pars error: can't convert argument [" + arg_name +
                             "] in command [" + cmd->name() + "]. (" + ex.what() + ")" +
                             "Use -h/help for more information");
        }
        args[arg_name] = any_val;
    }

    for (const auto &arg_def : cmd->argument_defs()) {
        if (args.find(arg_def.name) == args.end()) {
            if (arg_def.required) {
                throw CException("Command pars error: missed required argument [" + arg_def.name +
                                 "] in command [" + cmd->name() + "]. " +
                                 "Use -h/help for more information");
            } else {
                args[arg_def.name] = std::make_any<std::string>("");
            }
        }
    }

    for (const auto &ext_arg_pair : ext_args) {
        if ((args.find(ext_arg_pair.first) == args.end()) && (!ext_arg_pair.first.empty()))
            args[ext_arg_pair.first] = ext_arg_pair.second;
    }

    cmd->set_args(args);

    return cmd;
}
