#pragma once

// Structure for keeping track of a balloon's status.
struct balloon_t {
	float x, y; // coordinates
	bool is_yellow; // rotation angle
	bool is_shrinking_and_falling = false; // is set when balloon was hit and is falling and shrinking in size
	float scale = 1; // used for shrinking the balloon until scale <= 0
};

// Structure for keeping track of a shuriken's status.
struct shuriken_t {
	float x, y;	// coordinates
	float angle; // rotation angle
	bool is_falling = false; // is set when shuriken was hit and is falling
};