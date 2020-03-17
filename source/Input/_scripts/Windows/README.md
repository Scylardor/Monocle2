# GenerateMonocleKeyboardMap.py

If order to use the script, you need to install the pywin32 module:
- either via *pip install pywin32*
- or directly from the repo at https://github.com/mhammond/pywin32 .

Once it's installed you can then run GenerateMonocleKeyboardMap.py like any other Python script.

First argument you can give it is the file path it will generate a .cpp/.h file pair under.
E.g. "test/generated" to generate "test/generated.h" and "test/generated.cpp".

Second argument is the namespace in which it's going to enclose the generated Keycode variables.