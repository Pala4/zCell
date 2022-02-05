#include "ccmdmanager.h"

#include <sstream>

#include "ccommand.h"
#include "thread/cthreadpool.h"

using namespace zcell_lib;

void CCmdManager::set_thread_pool(CThreadPool *thread_pool)
{
    m_thread_pool = thread_pool;
}

void CCmdManager::create_command(const std::string &cmd_name, const bool &multy_thread,
                                 const CJob::function_t &func)
{
    if (cmd_name.empty() || (m_commands.find(cmd_name) != m_commands.end()))
        return;
    command_ptr_t cmd = std::make_shared<CCommand>(cmd_name, multy_thread, func);
    m_commands[cmd_name] = cmd;
}

void CCmdManager::execute(const std::string &cmd_line, const CJob::args_map_t &ext_args)
{
    std::vector<std::string> cmd_list = split(cmd_line, ' ');
    if (cmd_list.empty())
        return;
    std::string cmd_name = cmd_list.at(0);
    if (cmd_name.empty() || (m_commands.find(cmd_name) == m_commands.end()))
        return;
    std::map<std::string, std::any> args;
    if (cmd_list.size() > 1) {
        for (int idx = 1; idx < cmd_list.size(); ++idx) {
            if (idx + 1 >= cmd_list.size())
                break;
            args[cmd_list.at(idx)] = cmd_list.at(idx + 1);
        }
    }
    for (const auto &ext_arg_pair : ext_args) {
        if ((args.find(ext_arg_pair.first) == args.end()) && (!ext_arg_pair.first.empty()))
            args[ext_arg_pair.first] = ext_arg_pair.second;
    }
    if (m_commands[cmd_name]) {
        if (m_commands[cmd_name]->is_multy_thread()) {
            if (m_thread_pool != nullptr) {
                m_commands[cmd_name]->set_args(args);
                m_thread_pool->add_job(m_commands[cmd_name].get());
            }
        } else {
            m_commands[cmd_name]->execute(args);
        }
    }
}

std::vector<std::string> CCmdManager::split(const std::string &s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
      if (!item.empty())
          elems.push_back(std::move(item));
  }
  return elems;
}
