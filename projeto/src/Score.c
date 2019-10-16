#include "Score.h"

#include <stdlib.h>
// #include <string.h>

Score* newScore(int value) {
	Score* score = (Score*) malloc(sizeof(Score));

	score->value = value;

	return score;
}