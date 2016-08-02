#include <iostream>
#include <string>
#include <memory>
#include "chaiscript.hpp"
#include "chaiscript_stdlib.hpp"
        
        
typedef std::shared_ptr<chaiscript::ChaiScript> ChaiScriptSharedPtr;

class TestClass
{

    public:
        TestClass():test1(1), test2(3.5) {}
        TestClass(chaiscript::ChaiScript * chai):test1(1), test2(3.5), chai_(chai) {}
        TestClass(int test, double testz, chaiscript::ChaiScript *chai):test1(test), test2(testz), chai_(chai) {}
        int getTest1() { return test1; }
        double getTest2() { return test2; }
        
        void chaiZigZagTest(std::string function)
        {
            chai_->eval(function);
        }


    private:
        int                         test1;
        double                      test2;
        chaiscript::ChaiScript *    chai_;

};

int main(int argc, char** argv)
{

    if(argc != 2)
        std::cout << "Usage: ./chai_jump_test CHAI_SCRIPT\n";

    chaiscript::ChaiScript chai(chaiscript::Std_Lib::library());

    chai.add(chaiscript::user_type<TestClass>(), "TestClass");
    chai.add(chaiscript::fun(&TestClass::getTest1), "getTest1");
    chai.add(chaiscript::fun(&TestClass::getTest2), "getTest2");
    chai.add(chaiscript::fun([&chai] () { return std::make_shared<TestClass>(&chai); }), "TestClass");
    chai.add(chaiscript::fun(&TestClass::chaiZigZagTest), "chaiZigZagTest");
    //chai.add(chaiscript::fun(&TestClass::resetInstance), "resetInstance");
    

    chai.eval_file(argv[1]);


    std::cout << "Finished successfully?\n";

    
   
    return 0;

}
