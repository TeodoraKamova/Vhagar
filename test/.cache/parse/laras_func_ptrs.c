

void print_string(char *str);
void c4_free(void *p);
void *c4_malloc(int sz);
void c4_print_str(char *str) { print_string(str); }
void print_int(int d);

int square(int n) { return n * n; }

int increment(int n) { return n + 1; }

int main() {
int (*fp1)(int);
int (**fp2)(int);
int (***fp3)(int);
int (****fp4)(int);
int result;

fp1 = square;
fp1 = &square;
fp1 = &(*square);
fp2 = &fp1;
fp3 = &fp2;
fp4 = &fp3;
fp2 = &(*fp2);
fp2 = &(**fp3);

result = (***fp4)(5);
result = (********fp4)(5);
result = (****fp4)(5);
print_string("Square of 5: ");
print_int(result);
print_string("\n");

fp1    = increment;
result = (***fp4)(5);
result = (***fp4)(5);
result = (****fp4)(5);
result = (********fp4)(5);
print_string("Increment 5: ");
print_int(result);
print_string("\n");

}
