
#include <string>
#include <Python.h>
int main(int argc, char **argv) {
    std::string args;
    if ( argc > 1) {
        args.append("[");
        for (int i = 1; i < argc; i++) {
            if (i > 2)
                args.append(",");
            args.append("\"");
            args.append(argv[i]);
            args.append("\"");
        }
        args.append("]");
    }
    std::string pycode = "import pytest\npytest.main(" + args + ")\n";
    wchar_t * program_name = Py_DecodeLocale(argv[0], NULL);
    Py_SetProgramName(program_name);
    Py_Initialize();
    PyRun_SimpleString(&*pycode.begin());
    Py_Finalize();
    return 0;
}