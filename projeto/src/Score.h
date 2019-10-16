#ifndef _SCORE_H
#define _SCORE_H

/** @defgroup Mario Mario
 * @{
 * Functions to get the current day, month or year from the RTC
 */

/*! Represents the score */
typedef struct {
	
	int value;    /*! value number of pipes passed = score */

	char name[4]; /*! array of the player name */


	unsigned long day; /*! day when game was Played*/

	unsigned long month; /*! month when  game was Played */

	unsigned long year; /*! year when game was played */



} Score;

/**
 * @ brief creates socre
 * @param value score
 */
Score* newScore(int value);


/**@}*/

#endif
