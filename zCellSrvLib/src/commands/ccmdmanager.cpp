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

void CCmdManager::execute(const std::string &cmd_line, const CCommand::args_map_t &ext_args)
{
    std::vector<std::string> cmd_list = split(cmd_line, ' ');
    if (cmd_list.empty())
        throw CException("Command pars error: command line is empty");
    std::string cmd_name = cmd_list.at(0);
    if (cmd_name.empty() || (m_commands.find(cmd_name) == m_commands.end()))
        throw CException("Command pars error: unknown command [" + cmd_name + "]");
    command_ptr_t cmd = m_commands[cmd_name];
    if (!cmd)
        throw CException("Command pars error: command not registered [" + cmd_name + "]");
    std::map<std::string, std::any> args;
    if (cmd_list.size() - 1 != cmd->convertors().size()*2) {
        throw CException("Command pars error: more arguments of the function are required [" +
                         cmd_name + "]");
    }
    for (int idx = 1; idx < cmd_list.size(); idx += 2) {
        if (idx + 1 >= cmd_list.size())
            break;
        std::string arg_name = cmd_list.at(idx);
        std::string arg_val_str = cmd_list.at(idx + 1);
        if (cmd->convertors().find(arg_name) == cmd->convertors().end())
            throw CException("Command pars error: unknown argument [" + arg_name + "]");
        if (!cmd->convertors().at(arg_name)) {
            throw CException("Command pars error: convertors for argument [" + arg_name +
                             "] not registered");
        }
        std::any any_val;
        if (!cmd->convertors().at(arg_name)->convert(arg_val_str, any_val)) {
            throw CException("Command pars error: can't convert argument [" + arg_name +
                             "]");
        }
        args[arg_name] = any_val;
    }
    for (const auto &ext_arg_pair : ext_args) {
        if ((args.find(ext_arg_pair.first) == args.end()) && (!ext_arg_pair.first.empty()))
            args[ext_arg_pair.first] = ext_arg_pair.second;
    }
    if (cmd->is_multy_thread()) {
        if (m_thread_pool != nullptr) {
            auto cmd_exe = cmd->get_cmd_execution_instace();
            cmd_exe->set_args(args);
            m_thread_pool->add_job(cmd_exe);
        }
    } else {
        cmd->execute(args);
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
