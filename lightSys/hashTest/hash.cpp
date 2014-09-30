#include <iostream>
using namespace std;

signed char hash(signed char x) {
	signed char addr = 0;
	for (unsigned char i=0; i<8; i++) {
		char digit = (x >> i) & 1;
		addr = digit + (addr << 2) + (addr << 4) - addr;
	}
	return addr;
}

int main() {
	int sum = 0;
	for (int i=-127; i<128; i++) {
		cout << (int) hash(i) << endl;
		sum += hash(i);
	}
	cout << sum / 256.0 << endl;
	return 0;
}
