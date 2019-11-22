// netlib_utest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "gtest/gtest.h"

//
int _tmain(int argc, _TCHAR* argv[])
{
    std::cout << "Running main() from gtest_main.cc\n";

    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    printf("enter to exit\n");
    getchar();

    return 0;
}

