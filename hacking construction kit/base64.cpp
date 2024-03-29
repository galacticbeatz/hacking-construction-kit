/*
 * Author: thirdstormofcythraul@outlook.com
 */
#include "base64.h"

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include "memory_debug.h"

unsigned int Base64::mod_table[] = {0, 2, 1};

const char Base64::encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'};


Base64::Base64(): decoding_table(0) {
	this->build_decoding_table();
}

Base64::~Base64(){
	this->base64_cleanup();
}

char *Base64::encode(unsigned char *data, size_t input_length, size_t *output_length) {
	char *encoded_data;
	unsigned int i, j;


	*output_length = (size_t) (4.0 * ceil((double) input_length / 3.0));

	encoded_data = (char *)malloc(*output_length);
	if (encoded_data == 0) return 0;

	for (i = 0, j = 0; i < input_length;) {

		uint32_t octet_a = i < input_length ? data[i++] : 0;
		uint32_t octet_b = i < input_length ? data[i++] : 0;
		uint32_t octet_c = i < input_length ? data[i++] : 0;

		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	for (i = 0; i < mod_table[input_length % 3]; i++)
		encoded_data[*output_length - 1 - i] = '=';

	return encoded_data;
}


unsigned char *Base64::decode(const char *data,	size_t input_length, size_t *output_length) {
	unsigned char *decoded_data;
	unsigned int i, j;

	if (decoding_table == 0) build_decoding_table();

	if (input_length % 4 != 0) return 0;

	*output_length = input_length / 4 * 3;
	if (data[input_length - 1] == '=') (*output_length)--;
	if (data[input_length - 2] == '=') (*output_length)--;

	decoded_data = new unsigned char[(*output_length)];
	if (decoded_data == 0) return 0;

	for (i = 0, j = 0; i < input_length;) {

		uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

		uint32_t triple = (sextet_a << 3 * 6)
			+ (sextet_b << 2 * 6)
			+ (sextet_c << 1 * 6)
			+ (sextet_d << 0 * 6);

		if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
	}

	return decoded_data;
}


void Base64::build_decoding_table() {
	char i;

	decoding_table = (char *)malloc(256);

	for (i = 0; i < 0x40; i++)
		decoding_table[encoding_table[i]] = i;
}


void Base64::base64_cleanup() {
	free(decoding_table);
}