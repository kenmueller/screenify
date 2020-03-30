#ifndef Screen_hpp
#define Screen_hpp

#include <iostream>
#include <functional>
#include <string>
#include <regex>
#include <vector>
#include <utility>
#include <assert.h>

#define SCREEN_COORDINATE_PARAMS const size_t &x, const size_t &y

struct Screen {
	enum struct TextWrap {
		None,
		Letters,
		Words
	};
	
	constexpr static size_t DEFAULT_WIDTH = 30;
	constexpr static size_t DEFAULT_HEIGHT = 20;
	
	constexpr static char BLANK_CHAR = ' ';
	
	static Screen *shared;
	
	const size_t &width;
	const size_t &height;
	
	Screen(const size_t &width = DEFAULT_WIDTH, const size_t &height = DEFAULT_HEIGHT);
	
	void set_shared();
	
	bool is_coordinate_valid(SCREEN_COORDINATE_PARAMS);
	bool is_blank(SCREEN_COORDINATE_PARAMS);
	
	bool is_top_border(SCREEN_COORDINATE_PARAMS);
	bool is_bottom_border(SCREEN_COORDINATE_PARAMS);
	bool is_horizontal_border(SCREEN_COORDINATE_PARAMS);
	
	bool is_left_border(SCREEN_COORDINATE_PARAMS);
	bool is_right_border(SCREEN_COORDINATE_PARAMS);
	bool is_vertical_border(SCREEN_COORDINATE_PARAMS);
	
	bool is_border(SCREEN_COORDINATE_PARAMS);
	
	bool is_top_left_corner(SCREEN_COORDINATE_PARAMS);
	bool is_top_right_corner(SCREEN_COORDINATE_PARAMS);
	bool is_bottom_left_corner(SCREEN_COORDINATE_PARAMS);
	bool is_bottom_right_corner(SCREEN_COORDINATE_PARAMS);
	bool is_corner(SCREEN_COORDINATE_PARAMS);
	
	bool is_coordinate_before(
		const size_t &x1,
		const size_t &y1,
		const size_t &x2,
		const size_t &y2
	);
	
	void clear();
	char value_at(SCREEN_COORDINATE_PARAMS);
	
	void write(SCREEN_COORDINATE_PARAMS, const char &c);
	void write_batch(const std::function<char(SCREEN_COORDINATE_PARAMS)> &get_value);
	void write_all(const char &c);
	void write_text(SCREEN_COORDINATE_PARAMS, const std::string &text, const TextWrap &wrap_type);
	
	void outline_border(const char &c = '+', const char &h = '-', const char &v = '|');
	void draw_line(
		const char &c,
		const size_t &x1,
		const size_t &y1,
		const size_t &x2,
		const size_t &y2
	);
	void draw_rect(
		const char &c,
		const size_t &x1,
		const size_t &y1,
		const size_t &x2,
		const size_t &y2,
		const bool fill
	);
private:
	static std::vector<std::string> split_string_on_space(const std::string &s);
	static std::vector<std::pair<size_t, size_t>> *get_points_on_line(
		const long &x1,
		const long &y1,
		const long &x2,
		const long &y2
	);
	
	char *screen_values;
	
	void write_word_wrapping_text(SCREEN_COORDINATE_PARAMS, const std::string &text);
	
	void initialize_screen_values();
	void clear_output();
	
	size_t get_screen_index(SCREEN_COORDINATE_PARAMS);
	void set_screen_value(SCREEN_COORDINATE_PARAMS, const char &c);
};

#endif
