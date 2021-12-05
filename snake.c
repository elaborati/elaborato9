#include "snake.h" 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

static int equalPositions(struct position pos1, struct position pos2);
static struct position newPosition(struct position pos, enum direction dir, unsigned rows, unsigned cols);
static struct body newBody(struct position pos, struct body* prev, struct body* next);
static struct body* getTail(struct snake* s);
static void snakeIncreaseByPosition(struct snake* s, struct position pos);
static void firstPositionForSnake(struct snake* s, struct position firstPos);
static void snakeAppend(struct snake* s, struct position pos);

struct snake *snake_create(unsigned int rows, unsigned int cols) {
	struct snake* snake = malloc(sizeof(struct snake));
	snake->rows = rows;
	snake->cols = cols;
	snake->length = 0;

    srand(time(NULL));
	struct position randPos; randPos.i = rand() % rows; randPos.j =  rand() % cols;
    firstPositionForSnake(snake, randPos);
	return snake;
} 

void snake_kill(struct snake *s) {	
	struct body* prev = s->body;
	struct body* next;
	while (prev != NULL) {
		next = prev->next;
	 	free(prev);
		prev = next;
	}
	free(s);
	s = NULL;
}

struct position snake_head(struct snake *s) {
	struct position p = s->body->pos;
	return p;

}

struct position snake_body(struct snake *s, unsigned int i) {
	if (i < s->length) {
		unsigned n = 0;
		struct body* node = s->body;
		while (node != NULL) {
			if(n == i)
				return node->pos;
			node = node->next;
			++n; 
		}
	}
	struct position unkPos;
	unkPos.i = 0; unkPos.j = 0;
	return unkPos;
}

int snake_knotted(struct snake *s) {
	if (s != NULL && s->body != NULL) {
		struct position headPos = s->body->pos;
		struct body* node = s->body->next; /* Iniziamo a controllare da il nodo successivo alla testa */
		while (node != NULL) {
			if(equalPositions(node->pos, headPos))
				return 1;
			node = node->next; 
		}
	}
	
	return 0;
}

void snake_move(struct snake *s, enum direction dir) {
	/* Allunghiamo il serpente */
	snake_increase(s, dir);

	/* Tagliamo la coda */ 
	snake_decrease(s, 1);
}

void snake_reverse(struct snake *s) { 
	/*struct body* newTail = s->body;*/
	struct body* newHead = getTail(s);
	s->body->prev = NULL;
	
	struct body* node = s->body;
	while (node != NULL)  {
		struct body* tmp = node->prev;
		node->prev = node->next;
		node->next = tmp;
		node = node->prev;
	}
	s->body = newHead;
	newHead->prev = NULL;	
}

void snake_increase(struct snake *s, enum direction dir) {
	snakeIncreaseByPosition(s, newPosition(s->body->pos, dir, s->rows, s->cols));
}

void snake_decrease(struct snake *s, unsigned int decrease_len) {
	unsigned i = 0;
	if(s != NULL && s->length >= decrease_len)
		for(; i < decrease_len; ++i) {
			struct body* oldTail = getTail(s);
			struct body* newTail = oldTail->prev;
			newTail->next = NULL;
			s->body->prev = NULL;
			free(oldTail);
			--(s->length);
		}
}


/* Saves the snake into the filename. */
void snake_save(struct snake *s, char *filename) {
    FILE* file = fopen(filename, "w");
	if (file != NULL) {
		printf("File is opened in save\n");

		fprintf(file, "%u %u %u ", s->length, s->rows, s->cols);

		struct body* node = s->body;
		unsigned i = 0;
		for(; i < s->length && node != NULL; ++i) {
			fprintf(file, "%u %u ", node->pos.i, node->pos.j);
			node = node->next;
		}
	}
	fclose(file);
}

/* Loads the snake from filename */
struct snake *snake_read(char *filename) {
	FILE* file = fopen(filename, "r");
	//printf(strerror(errno));
    struct snake* s = malloc(sizeof(struct snake));
    if (file != NULL) {
        unsigned length, rows, cols;
		fscanf(file, "%u %u %u ", &length, &rows, &cols);
		unsigned i = 0;
		for(; i < length; ++i) {
			struct position pos;
			if(fscanf(file, "%u %u ", &(pos.i), &(pos.j)) != EOF) {
                if (i == 0)
                    firstPositionForSnake(s, pos);
                else
                    snakeAppend(s, pos);
            }
		}
        s->length = length;
        s->rows = rows;
        s->cols = cols;
        fclose(file);
    }
    return s;
}

static int equalPositions(const struct position pos1, const struct position pos2) {
	return (pos1.i == pos2.i && pos1.j == pos2.j);
}

static struct position newPosition(struct position pos, enum direction dir, unsigned rows, unsigned cols) {
    switch (dir) {
        case UP: pos.i = (pos.i+(rows-1)) % rows; break;
        case DOWN: pos.i = (pos.i+1) % rows; break;
        case RIGHT: pos.j = (pos.j+1) % cols; break;
        case LEFT: pos.j = (pos.j+(cols-1)) % cols; break;
        default: break;
    }
    return pos;
}

static struct body newBody(struct position pos, struct body* prev, struct body* next) {
	struct body b;
	b.pos = pos;
	b.next = next;
	b.prev = prev;
	return b;
}

static void snakeIncreaseByPosition(struct snake* s, struct position pos) {
	struct body* newHead = malloc(sizeof(struct body));
	*newHead = newBody(pos, NULL, s->body);
    s->body->prev = newHead;
    s->body = newHead;
	++(s->length);
}

static struct body* getTail(struct snake* s) {
	struct body* node = s->body;
	while (node->next != NULL)
		node = node->next;
	return node;
}

static void firstPositionForSnake(struct snake* s, struct position firstPos) {
	struct body* newHead = malloc(sizeof(struct body));
	*newHead = newBody(firstPos, NULL, NULL);
    s->body = newHead;
    ++(s->length);
}

static void snakeAppend(struct snake* s, struct position pos) {
    struct body* oldTail = getTail(s);
    oldTail->next = malloc(sizeof(struct body));
    *(oldTail->next) = newBody(pos, oldTail, NULL);
    ++(s->length);
}
