#include <QCoreApplication>

#include "cconclient.h"
#include "commands/cintconvertor.h"
#include "commands/cstringconvertor.h"
#include "thread/cworker.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    CConClient client;

    auto cmd_exit = std::make_shared<zcell_lib::CCommand>();
    cmd_exit->set_name("exit").
            set_async(false).
            set_description("Exit from application").
            set_usage("Exit from application").
            set_function([&client](zcell_lib::CJobBase *job,
                         const zcell_lib::CJob_::args_map_t &args) {
                client.done_input();
                QCoreApplication::exit();
            });
    client.add_command(cmd_exit);

    auto cmd_test = std::make_shared<zcell_lib::CCommand>();
    cmd_test->set_name("test").
            set_async(true).
            set_function([&client](zcell_lib::CJobBase *job,
                         const zcell_lib::CJob_::args_map_t &args) {
        if (job->worker() != nullptr) {
            for (int i = 0; i < 8; ++i) {
                client.output(std::to_string(i) + " from thread wrk id: " +
                              job->id());// std::to_string(job->worker()->id())
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        } else {
            for (int i = 0; i < 10; ++i) {
                client.output(std::to_string(i) + " from unthread");
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
    });
    client.add_command(cmd_test);

    client.init_input();

    return app.exec();
}
