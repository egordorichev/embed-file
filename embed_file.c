#include <stdlib.h>
#include <stdio.h>

FILE* open_or_exit(const char* file_name, const char* mode) {
	FILE* f = fopen(file_name, mode);

	if (f == NULL) {
		perror(file_name);
		exit(EXIT_FAILURE);
	}

	return f;
}

int main(int argc, char** argv) {
	if (argc < 3) {
		fprintf(stdout, "USAGE: %s {sym} {rsrc}\n  Creates {sym}.c from the contents of {rsrc}\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char* sym = argv[1];
	FILE* in = open_or_exit(argv[2], "r");

	printf("Embedding file %s...\n", argv[2]);

	char lib_file[256];
	snprintf(lib_file, sizeof(lib_file), "src/lit/std/compiled/%s.c", sym);

	FILE* out = open_or_exit(lib_file, "w");

	fprintf(out, "#include <stdlib.h>\n");
	fprintf(out, "const char %s[] = {\n", sym);

	unsigned char buf[256];
	size_t num_read = 0;
	size_t line_count = 0;

	do {
		num_read = fread(buf, 1, sizeof(buf), in);
		size_t i;

		for (i = 0; i < num_read; i++) {
			fprintf(out, "0x%02x,", buf[i]);

			if (++line_count == 10) {
				fprintf(out, "\n");
				line_count = 0;
			}
		}
	} while (num_read > 0);

	fprintf(out, "0x0,");

	if (line_count > 0) {
		fprintf(out, "\n");
	}

	fprintf(out, "};\n");
	fprintf(out, "const size_t %s_len = sizeof(%s);", sym, sym);

	fclose(in);
	fclose(out);

	return EXIT_SUCCESS;
}