# Vhagar's Test Suite

Hello and welcome!
We wrote this systemtest suite in python which takes your compiled binary `build/debug/c4`,
runs it over a small snippet of code and compares the output to what the test expected.
You can run the tests via `python3 systemtests/run_tests.py` from your projects root folder.

If you only want to test specific flags of your compiler, just add them to the script, e.g.
`python3 systemtests/run_tests.py --tokenize`.

The output will look something like this but more colorful:

```
[Vhagar]: tokenize/punctuators_digraphs.test failed!
  tokenize/punctuators_digraphs.c:1:1: punctuator <:
- tokenize/punctuators_digraphs.c:2:2: punctuator %>
                                    ^             ^
+ tokenize/punctuators_digraphs.c:2:1: punctuator :>
                                    ^             ^
  tokenize/punctuators_digraphs.c:3:1: punctuator <%
  tokenize/punctuators_digraphs.c:4:1: punctuator %>
  tokenize/punctuators_digraphs.c:5:1: punctuator %:
  tokenize/punctuators_digraphs.c:6:1: punctuator %:%:
[Vhagar]: tokenize/identifier.test passed!
[Vhagar]: tokenize/function_call.test passed!
[Vhagar]: tokenize/string_basics.test passed!
```

## How to write new tests?

Adding new tests is super simple!

1. Create a new `.test` file in the corresponding folder
2. Write your input C code
3. put a seperating line which starts with `##`
4. and then write what you expected from stdout/stderr below

That's it. The framework will split the file and hide the code and the logs seperately in the `systemtests/.cache` folder.
If you want to debug your implementation with a specific test case you can find the `.c` file in there.

```
int main(int argc, char **argv) {
    return 0;
}
## expected output:
tokenize/main_function.c:1:1: keyword int
tokenize/main_function.c:1:5: identifier main
tokenize/main_function.c:1:9: punctuator (
tokenize/main_function.c:1:10: keyword int
tokenize/main_function.c:1:14: identifier argc
tokenize/main_function.c:1:18: punctuator ,
tokenize/main_function.c:1:20: keyword char
tokenize/main_function.c:1:25: punctuator *
tokenize/main_function.c:1:26: punctuator *
tokenize/main_function.c:1:27: identifier argv
tokenize/main_function.c:1:31: punctuator )
tokenize/main_function.c:1:33: punctuator {
tokenize/main_function.c:2:5: keyword return
tokenize/main_function.c:2:12: constant 0
tokenize/main_function.c:2:13: punctuator ;
tokenize/main_function.c:3:1: punctuator }
```
