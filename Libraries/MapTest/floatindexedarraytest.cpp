#include "tests.h"

void fillAndRecoverElements();
void varyingIndexSizes();

TestFunction FLOAT_INDEXED_ARRAY_TESTS[] = {
    fillAndRecoverElements,
    varyingIndexSizes,
    0
};

void fillAndRecoverElements()
{
    cout << "fillAndRecoverElements..." << endl;

    FloatIndexedArray<int> fia(100, 0.0, 100.0);
    for(int i = 0; i < 100; i++) {
        int *single, length;
        length = fia.cells((double) i + 0.25, (double) i + 0.75, single);
        assert(length == 1);
        single[0] = i;
    }
    FloatIndexedArray<int>::T *all;
    int length;
    length = fia.cells(0.0, 100.0, all);
    assert(length == 100);
    for(int i = 0; i < 100; i++) {
        assert(all[i] == i);
    }
}

void varyingIndexSizes()
{
    cout << "varyingIndexSizes..." << endl;

    double min = -100; double max = 1003383548874; int N = 1000000;
    double step = (max - min) / (double) N; double eps = 0.1 * step;
    FloatIndexedArray<int> fia(N, min, max);
    for(int i = 1; i < N; i++) {
        int *sub, length;
        length = fia.cells(min + eps, min + i*step - eps, sub);
        assert(length == i);
    }
}
