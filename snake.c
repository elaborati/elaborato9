#include "snake.h" 
#include <stdio.h>
#include <stdlib.h>



static int equalPositions(const struct position pos1, const struct position pos2);
static struct position newPosition(struct position pos, enum direction dir, unsigned rows, unsigned cols);
static struct body newBody(struct position pos, struct body* prev, struct body* next);
static struct body* tail(struct snake* s) {return s->body->prev;}

struct snake *snake_create(unsigned int rows, unsigned int cols) {
	struct snake* snake = malloc(sizeof(struct snake));
	snake->rows = rows;
	snake->cols = cols;
	snake->length = 1;

	snake->body = malloc(sizeof(struct body)); /*  */
	struct position randPos; randPos.i = rand() % rows; randPos.j =  rand() % cols;
	*(snake->body) = newBody(randPos, snake->body, NULL);
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
	/*TODO #1 servirebbe avere un puntatore che punta alla coda del snake*/
	/* Allunghiamo il serpente */
	snake_increase(s, dir);

	/* Tagliamo la coda */ 
	snake_decrease(s, 1);
}

void snake_reverse(struct snake *s) { 
	struct body* newTail = s->body;
	struct body* newHead = tail(s);
	s->body->prev = NULL;
	
	struct body* node = s->body;
	while (node != NULL)  {
		struct body* tmp = node->prev;
		node->prev = node->next;
		node->next = tmp;
		node = node->prev;
	}
	s->body = newHead;
	newHead->prev = newTail;
}

void snake_increase(struct snake *s, enum direction dir) {
	struct body* newHead = malloc(sizeof(struct body));
	struct position newPos = newPosition(s->body->pos, dir, s->rows, s->cols);
	*newHead = newBody(newPos, tail(s), s->body);
    s->body->prev = newHead;
    s->body = newHead;
	++(s->length);
}

void snake_decrease(struct snake *s, unsigned int decrease_len) {
	unsigned i = 0;
	if(s != NULL && s->length >= decrease_len)
		for(; i < decrease_len; ++i) {
			struct body* oldTail = s->body->prev;
			struct body* newTail = oldTail->prev;
			newTail->next = NULL;
			s->body->prev = newTail;
			free(oldTail);
			--(s->length);
		}
}


/* Saves the snake into the filename. */
void snake_save(struct snake *s, char *filename) {
    
}

/* Loads the snake from filename */
struct snake *snake_read(char *filename) {
    return NULL;    
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

