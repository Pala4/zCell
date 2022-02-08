#include "ccmdmanager.h"

#include <sstream>

#include "ccommand.h"
#include "thread/cthreadpool.h"

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

bool CCmdManager::execute(const std::string &cmd_line, const CCommand::args_map_t &ext_args)
{
    std::vector<std::string> cmd_list = split(cmd_line, ' ');
    if (cmd_list.empty())
        return false;
    std::string cmd_name = cmd_list.at(0);
    if (cmd_name.empty() || (m_commands.find(cmd_name) == m_commands.end()))
        return false;
    command_ptr_t cmd = m_commands[cmd_name];
    if (!cmd)
        return false;
    std::map<std::string, std::any> args;
    if (cmd_list.size() - 1 != cmd->convertors().size()*2)
        return false;
    for (int idx = 1; idx < cmd_list.size(); idx += 2) {
        if (idx + 1 >= cmd_list.size())
            break;
        std::string arg_name = cmd_list.at(idx);
        std::string arg_val_str = cmd_list.at(idx + 1);
        if (cmd->convertors().find(arg_name) == cmd->convertors().end())
            return false;
        if (!cmd->convertors().at(arg_name))
            return false;
        std::any any_val;
        if (!cmd->convertors().at(arg_name)->convert(arg_val_str, any_val))
            return false;
        args[arg_name] = any_val;
    }
    for (const auto &ext_arg_pair : ext_args) {
        if ((args.find(ext_arg_pair.first) == args.end()) && (!ext_arg_pair.first.empty()))
            args[ext_arg_pair.first] = ext_arg_pair.second;
    }
    if (cmd->is_multy_thread()) {
        if (m_thread_pool != nullptr) {
            cmd->set_args(args);
            m_thread_pool->add_job(cmd.get());
        }
    } else {
        cmd->execute(args);
    }

    return true;
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
