int main(void)
{
    if (sizeof(struct {
            char a;
            struct
            {
                int (*f)(int, int, char);
            } gimmeaname;
        }) != 16)
        return 1;

    struct IKnowWhatImDoing
    {
        char a;
        int b;
        int* c;
    };

    if (sizeof(struct IKnowWhatImDoing) != 16)
        return 2;

    struct HappyPaddingDay
    {
        char a;
        char b;
        char c;
    };
    if (sizeof(struct HappyPaddingDay) != 3)
        return 3;

    struct OkLetsGo
    {
        struct HappyPaddingDay a;
        int* hehe;
        struct HappyPaddingDay b;
    };

    if (sizeof(struct OkLetsGo) != 24)
        return 4;

    if (sizeof("Hello World") != 12)
        return 5;

    if (sizeof(*"Hello World") != 1)
        return 6;

    if (sizeof(sizeof(int)) != 4)
        return 7;

    if (sizeof(sizeof(char**)) != 4)
        return 8;

    if ((sizeof(struct OkLetsGo) + (char)1) != 25)
        return 9;

    if (sizeof(sizeof(struct OkLetsGo) + (char)1) != 4)
        return 10;
}
