// utest.cpp : �������̨Ӧ�ó������ڵ㡣
//
int log_bt2 = 0;
#include "gtest/gtest.h"
//
int main(int argc, char* argv[])
{
    std::cout << "Running main() from gtest_main.cc\n";  

    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS(); 
    printf("enter for exit\n");
    getchar();

	return 0;
}

