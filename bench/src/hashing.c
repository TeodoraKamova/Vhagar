int xorshift(int key)
{
    key = key ^ (key << 13);
    key = key ^ (key >> 17);
    key = key ^ (key <<  5);

    return key;
}

int checksum(int *data, int size)
{
    int i;
    int r;
    r = 858993459;

    i = 0;
    while (i < size)
    {
        r = r ^ xorshift(data[i]);

        ++i;
    }

    return r;
}
