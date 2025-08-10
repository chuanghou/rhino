#include <iostream>
#include <string>

#ifndef COMPILE_WHEN_TEST
#include <boost/program_options.hpp>
int main(int argc, char* argv[]) {
    boost::program_options::options_description opts("demo options");
    opts.add_options()
        ("about,a", "this is a about.")
        ("version,v", "program version");//两个程序选项
    boost::program_options::variables_map vm;//选项存储map容器
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opts), vm);//解析存储
    if (vm.empty())
    {
        std::cout << "no options" << std::endl;
        return -1;
    }
    if (vm.count("about"))
    {
        std::cout << std::endl << opts << std::endl;
    }
    if (vm.count("version"))
    {
        std::cout <<"version is V3.3.0" << std::endl;
    }
    return 0;
}
#endif