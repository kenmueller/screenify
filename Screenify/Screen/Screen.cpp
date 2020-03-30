#include "Screen.hpp"

Screen *Screen::shared = nullptr;

Screen::Screen(const size_t &width, const size_t &height)
	: width(width)
	, height(height)
	, screen_values(new char[width * height]) {
		initialize_screen_values();
	}

bool Screen::is_coordinate_valid(SCREEN_COORDINATE_PARAMS) {
	return x >= 0 && x < width && y >= 0 && y < height;
}

bool Screen::is_blank(SCREEN_COORDINATE_PARAMS) {
	return value_at(x, y) == BLANK_CHAR;
}

bool Screen::is_top_border(SCREEN_COORDINATE_PARAMS) {
	return !y;
}

bool Screen::is_bottom_border(SCREEN_COORDINATE_PARAMS) {
	return y == height - 1;
}

bool Screen::is_horizontal_border(SCREEN_COORDINATE_PARAMS) {
	return is_top_border(x, y) || is_bottom_border(x, y);
}

bool Screen::is_left_border(SCREEN_COORDINATE_PARAMS) {
	return !x;
}

bool Screen::is_right_border(SCREEN_COORDINATE_PARAMS) {
	return x == width - 1;
}

bool Screen::is_vertical_border(SCREEN_COORDINATE_PARAMS) {
	return is_left_border(x, y) || is_right_border(x, y);
}

bool Screen::is_border(SCREEN_COORDINATE_PARAMS) {
	return is_horizontal_border(x, y) || is_vertical_border(x, y);
}

bool Screen::is_top_left_corner(SCREEN_COORDINATE_PARAMS) {
	return !(x || y);
}

bool Screen::is_top_right_corner(SCREEN_COORDINATE_PARAMS) {
	return x == width - 1 && !y;
}

bool Screen::is_bottom_left_corner(SCREEN_COORDINATE_PARAMS) {
	return x == width - 1 && y == height - 1;
}

bool Screen::is_bottom_right_corner(SCREEN_COORDINATE_PARAMS) {
	return !x && y == height - 1;
}

bool Screen::is_corner(SCREEN_COORDINATE_PARAMS) {
	return (
		is_top_left_corner(x, y) ||
		is_top_right_corner(x, y) ||
		is_bottom_left_corner(x, y) ||
		is_bottom_right_corner(x, y)
	);
}

bool Screen::is_coordinate_before(
	const size_t &x1,
	const size_t &y1,
	const size_t &x2,
	const size_t &y2
) {
	return y1 < y2 || (y1 == y2 && x1 < x2);
}

void Screen::clear() {
	write_all(BLANK_CHAR);
}

char Screen::value_at(SCREEN_COORDINATE_PARAMS) {
	assert(is_coordinate_valid(x, y));
	
	return screen_values[get_screen_index(x, y)];
}

void Screen::write(SCREEN_COORDINATE_PARAMS, const char &c) {
	if (c == value_at(x, y))
		return; // No changes needed
	
	clear_output();
	
	for (size_t _y = 0; _y < height; _y++) {
		for (size_t _x = 0; _x < width; _x++)
			std::cout << (_x == x && _y == y ? c : value_at(_x, _y));
		
		std::cout << '\n';
	}
	
	set_screen_value(x, y, c);
}

void Screen::write_batch(const std::function<char(SCREEN_COORDINATE_PARAMS)> &get_value) {
	clear_output();
	
	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			const char &c = get_value(x, y);
			
			std::cout << c;
			
			if (c != value_at(x, y))
				set_screen_value(x, y, c);
		}
		
		std::cout << '\n';
	}
}

void Screen::write_all(const char &c) {
	write_batch([&c](SCREEN_COORDINATE_PARAMS) {
		return c;
	});
}

void Screen::write_text(SCREEN_COORDINATE_PARAMS, const std::string &text, const TextWrap &wrap_type) {
	if (wrap_type == TextWrap::Words)
		return write_word_wrapping_text(x, y, text);
	
	const bool wrap = wrap_type == TextWrap::Letters;
	
	size_t i = 0;
	bool done = false;
	
	write_batch([this, &x, &y, &text, &wrap, &i, &done](const size_t &_x, const size_t &_y) {
		const char &old_value = value_at(_x, _y);
		
		if (done)
			return old_value;
		
		if (i >= text.length()) {
			done = true;
			return old_value;
		}
		
		if (is_coordinate_before(_x, _y, x, y))
			return old_value;
		
		if (old_value == BLANK_CHAR)
			return text.at(i++);
		
		if (!wrap)
			done = true;
		
		return old_value;
	});
}

void Screen::outline_border(const char &c, const char &h, const char &v) {
	write_batch([this, &c, &h, &v](SCREEN_COORDINATE_PARAMS) {
		if (is_corner(x, y))
			return c;
		
		if (is_horizontal_border(x, y))
			return h;
		
		if (is_vertical_border(x, y))
			return v;
		
		return value_at(x, y);
	});
}

void Screen::draw_line(
	const char &c,
	const size_t &x1,
	const size_t &y1,
	const size_t &x2,
	const size_t &y2
) {
	const auto points = get_points_on_line(x1, y1, x2, y2);
	
	if (!points->size())
		return;
	
	bool done = false;
	
	auto next_point = points->at(0);
	size_t i = 0;
	
	write_batch([this, &c, &points, &done, &next_point, &i](SCREEN_COORDINATE_PARAMS) {
		if (done || !(x == next_point.first && y == next_point.second))
			return value_at(x, y);
		
		if (++i < points->size())
			next_point = points->at(i);
		else
			done = true;
		
		return c;
	});
}

void Screen::draw_rect(
	const char &c,
	const size_t &x1,
	const size_t &y1,
	const size_t &x2,
	const size_t &y2,
	const bool fill
) {
	write_batch([&](SCREEN_COORDINATE_PARAMS) {
		const bool is_between_x = (x >= x1 && x <= x2) || (x >= x2 && x <= x1);
		const bool is_between_y = (y >= y1 && y <= y2) || (y >= y2 && y <= y1);
		
		if (
			fill
				? is_between_x && is_between_y
				: (
					((x == x1 || x == x2) && is_between_y) ||
					((y == y1 || y == y2) && is_between_x)
				)
		)
			return c;
		
		return value_at(x, y);
	});
}

std::vector<std::string> Screen::split_string_on_space(const std::string &s) {
	const std::regex r("\\s+");
	
	return { std::sregex_token_iterator(s.begin(), s.end(), r, -1), {} };
}

std::vector<std::pair<size_t, size_t>> *Screen::get_points_on_line(
	const long &x1,
	const long &y1,
	const long &x2,
	const long &y2
) {
	auto acc = new std::vector<std::pair<size_t, size_t>>();
	
	long m_new = 2 * (y2 - y1);
	long slope_error_new = m_new - (x2 - x1);
	
	for (long x = x1, y = y1; x <= x2; x++) {
		acc->push_back({ x, y });
		
		slope_error_new += m_new;
		
		if (slope_error_new >= 0) {
			y++;
			slope_error_new -= 2 * (x2 - x1);
		}
	}
	
	return acc;
}

void Screen::write_word_wrapping_text(SCREEN_COORDINATE_PARAMS, const std::string &text) {
	const std::vector<std::string> &parts = split_string_on_space(text);
	
	if (!parts.size())
		return;
	
	std::string part = parts.at(0);
	
	size_t i = 0;
	size_t j = 0;
	
	bool done = false;
	
	write_batch([this, &x, &y, &text, &parts, &part, &i, &j, &done](const size_t &_x, const size_t &_y) {
		if (done || is_coordinate_before(_x, _y, x, y))
			return value_at(_x, _y); // If you're done or before the desired starting point
		
		size_t available_chars = 0;
		
		while (value_at(_x + available_chars, _y) == BLANK_CHAR)
			available_chars++;
		
		size_t remaining_chars = part.length() - j;
		
		if (!remaining_chars) {
			if (++i >= parts.size()) {
				done = true;
				return value_at(_x, _y);
			}
			
			part = parts.at(i);
			j = 0;
			
			return value_at(_x, _y);
		}
		
		if (remaining_chars > available_chars)
			return value_at(_x, _y);
		
		return part.at(j++);
	});
}

void Screen::initialize_screen_values() {
	for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
			set_screen_value(x, y, BLANK_CHAR);
}

void Screen::clear_output() {
#if _WIN64 || _WIN32
	system("cls");
#else
	system("clear");
#endif
}

size_t Screen::get_screen_index(SCREEN_COORDINATE_PARAMS) {
	return y * width + x;
}

void Screen::set_screen_value(SCREEN_COORDINATE_PARAMS, const char &c) {
	screen_values[get_screen_index(x, y)] = c;
}
