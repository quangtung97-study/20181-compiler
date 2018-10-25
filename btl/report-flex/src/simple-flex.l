    int num_lines = 0, num_chars = 0;
%%
\n      { ++num_lines; ++num_chars; return 1; }
.       { ++num_chars; return 2; }
%%

int yywrap() {
    return 1;
}

int main() {
    while (yylex() != 0) {}
    printf("So dong: %d, so ki tu: %d\n", num_lines, num_chars);
}
