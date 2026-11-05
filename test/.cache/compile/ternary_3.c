int main(int argc, char** argv)
{
    int res;
    res = argc;                       // res == 1
    res = (res == 1) ? ++res : ++res; // res == 2
    return res != 2;                  // return 0
}
