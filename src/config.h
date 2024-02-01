#ifndef CONFIG_H
#define CONFIG_H

// Times per second
#define UPDATE_FREQUENCY 60

#define INITIAL_DOWN_INTERVAL 1.0f
#define PIECES_FOR_SPEED_INCREASE 10
#define DOWN_INTERVAL_VARIATION 0.05f
#define MAX_DOWN_INTERVAL 0.5f
#define DOWN_INTERVAL_DECREMENT 1
#define SCORE_PER_PIECE 25
#define SCORE_PER_LINES(lines) 100 * (1 << lines)
#define WALL_COLOR 0.9f

#endif /* CONFIG_H */
