/*
 * ascii_string.h
 *
 */

#ifndef ASCII_STRING_H_
#define ASCII_STRING_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <ctype.h>

static inline uint32_t ascii_to_int(char ascii)
{
	if ((ascii >= '0') && (ascii <= '9')) {
		return (uint32_t)(ascii - '0');
	}

	return 0;
}

static inline char int_to_ascii(uint32_t value)
{
	if (value <= 9) {
		return (char)('0' + value);
	}

	return '0';
}

static inline uint32_t ascii_to_hex(char ascii)
{
	if ((ascii >= '0') && (ascii <= '9')) {
		return (uint32_t)(ascii - '0');
	}
	if ((ascii >= 'A') && (ascii <= 'F')) {
		return (uint32_t)(ascii - 'A') + 10;
	}
	if ((ascii >= 'a') && (ascii <= 'f')) {
		return (uint32_t)(ascii - 'a') + 10;
	}

	return 0xFFu;
}

static inline char hex_to_ascii(uint32_t value)
{
	if (value <= 9) {
		return (char)('0' + value);
	}
	if (value <= 0x0F) {
		return (char)('A' + (value - 10));
	}

	return '0';
}

static inline bool ascii_equal_nocase(const char *a, const char *b)
{
	if ((a == NULL) || (b == NULL)) {
		return false;
	}

	while ((*a != '\0') && (*b != '\0')) {
		if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
			return false;
		}
		a++;
		b++;
	}

	return ((*a == '\0') && (*b == '\0'));
}

static inline uint32_t ascii_to_decimal(const char *ascii, bool *ok)
{
	uint32_t result = 0;
	uint32_t digits = 0;

	if (ok != NULL) {
		*ok = false;
	}
	if (ascii == NULL) {
		return 0u;
	}

	while (*ascii == ' ') {
		ascii++;
	}

	while ((*ascii >= '0') && (*ascii <= '9')) {
		result = (result * 10) + (uint32_t)(*ascii - '0');
		ascii++;
		digits++;
		if (digits > 10) {
			return 0;
		}
	}

	if (digits == 0) {
		return 0; 
	}
	if ((*ascii != '\0') && (*ascii != ' ')) {
		return 0; 
	}

	if (ok != NULL) {
		*ok = true;
	}

	return result;
}

static inline int32_t ascii_to_signed_decimal(const char *ascii, bool *ok)
{
	bool     parsed = false;
	bool     minus  = false;
	uint32_t value;

	if (ok != NULL) {
		*ok = false;
	}
	if (ascii == NULL) {
		return 0;
	}

	if (*ascii == '-') {
		minus = true;
		ascii++;
	} else if (*ascii == '+') {
		ascii++;
	}

	value = ascii_to_decimal(ascii, &parsed);
	if (!parsed || (value > 0x7FFFFFFFu)) {
		return 0;
	}

	if (ok != NULL) {
		*ok = true;
	}

	return minus ? -(int32_t)value : (int32_t)value;
}

static inline uint32_t ascii_to_integer(const char *ascii, bool *ok)
{
	uint32_t result = 0;
	uint32_t digits = 0;

	if (ok != NULL) {
		*ok = false;
	}
	if (ascii == NULL) {
		return 0;
	}

	while (*ascii == ' ') {
		ascii++;
	}

	if ((ascii[0] == '0') && ((ascii[1] == 'x') || (ascii[1] == 'X'))) {
		ascii += 2;
	}

	for (;;) {
		const uint32_t digit = ascii_to_hex(*ascii);

		if (digit == 0xFFu) {
			break;
		}

		result = (result << 4) | digit;
		ascii++;
		digits++;
		if (digits > 8u) {
			return 0;
		}
	}

	if (digits == 0) {
		return 0;
	}
	if ((*ascii != '\0') && (*ascii != ' ')) {
		return 0;
	}

	if (ok != NULL) {
		*ok = true;
	}

	return result;
}

static inline bool ascii_to_on_off(const char *ascii, bool *on)
{
	if ((ascii == NULL) || (on == NULL)) {
		return false;
	}

	if (ascii_equal_nocase(ascii, "on") || ascii_equal_nocase(ascii, "1")
			|| ascii_equal_nocase(ascii, "true")) {
		*on = true;
		return true;
	}

	if (ascii_equal_nocase(ascii, "off") || ascii_equal_nocase(ascii, "0")
			|| ascii_equal_nocase(ascii, "false")) {
		*on = false;
		return true;
	}

	return false;
}

static inline uint32_t ascii_to_hex_array(uint8_t *output, const char *input,
		uint32_t max_size)
{
	uint32_t count = 0;

	if ((output == NULL) || (input == NULL)) {
		return 0u;
	}

	while ((input[0] != '\0') && (count < max_size)) {
		const uint32_t high = ascii_to_hex(input[0]);
		const uint32_t low  = ascii_to_hex(input[1]);

		if ((high == 0xFFu) || (low == 0xFFu)) {
			return 0u; 
		}

		output[count] = (uint8_t)((high << 4) | low);
		count++;
		input += 2;
	}

	return count;
}

#endif /* ASCII_STRING_H_ */
