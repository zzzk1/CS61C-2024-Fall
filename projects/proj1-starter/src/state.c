#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

void create_default_snake(game_state_t *state);
void init_default_board(game_state_t *state);
char get_next_sign(game_state_t *state, snake_t *snake);

game_state_t *create_default_state() {
  game_state_t *state = malloc(sizeof(game_state_t));
  state->num_rows = 18;

  // allow in heap
  state->board = malloc((state->num_rows) * sizeof(char*));
  for (int i = 0; i < state->num_rows; i++) {
    state->board[i] = (char*)malloc(21*sizeof(char));
    memset(state->board[i], ' ', 20);
    state->board[i][20] = '\0';
  }

  init_default_board(state);

  //fruit here
  set_board_at(state, 2, 9, '*');

  //snake here
  state->num_snakes = 1;
  create_default_snake(state);

  return state;
}

void init_default_board(game_state_t *state) {
  //init first row and last row.
  for (int col = 0; col < 20; col++) {
    state->board[0][col] = '#';
  }

  strcpy(state->board[state->num_rows-1], state->board[0]);

  //others.
  for (int row = 1; row < state->num_rows - 1; row++) {
    state->board[row][0] = '#';
    state->board[row][19] = '#';
  }

  for (int row = 2; row < state->num_rows - 1; row++) {
    strcpy(state->board[row], state->board[row-1]);
  }  
}

void create_default_snake(game_state_t *state) {
  snake_t *snake = malloc(sizeof(snake_t));
  snake->live = true;
  snake->head_row = 2;
  snake->head_col = 4;
  snake->tail_row = 2;
  snake->tail_col = 2;
  state->snakes = snake;

  //put on board
  set_board_at(state, snake->head_row, snake->head_col, 'D');
  set_board_at(state, snake->head_row, snake->head_col-1, '>');
  set_board_at(state, snake->tail_row, snake->tail_col, 'd');
}

/* Task 2 */
void free_state(game_state_t *state) {

  for (int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }

  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  for (int row = 0; row < state->num_rows; row++) {
    fprintf(fp, "%s\n", state->board[row]);
  }
  
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  if (c == 'w' || c == 's' || c == 'a' || c == 'd') {
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  if (c == 'W' || c == 'S' || c == 'A' || c == 'D' || c == 'x') {
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  if (is_tail(c) || is_head(c)) {
    return true;
  }
  else if (c == '^' || c == '<' || c == 'v' || c == '>') {
    return true;
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  if (c == '^') {
    return 'w';
  }
  else if (c == '<') {
    return 'a';
  }
  else if (c == 'v') {
    return 's';
  }
  else if (c == '>') {
    return 'd';
  }
  return ' ';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  if (c == 'W') {
    return '^';
  }
  else if (c == 'A') {
    return '<';
  }
  else if (c == 's') {
    return 'v';
  }
  else if (c == 'D') {
    return '>';
  }
  return ' ';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  }
  else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  }
  else {
      return cur_row;
  }
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  }
  else if (c == '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  } else {
    return cur_col;
  }
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  snake_t* snake = state->snakes + snum;
 
  return get_next_sign(state, snake);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  snake_t *snake = state->snakes + snum;
  
  char head = get_board_at(state, snake->head_row, snake->head_col);
  unsigned int next_row = get_next_row(snake->head_row, head);
  unsigned int next_col = get_next_col(snake->head_col, head);

  // update board
  char body_increment = head_to_body(head);
  set_board_at(state, snake->head_row, snake->head_col, body_increment);
  set_board_at(state, next_row, next_col, head);

  // update snake
  snake->head_row = next_row;
  snake->head_col = next_col;

  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  snake_t *snake = state->snakes + snum;
  
  char tail = get_board_at(state, snake->tail_row, snake->tail_col);
  unsigned int next_row = get_next_row(snake->tail_row, tail);
  unsigned int next_col = get_next_col(snake->tail_col, tail);

  // update board
  char body = get_board_at(state, next_row, next_col);
  char new_tail = body_to_tail(body);
  set_board_at(state, snake->tail_row, snake->tail_col, ' ');
  set_board_at(state, next_row, next_col, new_tail);

  // update snake
  snake->tail_row = next_row;
  snake->tail_col = next_col;

  return;
}

char get_next_sign(game_state_t *state, snake_t *snake) {
  char head = get_board_at(state, snake->head_row, snake->head_col);
  unsigned int next_row = get_next_row(snake->head_row, head);
  unsigned int next_col = get_next_col(snake->head_col, head);

  return get_board_at(state, next_row, next_col);
}

bool is_furit(game_state_t *state, snake_t *snake) {
  return get_next_sign(state, snake) == '*';
}

bool is_crash(game_state_t *state, snake_t *snake) {
  return get_next_sign(state, snake) != ' ';
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  for (int i = 0; i < state->num_snakes; i++) {
    snake_t *snake = state->snakes + (long unsigned int)i * sizeof(snake_t);
    if (!snake->live) {
      continue;
    }
    if (is_furit(state, snake)) {
      update_head(state, (unsigned int)i);
      add_food(state);
      continue;
    }
    if (is_crash(state, snake)) {
      snake->live = false;
      set_board_at(state, snake->head_row, snake->head_col, 'x');
      continue;
    }
    
    update_head(state, (unsigned int)i);
    update_tail(state, (unsigned int)i);
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  char line[100];
  char *result = fgets(line, sizeof(line), fp);

  if (result == NULL) {
    return NULL;
  }

  size_t len = strlen(line);
  result = malloc(len + 1 * sizeof(char));
  
  strcpy(result, line);
  result[len] = '\0';
  return result;
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  char *line;
  game_state_t *result = malloc(sizeof(game_state_t));

  int rows = 0;
  while ((line = read_line(fp)) != NULL) {
    if (result->board == NULL) {
      result->board = malloc((rows + 1) * sizeof(char*));
    } else {
      result->board = realloc(result->board, (rows + 1) * sizeof(char*));
    }
    
    result->board[rows] = malloc(strlen(line) * sizeof(char));

    strncpy(result->board[rows], line, strlen(line)-1);
    result->board[rows][strlen(line)-1] = '\0';
    rows++;
  }

  result->num_rows = rows;
  result->snakes = NULL;
  result->num_snakes = 0;
  return result;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  snake_t *snake = state->snakes + snum;

  char tail_to_head = get_board_at(state, snake->tail_row, snake->tail_col);
  unsigned int next_row = snake->tail_row;
  unsigned int next_col = snake->tail_col;

  while (!is_head(tail_to_head)) {
    next_row = get_next_row(next_row, tail_to_head);
    next_col = get_next_col(next_col, tail_to_head);
    tail_to_head = get_board_at(state, next_row, next_col);
  } 

  snake->head_row = next_row;
  snake->head_col = next_col;
  return;
}
     
/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  for (unsigned int row = 1; row < state->num_rows - 1; row++) {
      for (unsigned int col = 1; col < sizeof(state->board[row]); col++) {
        if (is_tail(get_board_at(state, row, col))) {
          snake_t *snake = malloc(sizeof(snake_t));
          snake->tail_row = row;
          snake->tail_col = col;
          state->snakes = realloc(snake, sizeof(snake_t) * (state->num_snakes + 1));
          find_head(state, state->num_snakes);
          state->num_snakes++;
        }
      }
  }
  return state;
}
