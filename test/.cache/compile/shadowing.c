int main()
{
    int v_256;
    v_256 = 256;
    int res;
    res = 0;

    {
        char res;
        res = v_256;
        if (res != 0)
            return 1;

        {
            int res;
            res = v_256;
            if (res != v_256)
                return 2;
        }
    }

    return res;
}