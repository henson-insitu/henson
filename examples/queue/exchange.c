#include <stdio.h>

#include <henson/data.h>

int main()
{
    while (!henson_queue_empty("test"))
    {
        int x; double y;
        henson_load_int("test", &x);
        henson_load_double("test", &y);

        printf("Got %d %f in exchange\n", x, y);
    }

    henson_save_int("test", 41);
    henson_save_float("test", 9.8);
    henson_save_int("test", 42);
    henson_save_float("test", 15.2);
}
