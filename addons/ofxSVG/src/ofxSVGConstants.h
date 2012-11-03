/*
 *  ofxSVGConstants.h
 *  
 *
 *  Created by base on 26/09/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma mark once

namespace ofxSVGConstant {

	enum asciiTypes{

	//! @brief Mask for all lowercase characters: <code>abcdefghijklmnopqrstuvwxyz</code>.
	ASCII_CLASS_LOWER = 0x01,
	//! @brief Mask for all uppercase characters: <code>ABCDEFGHIJKLMNOPQRSTUVWXYZ</code>.
	ASCII_CLASS_UPPER = 0x02,
	//! @brief Mask for all hex characters except digits: <code>abcdefABCDEF</code>.
	ASCII_CLASS_XALPHA = 0x04,
	//! @brief Mask for all ASCII digits: <code>0123456789</code>
	ASCII_CLASS_DIGIT = 0x08,
	//! @brief Mask for underscore character: <code>_</code>
	ASCII_CLASS_UNDERSCORE = 0x10,
	//! @brief Mask for all white-spaces: <code>' ', '\\t', '\\n', '\\r', '\\f', '\\v'</code>
	ASCII_CLASS_SPACE = 0x20,
	//! @brief Mask for all punctuation characters.
	ASCII_CLASS_PUNCT = 0x40,
	//! @brief Mask for all blank characters: <code>' ', '\\t'</code>
	ASCII_CLASS_BLANK = 0x80,

	//! @brief Mask for all alpha characters (@c ASCII_CLASS_LOWER | @c ASCII_CLASS_UPPER).
	ASCII_CLASS_ALPHA = ASCII_CLASS_LOWER | ASCII_CLASS_UPPER,
	//! @brief Mask for all alpha characters or underscore (@c ASCII_CLASS_ALPHA | @c ASCII_CLASS_UNDERSCORE).
	ASCII_CLASS_ALPHA_ = ASCII_CLASS_ALPHA | ASCII_CLASS_UNDERSCORE,
	//! @brief Mask for all alpha and numeric characters (@c ASCII_CLASS_ALPHA | @c ASCII_CLASS_DIGIT).
	ASCII_CLASS_ALNUM = ASCII_CLASS_ALPHA | ASCII_CLASS_DIGIT,
	//! @brief Mask for all graphical characters (@c ASCII_CLASS_ALNUM | @c ASCII_CLASS_PUNCT).
	ASCII_CLASS_GRAPH = ASCII_CLASS_ALNUM | ASCII_CLASS_PUNCT,
	//! @brief Mask for all printable characters (@c ASCII_CLASS_GRAPH | @c ASCII_CLASS_BLANK).
	ASCII_CLASS_PRINT = ASCII_CLASS_GRAPH | ASCII_CLASS_BLANK,
	//! @brief Mask for all hex-digit characters (0-9, A-F, a-f).
	ASCII_CLASS_XDIGIT = ASCII_CLASS_DIGIT | ASCII_CLASS_XALPHA

	};

};