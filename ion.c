#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#define MAX(x, y) ((x) >= (y) ? (x) : (y))

// stretchy buffers, invented by sean barrett

typedef struct BufHdr{
	size_t len;
	size_t cap;
	char buf[0]; // [0] new in C99
} BufHdr;

#define buf__hdr(b) ((BufHdr *)(char *)b - offsetof(BufHdr, buf))
#define buf__fits(b, n) (buf_len(b) + (n) <= buf_cap(b))
#define buf__fit(b, n) (buf__fits(b, n) ? 0 : ((b) = (buf__grow((b), buf_len(b) + (n), sizeof(*(b))))))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_push(b, x) (buf__fit(b, 1), b[buf_len(b)] = (x), buf__hdr(b)->len++)
#define buf_free(b) ((b) ? free(buf__hdr(b)) : 0)

void *xrealloc(void *ptr, size_t num_bytes) {
	ptr = realloc(ptr, num_bytes);
	if (!ptr) {
		perror("xrealloc failed");
		exit(1);
	}

	return ptr;
}

void *xmalloc(size_t num_bytes) {
	void *ptr = malloc(num_bytes);
	if (!ptr) {
		perror("malloc failed");
		exit(1);
	}

	return ptr;
}

void *buf__grow(const void *buf, size_t new_len, size_t elem_size) {
	size_t new_cap = MAX(1 + 2 * buf_cap(buf), new_len);
	assert(new_len <= new_cap);
	size_t new_size = offsetof(BufHdr, buf) + new_cap * elem_size;
	BufHdr *new_hdr;
	if (buf) {
		new_hdr = xrealloc(buf__hdr(buf), new_size);
	} else {
		new_hdr = xmalloc(new_size);
		new_hdr->len = 0;
	}

	new_hdr->cap = new_cap;
	return new_hdr->buf;
}

void buf_test() {
	int *buf = NULL;
	enum  { N = 1024 };
	for (int i = 0; i < N; i++) {
		buf_push(buf, i);
	}
	assert(buf_len(buf) == N);
	for (int i = 0; i < buf_len(buf); i++) {
		assert(buf[i] == i);
	}

	buf_free(buf);
}

// lexing: translating char stream to token stream
// e.g. 1234 (x+y) translates into '1234' '(' 'x' '+' 'y' ')'

typedef enum TokenKind {
	TOKEN_INT = 128,
	TOKEN_NAME,
	// ...
} TokenKind;

typedef struct Token {
	TokenKind kind;
	union {
		uint64_t val;
		struct {
			const char *start;
			const char *end;
		};
	};
	// ...
} Token;

Token token;
const char *stream;

void next_token() {
	switch (*stream) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': {
			uint64_t val = 0;
			while (isdigit(*stream)) {
				val *= 10;
				val += *stream++ - '0';
				stream++;
			}
			token.kind = TOKEN_INT;
			token.val = val;
			break;
		}
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
		// case 'a':
		// case 'b':
		// case 'c':
		// case 'd':
		// case 'e':
		// case 'f':
		// case 'g':
		// case 'h':
		// case 'i':
		// case 'j':
		// case 'k':
		// case 'l':
		// case 'm':
		// case 'n':
		// case 'o':
		// case 'p':
		// case 'q':
		// case 'r':
		// case 's':
		// case 't':
		// case 'u':
		// case 'v':
		// case 'w':
		// case 'x':
		// case 'y':
		// case 'z':
		case '_': {
			const char *start = stream++;
			while(isalnum(*stream) || *stream == '_') {
				stream++;
			}
			token.kind = TOKEN_NAME;
			token.start = start;
			token.end = stream;
			break;
		}
		default:
			token.kind = *stream++;
			break;
	}
}

void print_token(Token token) {
	switch(token.kind) {
	case TOKEN_INT:
		printf("TOKEN VALUE: %llu", token.val);
		break;
	case TOKEN_NAME:
		printf("TOKEN NAME: %.*s", token.end - token.start, token.start);
		break;
	default:
		printf("TOKEN '%c'\n", token.kind);
	}
	printf("\n");
}

void lex_test() {
	char *source = "+()_HELLO1,234+FOO!994";
	stream = source;
	next_token();
	while(token.kind) {
		print_token(token);
		next_token();
	}
}

int main(int argc, char **argv) {
	buf_test();
	// lex_test();
	return 0;
}
