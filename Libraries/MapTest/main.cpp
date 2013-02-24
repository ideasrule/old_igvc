#include "tests.h"

int main(void)
{
    cout << "Occupancy map tests..." << endl;
	for(int i = 0; OCCUPANCY_MAP_TESTS[i] != 0; i++)
	{
		OCCUPANCY_MAP_TESTS[i]();
	}

    cout << endl << "Float indexed array tests..." << endl;
	for(int i = 0; FLOAT_INDEXED_ARRAY_TESTS[i] != 0; i++)
	{
		FLOAT_INDEXED_ARRAY_TESTS[i]();
	}

    cout << "Done!" << endl;

    return 0;
}
