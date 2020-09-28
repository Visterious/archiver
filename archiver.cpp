#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include <cmath>
#include <fstream>
#include <string.h>
#include <vector>
#include <algorithm>
using namespace std;

// Class for numbers with the number of their 
// occurrences in the source file and its bitness
class NumObj {
public:
	int num, count, bits;

	// overloading the <operator for sorting
	bool operator<(NumObj t) const {
		return count*bits < t.count*t.bits;
	}
};

void compression(string filename, int size, FILE* of, int* O, int* N, int* S);
void decompression(string filename, FILE* of, int* O, int* N, int* S);
static void usage();
static int count_of_bits(int num);


int main(int argc, char const *argv[]) {
	const int size = 100000;
	int OldBits = 0, NewBits = 0, Swaps = 0;
	string filename;
	bool mode;

	if (argc < 3) {
		usage();
		exit(1);
	} else {
		filename = argv[1];
		if (strcmp(argv[2], "-comp") == 0) mode = 1;
		else if (strcmp(argv[2], "-decomp") == 0) mode = 0;
		else {
			usage();
			exit(1);
		}
	}

	if (mode) {
		FILE* of = fopen("CD.dat", "wt");
		compression(filename, size, of, &OldBits, &NewBits, &Swaps);
		fclose(of);
	} else {
		FILE* of = fopen("DC.txt", "wt");
		decompression(filename, of, &OldBits, &NewBits, &Swaps);
		fclose(of);
	}

	// statistic
	cout << "-- STATISTICS --\n";
	cout << "Were bits: " << OldBits << endl;
	cout << "Became bits: " << NewBits << endl;
	cout << "Swaps: " << Swaps << endl;
}

// file compression function
void compression(string filename, int size, FILE* of, int* O, int* N, int* S) {
	char buf[16]; // buffer for a read line
	int x = 0, itr = 0, OldBits = 0, NewBits = 0, Swaps = 0; // variables :D
	vector<int> ListOfNums;
	unordered_map<int, int> m, conformity, reverse_conf;

	vector<NumObj> ListOfNumObjs;
	NumObj obj;

	// checking for file existence
	FILE* in;
	if ((in = fopen(filename.c_str(), "rt")) == NULL) {
		printf("File %s not found!\n", filename.c_str());
		exit(2);
	}

	// segment reading
	while (1) {
		// file reading
		while (itr < size && fgets(buf, 16, in)) {
			if (buf == "") continue;
			x = atoi(buf);
			m[x]++;
			OldBits += count_of_bits(x);
			ListOfNums.push_back(x);
			itr++;
		}

		// data check
		if (m.empty()) break;

		unordered_map<int, int>::iterator ii;

		// filling of ListOfNumObjs
		for (ii = m.begin(); ii != m.end(); ++ii) {
			obj.num = ii->first;
			obj.count = ii->second;
			obj.bits = count_of_bits(ii->first);
			ListOfNumObjs.push_back(obj);		}

		vector<NumObj>::iterator i;

		// sorting of ListOfNumObjs
		std::sort(ListOfNumObjs.begin(), ListOfNumObjs.end());


		// replacement algorithm
		int num = 0, temp = 0, cobNum, cobINum, mC;
		bool cond;
		x = conformity.size();
		for (i = ListOfNumObjs.end(), i--; i != ListOfNumObjs.begin(); --i) {
			while (1) {
				num++;
				if (num > 30) {
					num = temp;
					break;
				}
				cobNum = count_of_bits(num);
				cobINum = count_of_bits(i->num);
				mC = m[i->num];
				cond = cobNum * mC + (cobNum + cobINum) + 1 < cobINum * mC;
				if (!m[num] && cond) {
					conformity[i->num] = num;
					reverse_conf[num] = i->num;
					break;
				}
				else if (m[num] && i->count > m[num] && cond) {
					conformity[i->num] = num;
					reverse_conf[num] = i->num;
					break;
				}
			}
			temp = num;
		}
		Swaps += conformity.size();


		// write a heading
		fputs("*\n", of);
		for (ii = conformity.begin(); ii != conformity.end(); ++ii) {
			fputs(to_string(ii->first).c_str(), of);
			fputs(":", of);
			fputs(to_string(ii->second).c_str(), of);
			fputs("\n", of);
			NewBits += count_of_bits(ii->first) + count_of_bits(ii->second);
		}
		fputs("*\n", of);


		// write a file
		vector<int>::iterator li;
		for (li = ListOfNums.begin(); li != ListOfNums.end(); ++li) {
			if (conformity[*li]) {
				fputs(to_string(conformity[*li]).c_str(), of);
				NewBits += count_of_bits(conformity[*li]);
			}
			else {
				if (reverse_conf[*li]) {
					fputs(to_string(reverse_conf[*li]).c_str(), of);
					NewBits += count_of_bits(reverse_conf[*li]);
				}
				else {
					fputs(to_string(*li).c_str(), of);
					NewBits += count_of_bits(*li);
				}
			}
			fputs("\n", of);
		}

		// clear variables and containers
		ListOfNums.clear();
		ListOfNumObjs.clear();
		m.clear();
		conformity.clear();
		reverse_conf.clear();
		itr = 0;
		if (feof(in)) break;
	}
	fclose(in);
	*O += OldBits;
	*N += NewBits;
	*S += Swaps;
}

// file decompression function
void decompression(string filename, FILE* of, int* O, int* N, int* S) {
	char buf[16];
	int x = 0, OldBits = 0, NewBits = 0, Swaps = 0, a, b;
	unordered_map<int, int> conformity, reverse_conf;
	bool mode = false;

	FILE* in;
	if ((in = fopen(filename.c_str(), "rt")) == NULL) {
		printf("File %s not found!\n", filename.c_str());
		exit(2);
	}

	while (fgets(buf, 16, in)) {
		if (strcmp(buf, "") == 0) continue;
		if (strcmp(buf, "*\n") == 0) { 
			mode = !mode;
			if (mode) conformity.clear();
			else Swaps += conformity.size();
		}
		if (mode) {
			fscanf(in, "\n%d:%d", &a, &b);
			conformity[b] = a;
			reverse_conf[a] = b;
			OldBits += count_of_bits(a) + count_of_bits(b);
		} else {
			if (strcmp(buf, "*\n") == 0) continue;
			x = atoi(buf);
			OldBits += count_of_bits(x);
			if (conformity[x]) {
				fputs(to_string(conformity[x]).c_str(), of);
				fputs("\n", of);
				NewBits += count_of_bits(conformity[x]);
			} else if (reverse_conf[x]) {
				fputs(to_string(reverse_conf[x]).c_str(), of);
				fputs("\n", of);
				NewBits += count_of_bits(reverse_conf[x]);
			} else {
				fputs(to_string(x).c_str(), of);
				fputs("\n", of);
				NewBits += count_of_bits(x);
			}
		}
		if (feof(in)) break;
	}
	fclose(in);
	*O += OldBits;
	*N += NewBits;
	*S += Swaps;
}

static void usage() {
	printf("Usage: ./archivator <filename> <-comp|-decomp>\n");
}

// Counts the bitness of the number
static int count_of_bits(int num) {
	num = num >= 0 ? num : num * -1;
	return ceil(log10(num + 1) / log10(2));
}