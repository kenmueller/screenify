#include <assert.h>

#include "Screen.hpp"

int main() {
	// Top left of screen: (0, 0)
	
	auto s = new Screen(20, 15); // 20x15 screen
	
	s->outline_border(); // Using default params
	
	s->write_text(1, // Starting x
				  1, // Starting y
				  "Hello! This is some wrapped text. The words will not be cut in half!",
				  Screen::TextWrap::Words); // None (Cut), Letters, Words
	
	s->draw_line('~', // Horizontal line
				 1, 5, // Point 1
				 18, 5); // Point 2
	
	s->draw_line('*', // Jagged diagonal line (using Bresenham's algorithm)
				 1, 7, // Point 1
				 18, 13); // Point 2
	
	s->write_text(1, // Starting x
				  6, // Starting y
				  "That line is gross... I'm not touching it",
				  Screen::TextWrap::Words); // None (Cut), Letters, Words
	
	assert(s->is_blank(1, 13)); // Hmm... just wanna make sure I don't ruin anything
	
	s->write(1, 13, '$'); // Writes a specified coordinate (look at the bottom left)
	
	assert(s->value_at(1, 13) == '$'); // Making sure it actually modified the coordinate
	
	s->draw_rect('+', // The fill character
				 1, 10, // Point 1
				 5, 12, // Point 2
				 false); // Fill rectangle
	
	s->draw_rect('#', // The fill character
				 7, 12, // Point 1
				 10, 13, // Point 2
				 true); // Fill rectangle
	
	// More efficient for writing large batches of coordinates.
	// This block is run over all coordinates in the screen, and returns a character.
	// You can get the existing value at a coordinate using the `value_at(x, y)` function.
	// The macro `SCREEN_COORDINATE_PARAMS` is equivalent to `size_t x, size_t y`.
	//
	// s->write_batch([](SCREEN_COORDINATE_PARAMS) {
	//     if (y >= 12)
	//         return '*';
	//
	//     return s->value_at(x, y); // The old value. Efficient because of diff-checking
	// });
}
