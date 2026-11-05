import os, sys
import argparse
import subprocess
import difflib
import json

from utils2 import compare_outputs

from concurrent.futures import ThreadPoolExecutor, wait

tabwith = 4

# put here the absolute path of the c4/ folder
workspace = os.path.abspath(".")

# put here the absolute path of the systemtests/ folder
# note that this folder can be renamed or moved to other locations
# but then this variable should be updated accordingly
systemtests = os.sep.join([workspace, "systemtests"])

compiler = os.sep.join([workspace, "build", "debug", "c4"])
cache = os.sep.join([systemtests, ".cache"])

dragon = """
                         _,~--~--~--.__                     :
                   ,---:'.,            `-._                 :   Team Vhagar's Systemtest Suite
                        `:.`---.__         `-._             :
                          `:.     `--.         `.           :   runs all tests located in 'systemtests/'.
                            \\.        `.         `.         :   specific folders for corresponding flags.
                    (,,(,    \\.         `.   ____,-`.,      :   each test file contains its logs.
                 (,'     `/   \\.   ,--.___`.'               :       feel free to modify.
              , ,'  ,--.  `,   \\.;'         `               :
              `{   }    \\  :   |:;                          :   Credit: Jonah, Teddy & Tarik
               (◣ ◢)    /  /   /:/
                l;;j   /  ,' ,///.    ,---.      ,          :
                \\.;'  /  ,' /  _  \\  /  _  \\   ,'/          :   We cannot guarantee 100% correctness.
                      \\   `'  / \\  `'  / \\  `.' /           :       Please share your tests & benchmarks.
                       `.___,'   `.__,'   `.__,'            :
"""


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("--all", help="runs all tests with respective flags", action="store_true")
    parser.add_argument("--tokenize", action="store_true", default=False,
        help="runs all tests in systemtests/lexer/ with '--tokenize' flag enabled.")
    parser.add_argument("--print-ast", action="store_true", default=False,
        help="runs all tests in systemtests/parser/ with '--print-ast' flag enabled.")
    parser.add_argument("--parse", action="store_true", default=False,
        help="runs all tests in systemtests/parser/ with '--parse' flag enabled.")
    parser.add_argument("--compile", action="store_true", default=False,
        help="doesnt do anything yet.")
    parser.add_argument("--no-colors", action="store_true", default=False)
    parser.add_argument("--no-semantics", action="store_true", default=False,
        help="runs all parser tests with the additional '--no-semantics' flag enabled.")
    parser.add_argument("--verify", action="store_true", default=False,
        help="runs all compile tests with the additional '--verify' flag enabled.")
    parser.add_argument("--optimize", action="store_true", default=False,
        help="runs all compile tests with the additional '--optimize' flag enabled.")

    args = parser.parse_args()

    # if no specific category is given, run all tests
    if args.tokenize == False \
        and args.parse == False \
        and args.print_ast == False \
        and args.compile == False:

        args.all = True

    # if --all is enabled, then enable everything else
    if args.all:
        args.tokenize = True
        args.parse = True
        args.print_ast = True
        args.compile = True

    global no_colors
    no_colors = args.no_colors

    global optimize
    optimize = args.optimize

    global verify
    verify = args.verify

    return args

## file utils such that we can pass around absolute paths everywhere
## but only use the relevant stuff for printing/modifications

class File:
    def __init__(self, abspath: str):
        self.path = abspath

    # /foo/bar.txt ~~> bar.txt
    def filename(self):
        return os.path.basename(self.path)

    # /foo/bar.txt ~~> bar
    def basename(self):
        filename = self.filename()
        basename, _ = os.path.splitext(filename)
        return basename

    # /home/vhagar/foo/bar.txt ~~> foo/bar.txt
    def displayname(self):
        dirs, filename = os.path.split(self.path)
        _, dir = os.path.split(dirs)

        return os.sep.join([dir, filename])

    # /foo/bar.txt ~~> /foo/bar.txt
    def abspath(self):
        return self.path

    def __str__(self):
        return self.path

    # returns a file object with a changed extension
    # File('/foo/bar.c').with_ext('.ll') ~~> /foo/bar.ll
    def with_ext(self, ext: str):
        newpath, _ = os.path.splitext(self.path)
        return File(newpath + ext)


class TestResult:
    def __init__(self, testname):
        self.testname = testname

    def show_output(self):
        pass

    def show_passed(self):
        print(f"[Vhagar]: {self.testname} passed!")

    def show_failed(self):
        print(f"[Vhagar]: {self.testname} failed!")

class SuccessResult(TestResult):
    def __init__(self, testname):
        super().__init__(testname)

    def show_output(self):
        super().show_passed()

class TimeoutResult(TestResult):
    def __init__(self, testname):
        super().__init__(testname)

    def show_output(self):
        super().show_failed()
        print("[Vhagar]: surpassed time out of 10 seconds.")

class DiffResult(TestResult):
    def __init__(self, testname, deltas):
        super().__init__(testname)

        self.deltas = deltas

    def show_output(self):
        super().show_failed()
        sys.stdout.writelines(self.deltas)
        sys.stdout.flush()

class ExoticResult(TestResult):
    def __init__(self, testname, returncode, stderr):
        super().__init__(testname)

        self.returncode = returncode
        self.stderr = stderr

    def show_output(self):
        super().show_failed()
        print("[Vhagar]: received exotic return code of", self.returncode)
        print(self.stderr)

class CompilationErrorResult(TestResult):
    def __init__(self, testname, returncode, stderr):
        super().__init__(testname)

        self.returncode = returncode
        self.stderr = stderr

    def show_output(self):
        super().show_failed()
        print("[Vhagar]: compilation failed with return code", self.returncode)
        print(self.stderr)

class InterpErrorResult(TestResult):
    def __init__(self, testname, llvmfile, stderr):
        super().__init__(testname)

        self.llvmfile = llvmfile
        self.stderr = stderr

    def show_output(self):
        super().show_failed()
        print("[Vhagar]: lli failed to interpret", self.llvmfile)
        print(self.stderr)

class JsonErrorResult(TestResult):
    def __init__(self, testname, msg, lineno):
        super().__init__(testname)
        self.msg = msg
        self.lineno = lineno

    def show_output(self):
        super().show_failed()
        print("[Vhagar]: failed to parse json. line", self.lineno)
        print("[Vhagar]:", self.msg)
        print()

def run_make():
    print("[Vhagar]: compiling...")
    process = subprocess.run(["make", "-j"], capture_output=True, text=True)

    if process.returncode != 0:
        print("[Vhagar]: compilation failed!")
        print(process.stderr)
        return False

    return True

def create_subfolders():
    os.makedirs(os.sep.join([systemtests, "tokenize"]), exist_ok=True)
    os.makedirs(os.sep.join([systemtests, "parse"]), exist_ok=True)
    os.makedirs(os.sep.join([systemtests, "print_ast"]), exist_ok=True)
    os.makedirs(os.sep.join([systemtests, "compile"]), exist_ok=True)
    os.makedirs(os.sep.join([cache]), exist_ok=True)
    os.makedirs(os.sep.join([cache, "tokenize"]), exist_ok=True)
    os.makedirs(os.sep.join([cache, "parse"]), exist_ok=True)
    os.makedirs(os.sep.join([cache, "print_ast"]), exist_ok=True)
    os.makedirs(os.sep.join([cache, "compile"]), exist_ok=True)
    return True

def listdir2(path):
    files = []

    try:
        for file in os.listdir(path):
            abspath = os.sep.join([path, file])
            _, extension = os.path.splitext(file)

            # If the tests dont end with .test we ignore them
            # You can use this to toggle tests on and off
            if extension != ".test":
                continue

            files.append(File(abspath))

    except FileNotFoundError:
        pass # a non-existing dir cannot contain tests

    return files

def collect_files(args):
    tests = []

    if args.tokenize:
        dir = os.sep.join([systemtests, "tokenize"])
        files = listdir2(dir)
        flags = ["--tokenize"]
        tests += map(lambda file: NormalTest(file, flags), files)

    if args.parse:
        dir = os.sep.join([systemtests, "parse"])
        files = listdir2(dir)
        flags = ["--parse"]
        if args.no_semantics:
            flags.append("--no-semantics")
        tests += map(lambda file: NormalTest(file, flags), files)

    if args.print_ast:
        dir = os.sep.join([systemtests, "print_ast"])
        files = listdir2(dir)
        flags = ["--print-ast"]
        if args.no_semantics:
            flags.append("--no-semantics")
        tests += map(lambda file: NormalTest(file, flags), files)

    if args.compile:
        dir = os.sep.join([systemtests, "compile"])
        files = listdir2(dir)
        flags = ["--compile", "--optimize"]
        if args.verify:
            flags.append("--verify")
        tests += map(lambda file: CompileTest(file, flags), files)

    return tests

def parse_normal_test(test_file: File):
    displayname = test_file.displayname()

    code_file = File(os.sep.join([cache, displayname])).with_ext(".c")
    logs_file = File(os.sep.join([cache, displayname])).with_ext(".log")

    # check whether file has already been split
    # and stored into .cache/ folder
    if os.path.exists(str(code_file)):
        test_age = os.path.getmtime(str(test_file))
        code_age = os.path.getmtime(str(code_file))

        if test_age < code_age:
            return code_file, logs_file

    # otherwise, split and parse files now
    # most test runs won't get to these lines
    # unless you manually tampered with the .cache/ folder
    code = []
    logs = []

    flag = False
    with open(test_file.abspath(), "r") as file:
        for lineno, line in enumerate(file, start=1):

            if line.strip().startswith("##"):
                flag = True
            elif flag:
                # automatically replace spaces with tabs in the test cases
                # so that you dont need to mess with your IDE
                line = line.replace(' '*tabwith, '\t')
                logs.append(line)
            else:
                code.append(line)

    # write processed lines into cache files
    with open(code_file.abspath(), "w+") as file:
        file.writelines(code)

    with open(logs_file.abspath(), "w+") as file:
        file.writelines(logs)

    return code_file, logs_file

def parse_compile_test(test_file: File):
    code_file = File(os.path.join(cache, test_file.displayname())).with_ext(".c")
    json_file = File(os.path.join(cache, test_file.displayname())).with_ext(".json")

    if os.path.exists(code_file.abspath()):
        test_age = os.path.getmtime(test_file.abspath())
        code_age = os.path.getmtime(code_file.abspath())

        if test_age < code_age:
            return code_file, json_file

    code = []
    json = []

    flag = False
    with open(test_file.abspath(), "r") as file:
        for lineno, line in enumerate(file, start=1):
            if line.strip().startswith("##"):
                flag = True
            elif flag:
                json.append(line)
            else:
                code.append(line)

    with open(str(code_file), "w+") as file:
        file.writelines(code)

    with open(str(json_file), "w+") as file:
        if json == []:
            json = ["{}"]

        file.writelines(json)

    return code_file, json_file

def chop_error(line: str):
    index = line.find("error: ")
    if index == -1:
        return line

    return line[:index + 6] + "\n"

class Test:
    def __init__(self, file: File, flags):
        self.file = file
        self.flags = flags

class NormalTest(Test):
    """
    Runs c4 compiler with an input file and expects a certain log output
    """
    def run_test(self):
        filename = self.file.displayname()
        code_file, logs_file = parse_normal_test(self.file)

        args = [compiler] + self.flags + [code_file.displayname()]

        try:
            process = subprocess.run(args, capture_output=True, text=True, timeout=10, cwd=cache)

            if process.returncode == 0:
                output = process.stdout
            elif process.returncode == 1:
                output = process.stderr.splitlines(keepends=True)
                # if error, we are only intereted in the first line
                output = ["" if len(output) == 0 else chop_error(output[0])]
                output = "".join(output)
            else:
                return ExoticResult(filename, process.returncode, process.stderr)

            with open(str(logs_file), "r") as file:
                expected = file.read()

            diff = compare_outputs(output, expected)

            if diff == None:
                return SuccessResult(filename)
            else:
                return DiffResult(filename, diff)
        except subprocess.TimeoutExpired:
            return TimeoutResult(filename)

class CompileTestArgs:
    def __init__(self, data):
        c4 = data.get("c4", {})

        # the flags provided to the c4 compiler
        # defaults to ["--compile"]
        self.flags = c4.get("flags", ["--compile"])

        if optimize:
            self.flags.append("--optimize")
        if verify:
            self.flags.append("--verify")

        # flags can also be given as a single string e.g.
        # "c4": { "flags": "--compile" }
        if type(self.flags) is str:
            self.flags = [self.flags]

        self.return_code = c4.get("return-code", 0)

        # if key "stdout" is not provided it will stay None
        # None indicates that the output from stdout will be ignored
        # same logic applies for the key "stderr"
        self.stdout = c4.get("stdout")
        self.stderr = c4.get("stderr")

    def run_compiler(self, file):
        displayname = file.displayname()
        args = [compiler] + self.flags + [displayname]

        try:
            process = subprocess.run(args, capture_output=True, text=True, timeout=5, cwd=cache)

            if process.returncode != self.return_code:
                return CompilationErrorResult(displayname, process.returncode, process.stderr)

            if self.stderr != None and process.stderr != self.stderr:
                diff = compare_outputs(process.stderr, self.stderr)
                return DiffResult(displayname, diff)

            if self.stdout != None and process.stdout != self.stdout:
                diff = compare_outputs(process.stdout, self.stdout)
                return DiffResult(displayname, diff)

            # c4 successfully compiled a llvm bitcode file
            # which can be located in the systemtests/.cache/ folder

            return None

        except subprocess.TimeoutExpired:
            return TimeoutResult(displayname)

class InterpTestArgs:
    def __init__(self, args):
        # you can provide an extra object which will be linked to your program
        # just provide a string with the C code, it will be compiled using gcc
        self.extra_object = args.get("extra-object")

        self.args = args.get("args", [])
        self.return_code = args.get("return-code", 0)

        self.stdout = args.get("stdout")
        self.stderr = args.get("stderr")

        self.lli_flags = []

        if args.get("c4lib", False):
            self.precompile_c4lib()


    def precompile_c4lib(self):
        subprocess.run(["gcc", "-c", "-xc", "../c4lib.c", "-oc4lib.o"], cwd=cache)
        self.lli_flags += ["--extra-object=" + cache + "/c4lib.o"]

    def precompileExtraObject(self):
        pass

    def run_interp(self, file):
        if self.extra_object:
            self.precompileExtraObject()

        displayname = "compile/" + file.basename() + ".test" # hacky
        lli = os.sep.join([workspace, "llvm", "install", "bin", "lli"])
        args = [lli] + self.lli_flags + [file.filename()] + self.args

        try:
            process = subprocess.run(args, capture_output=True, text=True, timeout=10, cwd=cache)

            if process.returncode != self.return_code:
                return ExoticResult(displayname, process.returncode, process.stderr)

            if self.stderr != None and process.stderr != self.stderr:
                diff = compare_outputs(process.stderr, self.stderr)
                return DiffResult(displayname, diff)

            if self.stdout != None and process.stdout != self.stdout:
                diff = compare_outputs(process.stdout, self.stdout)
                return DiffResult(displayname, diff)

            # everything fine
            return None

        except subprocess.TimeoutExpired:
            return TimeoutResult(file.filename())

class CompileTest(Test):
    """
    Runs c4 compiler with `--compile` flag, which should produce a .ll file.
    It then runs the bitcode interpreter lli with input file
    and expects a certain returncode from that program execution.
    """
    def run_test(self):
        lli = os.sep.join([workspace, "llvm", "install", "bin", "lli"])

        code_file, json_file = parse_compile_test(self.file)

        llvm_file = File(os.sep.join([cache, self.file.basename() + ".ll"]))
        args = [compiler] + self.flags + [code_file.displayname()]

        try:
            with open(str(json_file), "r") as file:
                data = json.load(file)
        except json.JSONDecodeError as e:

            return JsonErrorResult(self.file.displayname(), e.msg, e.lineno)


        res = CompileTestArgs(data.get("c4", {})).run_compiler(code_file)

        if res != None:
            return res

        for args in data.get("lli", [{}]):
            interp = InterpTestArgs(args)
            res = interp.run_interp(llvm_file)

            if res != None:
                return res

        return SuccessResult(self.file.displayname())


def main():
    print(dragon)

    args = parse_args()

    with ThreadPoolExecutor() as exec:
        future_make = exec.submit(run_make)
        future_mkdir = exec.submit(create_subfolders)
        future_ls = exec.submit(collect_files, args)

        # waits until make & mkdir have successfully finished
        if not all(wait([future_mkdir, future_make])[0]):
            return

        # waits and then extracts the test files to run
        tests = future_ls.result()

        print("[Vhagar]: running tests...")
        results = exec.map(lambda test: test.run_test(), tests)

        total = len(tests)
        passed = 0
        for result in results:
            if isinstance(result, SuccessResult):
                passed += 1

            result.show_output()

        print(f"[Vhagar]: passing {passed} / {total} tests.")



if __name__ == '__main__':
    main()
