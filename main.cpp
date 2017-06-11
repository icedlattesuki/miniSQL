//
//  main.cpp
//  interpreter
//
//  Created by Sr on 2017/5/20.
//  Copyright © 2017年 Sr. All rights reserved.
//

#include <iostream>
#include "interpreter.h"

int main(int argc, const char * argv[]) {
    std::cout<<">>> Welcome to MiniSQL"<<std::endl;
    while(1){
        try {
            Interpreter query;
            query.getQuery();
            query.EXEC();
        }
        catch(table_exist error){
            std::cout<<">>> Error: Table has existed!"<<std::endl;
        }
        catch(table_not_exist error) {
            std::cout<<">>> Error: Table not exist!"<<std::endl;
        }
        catch(exit_command error){
            std::cout<<">>> Bye bye~"<<std::endl;
            break;
        }
        catch(...){
            std::cout<<">>> Error: Input format error!"<<std::endl;
        }
    }
    return 0;
}
